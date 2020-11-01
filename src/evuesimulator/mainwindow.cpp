#include "mainwindow.h"
#include <QtCore>
#include "transparentdialog.h"


MainWindow::MainWindow(const QString& appdir, QWidget *parent)
    :QWidget(parent),
    m_appdir(appdir)
{
    setFixedHeight(626);
    setFixedWidth(530);
    setWindowTitle("EVM OpenHarmony JS");
    setWindowIcon(QIcon(":/skin/logo.png"));
    startEVUE(m_appdir);
    initUI();
    initConnect();

    m_timer = new QTimer;
    m_timer->setSingleShot(true);
    m_timer->setInterval(15000);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::closeEVUE);
}

void MainWindow::initUI()
{
    m_bgwidget = new TransparentWidget(this);
    m_bgwidget->setWindowFlag(Qt::Tool);
    m_bgwidget->show();

#if defined(Q_OS_WIN)
    m_containerWindow = QWindow::fromWinId((WId)m_hwnd);
    m_containerWidget = QWidget::createWindowContainer(m_containerWindow, this);
    m_containerWidget->setFixedSize(454, 454);
    m_containerWidget->move((this->width() - m_containerWidget->width())/2, (this->height() - m_containerWidget->height())/2);
#else
    QStringList args;
    args << "evue.exe";
    args << appdir;
    m_embedApp = new EmbedApp(args, "", 0, 0);
    connect(m_embedApp, &EmbedApp::windowReady, this, &MainWindow::onWindowReady);
    m_embedApp->start();
#endif
    m_timer->start();
}

void MainWindow::initConnect()
{
    connect(m_timer, &QTimer::timeout, this, &MainWindow::closeEVUE);
}

void MainWindow::startEVUE(const QString& appdir)
{
    m_timer = new QTimer;
    m_timer->setSingleShot(true);
    m_timer->setInterval(5000);
    QDir dir(appdir);
    if (dir.exists()){
        m_process = new QProcess;
        QStringList args;
        args << appdir;
        m_process->start("evue.exe", args);
        QThread::sleep(1);
#if defined(Q_OS_WIN)
        m_hwnd = GetProcessMainWnd((DWORD)m_process->processId());
#endif
    }
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEVUE()
{
    m_process->terminate();
}

void MainWindow::onWindowReady()
{
    m_containerWindow = m_embedApp->getWindow();
    m_containerWidget = QWidget::createWindowContainer(m_containerWindow, this);
    m_containerWidget->setFixedSize(454, 454);
    m_containerWidget->move((this->width() - m_containerWidget->width())/2, (this->height() - m_containerWidget->height())/2);
}

void MainWindow::paintEvent(QPaintEvent* p)
{
    (void)p;
}

void MainWindow::showEvent(QShowEvent *event)
{
    m_bgwidget->show();
    m_bgwidget->move(this->geometry().topLeft());
    QWidget::showEvent(event);
}

bool MainWindow::event(QEvent *event){
    if (event->type() == QEvent::Move){
        m_bgwidget->move(this->geometry().topLeft());
        m_bgwidget->show();
    }else if (event->type() == QEvent::Hide){
        m_bgwidget->hide();
    }else if (event->type() == QEvent::Show){
        m_bgwidget->show();
    }
    return QWidget::event(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    QDir dir(m_appdir);
#if defined(Q_OS_WIN)
    if (dir.exists() && m_process->processId() > 0){
        QStringList args;
        args << "/F";
        args << "/PID";
        args << QString::number(m_process->processId());
        QProcess pkill;
        pkill.startDetached("taskkill", args);
        qDebug() << "taskkill" << args;
    }
#endif
    m_bgwidget->close();
}


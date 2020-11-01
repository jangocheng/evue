#include "mainwindow.h"
#include <QTimer>
#include <QPainter>
#include <QThread>
#include <QMouseEvent>
#include "lvgl_main.h"
#include "evm_main.h"
#include "lvgl.h"

class GuiThread: public QThread
{
public:
    GuiThread(char * appdir){
        m_appdir = appdir;
    }
private:
    void run()
    {
        evm_main(m_appdir);
    }
    char * m_appdir;
};

MainWindow::MainWindow(char * file, QWidget *parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_NoBackground);
    setFixedHeight(626);
    setFixedWidth(530);
    setWindowTitle("EVM OpenHarmony JS");
    setWindowIcon(QIcon(":/skin/logo.png"));

    GuiThread* thread = new GuiThread(file);
    thread->start();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(30);//30ms
}

MainWindow::~MainWindow()
{

}

void MainWindow::paintEvent(QPaintEvent* p)
{
    (void)p;
    uchar * fb = (uchar*)evm_get_fb();
    if(fb){
        QPainter painter(this);
        QImage img(fb, LV_HOR_RES_MAX, LV_VER_RES_MAX, QImage::Format_RGB16);
        painter.drawImage((width()-LV_HOR_RES_MAX)/2, (height()-LV_VER_RES_MAX)/2, img);
        QPixmap pix;
        painter.translate(0, 0);
        pix.load(":/skin/gt2.png");//加载图片地址 可以是资源文件
        painter.drawPixmap(0, 0, width(), height(), pix);//绘制图片 横坐标、纵坐标、宽度、高度
    }
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    int x = e->x() - (width()-LV_HOR_RES_MAX)/ 2;
    int y = e->y() - (height()-LV_VER_RES_MAX)/2;
    evm_touchpad(x, y, 1);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    int x = e->x() - (width()-LV_HOR_RES_MAX)/ 2;
    int y = e->y() - (height()-LV_VER_RES_MAX)/2;
    evm_touchpad(x, y, 0);
}
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    int x = e->x() - (width()-LV_HOR_RES_MAX)/ 2;
    int y = e->y() - (height()-LV_VER_RES_MAX)/2;
    evm_touchpad(x, y, 1);
}

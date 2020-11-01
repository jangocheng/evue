#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "transparentdialog.h"
#include "embedapp.h"

#if defined(Q_OS_WIN)
#include "findwindow_win32.h"
#endif

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(const QString& appdir, QWidget *parent = nullptr);
    void initUI();
    void startEVUE(const QString& appdir);
    void initConnect();
    ~MainWindow();

public slots:
    void closeEVUE();
    void onWindowReady();

protected:
    void paintEvent(QPaintEvent* p);
    void showEvent(QShowEvent* event);
    bool event(QEvent *event);
    void closeEvent(QCloseEvent* event);

private:
    QString m_appdir;
    TransparentWidget * m_bgwidget;

#if defined(Q_OS_WIN)
    HWND m_hwnd;
#endif
    QWindow * m_containerWindow;
    QWidget* m_containerWidget;
    QProcess* m_process;
    QTimer * m_timer;

    EmbedApp* m_embedApp;

};
#endif // MAINWINDOW_H

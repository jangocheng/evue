#ifndef TRANSPARENTWIDGET_H
#define TRANSPARENTWIDGET_H

#include <QtCore>
#include <QtWidgets>

class TransparentWidget : public QDialog
{
    Q_OBJECT
public:
    TransparentWidget(QWidget *parent = 0)
        : QDialog(parent)
    {
        setFixedSize(530, 626);

        setWindowFlag(Qt::FramelessWindowHint);
        setWindowFlag(Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
    }

    void paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        QPixmap pix;
        painter.translate(0, 0);
        pix.load(m_backgroundimage);//加载图片地址 可以是资源文件
        painter.drawPixmap(0, 0, width(), height(), pix);//绘制图片 横坐标、纵坐标、宽度、高度
    }

    void mousePressEvent(QMouseEvent *event)
    {
        if (event->button() == Qt::LeftButton) {
            m_startPoint = frameGeometry().topLeft() - event->globalPos();
        }
    }

    void mouseMoveEvent(QMouseEvent *event)
    {
        this->move(event->globalPos() + m_startPoint);
    }

    inline QString backgroundimage() const {return m_backgroundimage;};
    inline void setBackgroundimage(const QString &backgroundimage){
        m_backgroundimage = backgroundimage;
    };

private:
    QPoint m_startPoint;
    QString m_backgroundimage = ":/skin/gt2.png";
};

#endif // TRANSPARENTWIDGET_H


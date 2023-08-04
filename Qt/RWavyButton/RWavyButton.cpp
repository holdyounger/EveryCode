#include "RWavyButton.h"
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>
#include <QPainter>

RWavyButton::RWavyButton(QWidget *parent)
    : QPushButton(parent)
{
    setMouseTracking(true);  // 开启鼠标追踪，使得可以在不按下鼠标按键的情况下追踪鼠标移动

    m_Timer = new QTimer;
    m_Timer->setInterval(m_nInterval);
    connect(m_Timer, &QTimer::timeout, this, &RWavyButton::slotEnter);
}

RWavyButton::~RWavyButton()
{

}

void RWavyButton::leaveEvent(QEvent *event)
{
    qDebug()<< __FUNCTION__;
    // setStyleSheet("background-color:blue;");
    m_Timer->disconnect();
    connect(m_Timer, &QTimer::timeout, this, &RWavyButton::slotLeave);
    m_Timer->start();
}

//重写鼠标出去时的方法
void  RWavyButton::enterEvent(QEvent *event)
{
    max_radius = qSqrt(qPow(width(), 2) + qPow(width(), 2));

    qDebug()<< __FUNCTION__;
    qDebug() <<max_radius;

    setStyleSheet("background-color:white;");

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    m_nCenter = mouseEvent->pos();

   int x = m_nCenter.x() ;
    int y = m_nCenter.y();
    m_nCenter.setX(y);
    m_nCenter.setY(x);

    qDebug() << m_nCenter;

    m_Timer->disconnect();
    connect(m_Timer, &QTimer::timeout, this, &RWavyButton::slotEnter);
    m_Timer->start();
}

void RWavyButton::slotEnter()
{
    m_nRadius += m_nRadiusStep;

    if(m_nRadius > max_radius)
    {
        m_Timer->stop();
        return;
    }

    update();
}

void RWavyButton::slotLeave()
{
    m_nRadius -= m_nRadiusStep;

    if(m_nRadius < 0)
    {
        m_Timer->stop();
        return;
    }

    update();
}

void RWavyButton::paintEvent(QPaintEvent *)
{
    if(m_nCenter.isNull())
        return;

    QPainter *painter = new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing);

    QBrush brush = QBrush(Qt::yellow);
    painter->setBrush(brush);
    painter->setPen(Qt::NoPen);

    QPainterPath path;
    path.addRoundedRect(this->rect(), 1, 1);
    painter->setClipPath(path);

    painter->drawEllipse(m_nCenter, m_nRadius, m_nRadius);
    painter->end();
}

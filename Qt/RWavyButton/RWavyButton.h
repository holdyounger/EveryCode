#ifndef RWAVYBUTTON_H
#define RWAVYBUTTON_H

#include <QDialog>
#include <QPushButton>
#include <QTimer>

QT_BEGIN_NAMESPACE
// namespace Ui { class RWavyButton; }
QT_END_NAMESPACE

class RWavyButton : public QPushButton
{
    Q_OBJECT

public:
    RWavyButton(QWidget *parent = nullptr);
    ~RWavyButton();

protected:
    void leaveEvent(QEvent*event);//重写鼠标出去时的方法
    void  enterEvent(QEvent*event);

    void paintEvent(QPaintEvent *);
private:
   //  Ui::RWavyButton *ui;
    // QPushButton *m_btn;
    int max_radius;

    int m_nRadius = 0;
    int m_nRadiusStep = 2;
    QPoint m_nCenter;
    int m_nInterval = 10;
    int m_borderRadius = 10;

    QTimer *m_Timer;

private Q_SLOTS:
    void slotEnter();
    void slotLeave();

};
#endif // RWAVYBUTTON_H

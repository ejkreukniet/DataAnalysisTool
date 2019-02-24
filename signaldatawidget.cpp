#include <QPaintEvent>
#include <QPainter>

#include "signaldatawidget.h"

SignalDataWidget::SignalDataWidget(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Dark);
}

void SignalDataWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QRect rect = this->rect();

    QColor border(0, 0, 0, 255);
    QColor background(220, 220, 220, 255);
    QColor value(200, 180, 200, 255);

    QPainter painter(this);
    //painter.setPen(Qt::NoPen);
    //painter.setBrush(background);
    painter.fillRect(rect, background);
    painter.setPen(border);
    //painter.setBrush(border);
    rect.adjust(0, 0, -1, -1);
    painter.drawRect(rect);

    if (m_values->size() > 0)
    {
        int x = rect.left();
        int y = 0;
        int step = rect.width()/m_values->size();

        for (int i = 0; i < m_values->size(); ++i)
        {
            painter.drawLine(x, rect.bottom()-y*rect.height()/2048, x+step, rect.bottom()-m_values->at(i)*rect.height()/2048);
            x += step;
            y = m_values->at(i);
        }
    }
}

#ifndef SIGNALDATAWIDGET_H
#define SIGNALDATAWIDGET_H

#include <QWidget>

#include "dSFMT.h"

class SignalDataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalDataWidget(QWidget *parent = 0);

    void setValues(QList<uint32_t> *values)
    {
        m_values = values;
    }


protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    QList<uint32_t> *m_values;
};

#endif // SIGNALDATAWIDGET_H

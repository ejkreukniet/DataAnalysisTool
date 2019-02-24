#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QXmlStreamReader>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QTimer>
#include <QMediaPlayer>
#include <QVideoWidget>

#include "dSFMT.h"
#include "signaldatawidget.h"

#define MAX_CAMERAS 2

namespace Ui {
class MainWindow;
}

enum sources {
    FRONT,
    SIDE,
    NUM_SOURCES
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void animate();
    void SlotPlayClicked(bool checked = false);
    void SlotPauseClicked(bool checked = false);
    void SlotRecordClicked(bool checked = false);
    void SlotSampleClicked(bool checked = false);
    void handleError();

private:
    Ui::MainWindow *m_ui;

    QLabel *m_label[NUM_SOURCES];
    QMediaPlayer *m_player[NUM_SOURCES];
    QVideoWidget *m_widget[NUM_SOURCES];

    QLabel *m_graphicLabel;
    SignalDataWidget *m_signalData;

    QPushButton *m_buttonPlay;
    QPushButton *m_buttonPause;
    QPushButton *m_buttonRecord;
    QPushButton *m_buttonSample;

    QTimer *m_timer;

    uint32_t m_minValue;
    uint32_t m_maxValue;
    QList<uint32_t> m_values;
};

#endif // MAINWINDOW_H

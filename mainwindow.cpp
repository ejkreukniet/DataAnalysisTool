#include <QMessageBox>
#include <QCoreApplication>
#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QTimer>
#include <QWindow>
#include <QResizeEvent>
#include <QCommonStyle>

#include <QCameraInfo>

#include "MainWindow.h"
#include "ui_MainWindow.h"

#define MARGIN 20
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 30

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_minValue(-1)
    , m_maxValue(-1)
{
    // Seed random generator
    dsfmt_gv_init_gen_rand(25472);

    m_ui->setupUi(this);

    for (int i = 0; i < NUM_SOURCES; ++i)
    {
        m_label[i] = new QLabel(this);
        m_label[i]->setAlignment(Qt::AlignTop | Qt::AlignCenter);

        m_player[i] = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
        m_widget[i] = new QVideoWidget(this);
        m_player[i]->setVideoOutput(m_widget[i]);

        connect(m_player[i], SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));
        m_widget[i]->show();
    }
    m_label[FRONT]->setText("Front Camera");
    m_label[SIDE]->setText("Side Camera");

    m_player[FRONT]->setMedia(QUrl::fromLocalFile("D:/Work/DataAnalysisTool/debug/waterfall.avi"));
    m_player[SIDE]->setMedia(QUrl::fromLocalFile("D:/Work/DataAnalysisTool/debug/cam2.mp4"));

    qDebug() << m_player[FRONT]->isAvailable();


    m_graphicLabel = new QLabel(this);
//    m_graphicLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_graphicLabel->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    m_graphicLabel->setText("Sensor Data");

    m_signalData = new SignalDataWidget(this);
    m_signalData->setValues(&m_values);
//    m_signalData->setFrameStyle(QFrame::Box);
//    m_signalData->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QCommonStyle style;

    m_buttonPlay = new QPushButton(style.standardIcon(QStyle::SP_MediaPlay), "&Play", this);
    m_buttonPause = new QPushButton(style.standardIcon(QStyle::SP_MediaPause), "&Pause", this);
    m_buttonRecord = new QPushButton(style.standardIcon(QStyle::SP_DialogSaveButton), "&Record", this);
    m_buttonSample = new QPushButton(style.standardIcon(QStyle::SP_ComputerIcon), "&Sample", this);

    connect(m_buttonPlay, SIGNAL( clicked(bool) ), this, SLOT( SlotPlayClicked(bool) ));
    connect(m_buttonPause, SIGNAL( clicked(bool) ), this, SLOT( SlotPauseClicked(bool) ));
    connect(m_buttonRecord, SIGNAL( clicked(bool) ), this, SLOT( SlotRecordClicked(bool) ));
    connect(m_buttonSample, SIGNAL( clicked(bool) ), this, SLOT( SlotSampleClicked(bool) ));

    m_timer = new QTimer();
    m_timer->setTimerType(Qt::PreciseTimer);
    m_timer->setInterval(50);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(animate()));
}

MainWindow::~MainWindow()
{
    m_values.clear();

    delete m_timer;

    delete m_buttonSample;
    delete m_buttonRecord;
    delete m_buttonPause;
    delete m_buttonPlay;

    delete m_signalData;
    delete m_graphicLabel;

    for (int i = 0; i < NUM_SOURCES; ++i)
    {
        delete m_label[i];
        delete m_player[i];
        delete m_widget[i];
    }

    delete m_ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();

    int w = size.width();
    int h = size.height();

    int camWidth = 320;
    int camHeight = 240;

    // Reposition widgets
    for (int i = 0; i < NUM_SOURCES; ++i)
    {
        m_widget[i]->resize(camWidth, camHeight);
        m_widget[i]->move(i==0?w/2-camWidth-MARGIN/2:w/2+MARGIN/2, 2*MARGIN);
    }
    m_label[FRONT]->setGeometry(QRect(w/2-camWidth-MARGIN/2, MARGIN, camWidth, MARGIN));
    m_label[SIDE]->setGeometry(QRect(w/2+MARGIN/2, MARGIN, camWidth, MARGIN));

    m_graphicLabel->setGeometry(QRect(MARGIN, 3*MARGIN+camHeight, w-2*MARGIN, MARGIN));
    m_signalData->setGeometry(QRect(MARGIN, 4*MARGIN+camHeight, w-2*MARGIN, h-BUTTON_HEIGHT-camHeight-6*MARGIN));

    m_buttonPlay->resize(BUTTON_WIDTH, BUTTON_HEIGHT);
    m_buttonPause->resize(BUTTON_WIDTH, BUTTON_HEIGHT);
    m_buttonRecord->resize(BUTTON_WIDTH, BUTTON_HEIGHT);
    m_buttonSample->resize(BUTTON_WIDTH, BUTTON_HEIGHT);

    int bw = (w-(4*BUTTON_WIDTH+3*MARGIN/2))/2;

    m_buttonPlay->move(bw+0*(BUTTON_WIDTH+MARGIN/2), h-BUTTON_HEIGHT-MARGIN);
    m_buttonPause->move(bw+1*(BUTTON_WIDTH+MARGIN/2), h-BUTTON_HEIGHT-MARGIN);
    m_buttonRecord->move(bw+2*(BUTTON_WIDTH+MARGIN/2), h-BUTTON_HEIGHT-MARGIN);
    m_buttonSample->move(bw+3*(BUTTON_WIDTH+MARGIN/2), h-BUTTON_HEIGHT-MARGIN);
}

void MainWindow::animate()
{
    uint32_t value = dsfmt_gv_genrand_uint32();

    value = 32+(value % (2048-32));

    // Store minimum and maximum values
    if (m_minValue == -1 || value < m_minValue)
        m_minValue = value;
    if (m_maxValue == -1 || value > m_maxValue)
        m_maxValue = value;

    m_values.append(value);
//  qDebug() << m_values.size() << ":" << value << " " << m_minValue << " " << m_maxValue;

    m_signalData->update();
}

void MainWindow::SlotPlayClicked(bool checked)
{
    Q_UNUSED(checked);

    for (int i = 0; i < NUM_SOURCES; ++i)
    {
        m_player[i]->play();
    }
    m_timer->start();
}

void MainWindow::SlotPauseClicked(bool checked)
{
    Q_UNUSED(checked);

    for (int i = 0; i < NUM_SOURCES; ++i)
    {
        m_player[i]->pause();
    }
    m_timer->stop();
}

void MainWindow::SlotRecordClicked(bool checked)
{
    Q_UNUSED(checked);

    QCommonStyle style;

    if (m_buttonRecord->text() == "&Record")
    {
        for (int i = 0; i < NUM_SOURCES; ++i)
        {
  //          m_recorder[i]->record();
        }

        m_buttonRecord->setText("&Stop");
        m_buttonRecord->setIcon(style.standardIcon(QStyle::SP_MediaStop));
    }
    else
    {
        for (int i = 0; i < NUM_SOURCES; ++i)
        {
  //          m_recorder[i]->stop();
        }

        m_buttonRecord->setText("&Record");
        m_buttonRecord->setIcon(style.standardIcon(QStyle::SP_DialogSaveButton));
    }
}

void MainWindow::SlotSampleClicked(bool checked)
{
    Q_UNUSED(checked);
}

void MainWindow::handleError()
{
    qDebug() << m_player[FRONT]->errorString();
    qDebug() << m_player[SIDE]->errorString();
}

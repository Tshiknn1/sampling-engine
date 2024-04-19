#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sescript.h"

#include <QAudioFormat>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QPushButton>
#include <QMessageBox>

#include "basicosc.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    buftime_(100),
    running_(false)
{
    ui->setupUi(this);
    initializeWindow();
    initializeAudio();
}

MainWindow::~MainWindow()
{
    if (engine_ != nullptr) {
        delete engine_;
    }
    delete ui;
}

void MainWindow::initializeWindow() {
    // setup format
    format_.setSampleRate(44100);
    format_.setChannelConfig(QAudioFormat::ChannelConfigMono);  // will be stereo later
    format_.setSampleFormat(QAudioFormat::Float);
    const QAudioDevice &defaultDeviceInfo = QMediaDevices::defaultAudioOutput();
    ui->deviceBox->addItem(defaultDeviceInfo.description(), QVariant::fromValue(defaultDeviceInfo));
    for (auto &deviceInfo : QMediaDevices::audioOutputs()) {
        if (deviceInfo != defaultDeviceInfo && deviceInfo.isFormatSupported(format_)) {
            ui->deviceBox->addItem(deviceInfo.description(), QVariant::fromValue(deviceInfo));
        }
    }
    ui->deviceBox->setCurrentText(defaultDeviceInfo.description());

    engine_ = new SE::BasicOsc();
    endpoint_.reset(new QEndpoint(format_, engine_));

    ui->freqSlider->setMinimum(50);
    ui->freqSlider->setMaximum(1000);

    connect(ui->deviceBox, SIGNAL(activated(int)), this, SLOT(deviceChanged(int)));
    connect(ui->freqSlider, SIGNAL(valueChanged(int)), this, SLOT(freqChanged(int)));
    connect(ui->startBtn, &QPushButton::pressed, this, [=]{
        if (!endpoint_->isActive()) {
            endpoint_->noteOn();
        }
    });
    connect(ui->stopBtn, &QPushButton::pressed, this, [=] {
        if (endpoint_->isActive()) {
            endpoint_->noteOff();
        }
    });

    connect(this, &MainWindow::underrunDetected, this, &MainWindow::underrunMessage);
    connect(this, &MainWindow::stallDetected, this, &MainWindow::stallMessage);
    connect(&stallDetector_, &QTimer::timeout, this, [=] {
        if (running_) {
            if (endpoint_->lastBufferSize() == 0) {
                emit stallDetected();
            }
        }
    });
}

void MainWindow::initializeAudio() {
    running_ = false;
    const QAudioDevice deviceInfo = ui->deviceBox->currentData().value<QAudioDevice>();
    output_.reset(new QAudioSink(deviceInfo, format_));
    QObject::connect(output_.data(), &QAudioSink::stateChanged, this, [=](QAudio::State state) {
        if (running_ && (output_->error() == QAudio::UnderrunError)) {
            emit underrunDetected();
        }
    });

    endpoint_->start();
    qint64 bufferLength = format_.bytesForDuration( buftime_ * 1000 );
    output_->setBufferSize(bufferLength);
    output_->start(endpoint_.get());
    freqChanged(ui->freqSlider->value());

    auto bufferTime = format_.durationForBytes(output_->bufferSize()) / 1000;
    QTimer::singleShot(bufferTime * 2, this, [=] {
        running_ = true;
        stallDetector_.start(bufferTime * 4);
    });
}

void MainWindow::underrunMessage() {
    running_ = false;
    QMessageBox::warning(this, "Underrun Error", "Audio buffer underrun error detected.");
    running_ = true;
}

void MainWindow::stallMessage() {
    running_ = false;
    QMessageBox::warning(this, "Stall Error", "Audio buffer stall error detected.");
    running_ = true;
}

void MainWindow::freqChanged(int freq) {
    endpoint_->updateFreq(freq);
}

void MainWindow::deviceChanged(int device) {

}

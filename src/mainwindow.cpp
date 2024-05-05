#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAudioFormat>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>

#include <string>

#include "synthsection.h"
#include "samplersection.h"
#include "masterbus.h"
#include "modules/oscillator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    buftime_(100),
    running_(false),
    se_format_(44100, 1),
    engine_(se_format_),
    synthSection_(se_format_),
    samplerSection_(se_format_)
{
    ui->setupUi(this);
    initializeWindow();
    initializeAudio();
}

MainWindow::~MainWindow()
{
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

    endpoint_.reset(new QEndpoint(format_, &engine_));
    engine_.addAudioGen(&synthSection_);
    engine_.addAudioGen(&samplerSection_);

    connect(ui->deviceBox, SIGNAL(activated(int)), this, SLOT(deviceChanged(int)));

    // synth section waveform selector
    connect(ui->synthSawButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    synthSection_.getOsc().setWaveform(SE::Oscillator::Waveform::Saw);
                }
            }));
    connect(ui->synthSquareButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    synthSection_.getOsc().setWaveform(SE::Oscillator::Waveform::Square);
                }
            }));
    connect(ui->synthTriButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    synthSection_.getOsc().setWaveform(SE::Oscillator::Waveform::Tri);
                }
            }));
    connect(ui->synthNoiseButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    synthSection_.getOsc().setWaveform(SE::Oscillator::Waveform::Noise);
                }
            }));

    // synth section frequency selector
    // NOTE: below is not going to work because of frequency sequencer
    connect(ui->synthOscFreqEntry, SIGNAL(valueChanged(int)), this, SLOT([&] (int freq) {
                synthSection_.getOsc().frequency() = freq;
            }));

    // synth section rhythmic sequencer
    connectCheckWithSeqSlot(ui->synthSeqCheck0, 0);
    connectCheckWithSeqSlot(ui->synthSeqCheck1, 1);
    connectCheckWithSeqSlot(ui->synthSeqCheck2, 2);
    connectCheckWithSeqSlot(ui->synthSeqCheck3, 3);
    connectCheckWithSeqSlot(ui->synthSeqCheck4, 4);
    connectCheckWithSeqSlot(ui->synthSeqCheck5, 5);
    connectCheckWithSeqSlot(ui->synthSeqCheck6, 6);
    connectCheckWithSeqSlot(ui->synthSeqCheck7, 7);
    connectCheckWithSeqSlot(ui->synthSeqCheck8, 8);
    connectCheckWithSeqSlot(ui->synthSeqCheck9, 9);
    connectCheckWithSeqSlot(ui->synthSeqCheck10, 10);
    connectCheckWithSeqSlot(ui->synthSeqCheck11, 11);
    connectCheckWithSeqSlot(ui->synthSeqCheck12, 12);
    connectCheckWithSeqSlot(ui->synthSeqCheck13, 13);
    connectCheckWithSeqSlot(ui->synthSeqCheck14, 14);
    connectCheckWithSeqSlot(ui->synthSeqCheck15, 15);

    // synth section pitch sequencer
    connectSliderWithSeqSlot(ui->synthSeqSlider0, 0);
    connectSliderWithSeqSlot(ui->synthSeqSlider1, 1);
    connectSliderWithSeqSlot(ui->synthSeqSlider2, 2);
    connectSliderWithSeqSlot(ui->synthSeqSlider3, 3);
    connectSliderWithSeqSlot(ui->synthSeqSlider4, 4);
    connectSliderWithSeqSlot(ui->synthSeqSlider5, 5);
    connectSliderWithSeqSlot(ui->synthSeqSlider6, 6);
    connectSliderWithSeqSlot(ui->synthSeqSlider7, 7);
    connectSliderWithSeqSlot(ui->synthSeqSlider8, 8);
    connectSliderWithSeqSlot(ui->synthSeqSlider9, 9);
    connectSliderWithSeqSlot(ui->synthSeqSlider10, 10);
    connectSliderWithSeqSlot(ui->synthSeqSlider11, 11);
    connectSliderWithSeqSlot(ui->synthSeqSlider12, 12);
    connectSliderWithSeqSlot(ui->synthSeqSlider13, 13);
    connectSliderWithSeqSlot(ui->synthSeqSlider14, 14);
    connectSliderWithSeqSlot(ui->synthSeqSlider15, 15);

    // synth section envelope
    connect(ui->synthEnvAtkSlider, SIGNAL(valueChanged(int)), this, SLOT([&] (int val) {
                synthSection_.getEnv().attack() = val * EnvSliderScalingFactor;
            }));
    connect(ui->synthEnvHoldSlider, SIGNAL(valueChanged(int)), this, SLOT([&] (int val) {
                synthSection_.getEnv().hold() = val * EnvSliderScalingFactor;
            }));
    connect(ui->synthEnvRelSlider, SIGNAL(valueChanged(int)), this, SLOT([&] (int val) {
                synthSection_.getEnv().release() = val * EnvSliderScalingFactor;
            }));

    // synth section LFO
    connect(ui->synthLFOSawButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    synthSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Saw);
                }
            }));
    connect(ui->synthLFOSquareButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    synthSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Square);
                }
            }));
    connect(ui->synthLFOTriButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    synthSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Tri);
                }
            }));
    connect(ui->synthLFONoiseButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    synthSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Noise);
                }
            }));

    connect(ui->synthLFOFreqEntry, SIGNAL(valueChanged(double)), this, SLOT([&] (double val) {
                synthSection_.getLFO().frequency() = static_cast<float>(val);
            }));

    ui->synthLFODestBox->addItem("Oscillator Frequency", QVariant::fromValue(SE::SynthSection::ModDestination::OscFreq));
    ui->synthLFODestBox->addItem("Oscillator Amplitude", QVariant::fromValue(SE::SynthSection::ModDestination::OscAmpl));
    ui->synthLFODestBox->addItem("Trigger Rate", QVariant::fromValue(SE::SynthSection::ModDestination::TrigDelta));
    ui->synthLFODestBox->addItem("Envelope Attack", QVariant::fromValue(SE::SynthSection::ModDestination::EnvAttack));
    ui->synthLFODestBox->addItem("Envelope Hold", QVariant::fromValue(SE::SynthSection::ModDestination::EnvHold));
    ui->synthLFODestBox->addItem("Envelope Release", QVariant::fromValue(SE::SynthSection::ModDestination::EnvRelease));
    ui->synthLFODestBox->addItem("All Envelope Rates", QVariant::fromValue(SE::SynthSection::ModDestination::EnvAll));
    ui->synthLFODestBox->addItem("None", QVariant::fromValue(SE::SynthSection::ModDestination::None));
    ui->synthLFODestBox->setCurrentText("None");
    connect(ui->synthLFODestBox, SIGNAL(activated(int)), this, SLOT(updateSynthLFOMod(int)));
    connect(ui->synthLFOAmtEntry, SIGNAL(valueChanged(double)), this, SLOT(updateSynthLFOMod(int)));

    // sampler section
    connect(ui->loadSampleButton, SIGNAL(triggered(QAction*)), this, SLOT(loadSample));
    connect(ui->samplerPitchAdjEntry, SIGNAL(valueChanged(double)), this, SLOT([&] (double val) {
                samplerSection_.getPlayer().repitch(val / samplerSection_.getPlayer().pitch());
            }));

    // sampler rhythm sequencer
    connectCheckWithSeqSlot(ui->samplerSeqCheck0, 0);
    connectCheckWithSeqSlot(ui->samplerSeqCheck1, 1);
    connectCheckWithSeqSlot(ui->samplerSeqCheck2, 2);
    connectCheckWithSeqSlot(ui->samplerSeqCheck3, 3);
    connectCheckWithSeqSlot(ui->samplerSeqCheck4, 4);
    connectCheckWithSeqSlot(ui->samplerSeqCheck5, 5);
    connectCheckWithSeqSlot(ui->samplerSeqCheck6, 6);
    connectCheckWithSeqSlot(ui->samplerSeqCheck7, 7);
    connectCheckWithSeqSlot(ui->samplerSeqCheck8, 8);
    connectCheckWithSeqSlot(ui->samplerSeqCheck9, 9);
    connectCheckWithSeqSlot(ui->samplerSeqCheck10, 10);
    connectCheckWithSeqSlot(ui->samplerSeqCheck11, 11);
    connectCheckWithSeqSlot(ui->samplerSeqCheck12, 12);
    connectCheckWithSeqSlot(ui->samplerSeqCheck13, 13);
    connectCheckWithSeqSlot(ui->samplerSeqCheck14, 14);
    connectCheckWithSeqSlot(ui->samplerSeqCheck15, 15);

    // sampler pitch sequencer
    connectSliderWithSeqSlot(ui->samplerSeqSlider0, 0);
    connectSliderWithSeqSlot(ui->samplerSeqSlider1, 1);
    connectSliderWithSeqSlot(ui->samplerSeqSlider2, 2);
    connectSliderWithSeqSlot(ui->samplerSeqSlider3, 3);
    connectSliderWithSeqSlot(ui->samplerSeqSlider4, 4);
    connectSliderWithSeqSlot(ui->samplerSeqSlider5, 5);
    connectSliderWithSeqSlot(ui->samplerSeqSlider6, 6);
    connectSliderWithSeqSlot(ui->samplerSeqSlider7, 7);
    connectSliderWithSeqSlot(ui->samplerSeqSlider8, 8);
    connectSliderWithSeqSlot(ui->samplerSeqSlider9, 9);
    connectSliderWithSeqSlot(ui->samplerSeqSlider10, 10);
    connectSliderWithSeqSlot(ui->samplerSeqSlider11, 11);
    connectSliderWithSeqSlot(ui->samplerSeqSlider12, 12);
    connectSliderWithSeqSlot(ui->samplerSeqSlider13, 13);
    connectSliderWithSeqSlot(ui->samplerSeqSlider14, 14);
    connectSliderWithSeqSlot(ui->samplerSeqSlider15, 15);

    // sampler envelope
    connect(ui->samplerEnvAtkSlider, SIGNAL(valueChanged(int)), this, SLOT([&] (int val) {
                samplerSection_.getEnv().attack() = val * EnvSliderScalingFactor;
            }));
    connect(ui->samplerEnvHoldSlider, SIGNAL(valueChanged(int)), this, SLOT([&] (int val) {
                samplerSection_.getEnv().hold() = val * EnvSliderScalingFactor;
            }));
    connect(ui->samplerEnvRelSlider, SIGNAL(valueChanged(int)), this, SLOT([&] (int val) {
                samplerSection_.getEnv().release() = val * EnvSliderScalingFactor;
            }));

    // synth section LFO
    connect(ui->samplerLFOSawButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    samplerSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Saw);
                }
            }));
    connect(ui->samplerLFOSquareButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    samplerSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Square);
                }
            }));
    connect(ui->samplerLFOTriButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    samplerSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Tri);
                }
            }));
    connect(ui->samplerLFONoiseButton, SIGNAL(toggled(bool)), this, SLOT([&] (bool checked) {
                if (checked) {
                    samplerSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Noise);
                }
            }));

    connect(ui->samplerLFOFreqEntry, SIGNAL(valueChanged(double)), this, SLOT([&] (double val) {
                samplerSection_.getLFO().frequency() = static_cast<float>(val);
            }));

    ui->samplerLFODestBox->addItem("Sample Pitch", QVariant::fromValue(SE::SamplerSection::ModDestination::PlayerPitch));
    ui->samplerLFODestBox->addItem("Sample Amplitude", QVariant::fromValue(SE::SamplerSection::ModDestination::PlayerAmpl));
    ui->samplerLFODestBox->addItem("Trigger Rate", QVariant::fromValue(SE::SamplerSection::ModDestination::TrigDelta));
    ui->samplerLFODestBox->addItem("Envelope Attack", QVariant::fromValue(SE::SamplerSection::ModDestination::EnvAttack));
    ui->samplerLFODestBox->addItem("Envelope Hold", QVariant::fromValue(SE::SamplerSection::ModDestination::EnvHold));
    ui->samplerLFODestBox->addItem("Envelope Release", QVariant::fromValue(SE::SamplerSection::ModDestination::EnvRelease));
    ui->samplerLFODestBox->addItem("All Envelope Rates", QVariant::fromValue(SE::SamplerSection::ModDestination::EnvAll));
    ui->samplerLFODestBox->addItem("None", QVariant::fromValue(SE::SamplerSection::ModDestination::None));
    ui->samplerLFODestBox->setCurrentText("None");
    connect(ui->samplerLFODestBox, SIGNAL(activated(int)), this, SLOT(updateSamplerLFOMod(int)));
    connect(ui->samplerLFOAmtEntry, SIGNAL(valueChanged(double)), this, SLOT(updateSamplerLFOMod(int)));

    connect(ui->outputStartButton, &QPushButton::pressed, this, [=]{
        if (!endpoint_->isActive()) {
            endpoint_->start();
        }
    });
    connect(ui->outputStopButton, &QPushButton::pressed, this, [=] {
        if (endpoint_->isActive()) {
            endpoint_->stop();
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


void MainWindow::updateSynthLFOMod(int) {
    synthSection_.changeMod(static_cast<SE::SynthSection::ModDestination>(ui->synthLFODestBox->currentData().toInt()),
                            ui->synthLFOAmtEntry->value());
}


void MainWindow::updateSamplerLFOMod(int) {
    samplerSection_.changeMod(static_cast<SE::SamplerSection::ModDestination>(ui->samplerLFODestBox->currentData().toInt()),
                              ui->samplerLFOAmtEntry->value());
}


void MainWindow::deviceChanged(int device) {

}


void MainWindow::loadSample() {
    std::string fn = QFileDialog::getOpenFileName(this, tr("Load Sample"), tr("C:\Documents")).toStdString();
    samplerSection_.getPlayer().loadSample(fn);
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAudioFormat>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>

#include <string>
#include <iostream>

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
    std::cout << "mainwindow constructor" << std::endl;
    ui->setupUi(this);
    std::cout << "ui setup complete" << std::endl;
    initializeWindow();
    std::cout << "initializewindow complete" << std::endl;
    initializeAudio();
    std::cout << "initializeaudio complete" << std::endl;
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

    ui->synthSeqTempoEntry->setRange(20.0, 600.0);

    ui->synthSeqSlider0->setRange(20, 2000);
    ui->synthSeqSlider1->setRange(20, 2000);
    ui->synthSeqSlider2->setRange(20, 2000);
    ui->synthSeqSlider3->setRange(20, 2000);
    ui->synthSeqSlider4->setRange(20, 2000);
    ui->synthSeqSlider5->setRange(20, 2000);
    ui->synthSeqSlider6->setRange(20, 2000);
    ui->synthSeqSlider7->setRange(20, 2000);
    ui->synthSeqSlider8->setRange(20, 2000);
    ui->synthSeqSlider9->setRange(20, 2000);
    ui->synthSeqSlider10->setRange(20, 2000);
    ui->synthSeqSlider11->setRange(20, 2000);
    ui->synthSeqSlider12->setRange(20, 2000);
    ui->synthSeqSlider13->setRange(20, 2000);
    ui->synthSeqSlider14->setRange(20, 2000);
    ui->synthSeqSlider15->setRange(20, 2000);

    ui->synthSeqSlider0->setValue(440);
    ui->synthSeqSlider1->setValue(440);
    ui->synthSeqSlider2->setValue(440);
    ui->synthSeqSlider3->setValue(440);
    ui->synthSeqSlider4->setValue(440);
    ui->synthSeqSlider5->setValue(440);
    ui->synthSeqSlider6->setValue(440);
    ui->synthSeqSlider7->setValue(440);
    ui->synthSeqSlider8->setValue(440);
    ui->synthSeqSlider9->setValue(440);
    ui->synthSeqSlider10->setValue(440);
    ui->synthSeqSlider11->setValue(440);
    ui->synthSeqSlider12->setValue(440);
    ui->synthSeqSlider13->setValue(440);
    ui->synthSeqSlider14->setValue(440);
    ui->synthSeqSlider15->setValue(440);

    ui->synthEnvAtkSlider->setRange(0, format_.sampleRate() * 5);
    ui->synthEnvHoldSlider->setRange(0, format_.sampleRate() * 5);
    ui->synthEnvRelSlider->setRange(0, format_.sampleRate() * 5);

    endpoint_.reset(new QEndpoint(format_, &engine_));
    engine_.addAudioGen(&synthSection_);
    engine_.addAudioGen(&samplerSection_);

    connect(ui->deviceBox, SIGNAL(activated(int)), this, SLOT(deviceChanged(int)));

    connect(ui->synthSeqTempoEntry, &QDoubleSpinBox::valueChanged, this, [&] (double val) {
        synthSection_.getTrigger().delta() = format_.sampleRate() * 60 / val;
    });

    // synth section waveform selector
    connect(ui->synthSawButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    synthSection_.getOsc().setWaveform(SE::Oscillator::Waveform::Saw);
                }
            });
    connect(ui->synthSquareButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    synthSection_.getOsc().setWaveform(SE::Oscillator::Waveform::Square);
                }
            });
    connect(ui->synthTriButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    synthSection_.getOsc().setWaveform(SE::Oscillator::Waveform::Tri);
                }
            });
    connect(ui->synthNoiseButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    synthSection_.getOsc().setWaveform(SE::Oscillator::Waveform::Noise);
                }
            });

    // synth section frequency selector
    // NOTE: below is not going to work because of frequency sequencer
    connect(ui->synthOscFreqEntry, &QDoubleSpinBox::valueChanged, this, [&] (double freq) {
                synthSection_.getOsc().frequency() = static_cast<float>(freq);
            });

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
    connect(ui->synthEnvAtkSlider, &QSlider::valueChanged, this, [&] (int val) {
                synthSection_.getEnv().attack() = val * EnvSliderScalingFactor;
            });
    connect(ui->synthEnvHoldSlider, &QSlider::valueChanged, this, [&] (int val) {
                synthSection_.getEnv().hold() = val * EnvSliderScalingFactor;
            });
    connect(ui->synthEnvRelSlider, &QSlider::valueChanged, this, [&] (int val) {
                synthSection_.getEnv().release() = val * EnvSliderScalingFactor;
            });

    // synth section LFO
    connect(ui->synthLFOSawButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    synthSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Saw);
                }
            });
    connect(ui->synthLFOSquareButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    synthSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Square);
                }
            });
    connect(ui->synthLFOTriButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    synthSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Tri);
                }
            });
    connect(ui->synthLFONoiseButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    synthSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Noise);
                }
            });

    connect(ui->synthLFOFreqEntry, &QDoubleSpinBox::valueChanged, this, [&] (double val) {
                synthSection_.getLFO().frequency() = static_cast<float>(val);
            });

    ui->synthLFODestBox->addItem("Oscillator Frequency", QVariant::fromValue(SE::SynthSection::ModDestination::OscFreq));
    ui->synthLFODestBox->addItem("Oscillator Amplitude", QVariant::fromValue(SE::SynthSection::ModDestination::OscAmpl));
    ui->synthLFODestBox->addItem("Trigger Rate", QVariant::fromValue(SE::SynthSection::ModDestination::TrigDelta));
    ui->synthLFODestBox->addItem("Envelope Attack", QVariant::fromValue(SE::SynthSection::ModDestination::EnvAttack));
    ui->synthLFODestBox->addItem("Envelope Hold", QVariant::fromValue(SE::SynthSection::ModDestination::EnvHold));
    ui->synthLFODestBox->addItem("Envelope Release", QVariant::fromValue(SE::SynthSection::ModDestination::EnvRelease));
    ui->synthLFODestBox->addItem("All Envelope Rates", QVariant::fromValue(SE::SynthSection::ModDestination::EnvAll));
    ui->synthLFODestBox->addItem("None", QVariant::fromValue(SE::SynthSection::ModDestination::None));
    ui->synthLFODestBox->setCurrentText("None");
    connect(ui->synthLFODestBox, &QComboBox::activated, this, &MainWindow::updateSynthLFOMod);
    connect(ui->synthLFOAmtEntry, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateSynthLFOMod);

    // sampler section
    connect(ui->loadSampleButton, &QToolButton::triggered, this, &MainWindow::loadSample);
    connect(ui->samplerPitchAdjEntry, &QDoubleSpinBox::valueChanged, this, [&] (double val) {
                samplerSection_.getPlayer().repitch(val / samplerSection_.getPlayer().pitch());
            });

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
    connect(ui->samplerEnvAtkSlider, &QSlider::valueChanged, this, [&] (int val) {
                samplerSection_.getEnv().attack() = val * EnvSliderScalingFactor;
            });
    connect(ui->samplerEnvHoldSlider, &QSlider::valueChanged, this, [&] (int val) {
                samplerSection_.getEnv().hold() = val * EnvSliderScalingFactor;
            });
    connect(ui->samplerEnvRelSlider, &QSlider::valueChanged, this, [&] (int val) {
                samplerSection_.getEnv().release() = val * EnvSliderScalingFactor;
            });

    // synth section LFO
    connect(ui->samplerLFOSawButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    samplerSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Saw);
                }
            });
    connect(ui->samplerLFOSquareButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    samplerSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Square);
                }
            });
    connect(ui->samplerLFOTriButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    samplerSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Tri);
                }
            });
    connect(ui->samplerLFONoiseButton, &QRadioButton::toggled, this, [&] (bool checked) {
                if (checked) {
                    samplerSection_.getLFO().setWaveform(SE::Oscillator::Waveform::Noise);
                }
            });

    connect(ui->samplerLFOFreqEntry, &QDoubleSpinBox::valueChanged, this, [&] (double val) {
                samplerSection_.getLFO().frequency() = static_cast<float>(val);
            });

    ui->samplerLFODestBox->addItem("Sample Pitch", QVariant::fromValue(SE::SamplerSection::ModDestination::PlayerPitch));
    ui->samplerLFODestBox->addItem("Sample Amplitude", QVariant::fromValue(SE::SamplerSection::ModDestination::PlayerAmpl));
    ui->samplerLFODestBox->addItem("Trigger Rate", QVariant::fromValue(SE::SamplerSection::ModDestination::TrigDelta));
    ui->samplerLFODestBox->addItem("Envelope Attack", QVariant::fromValue(SE::SamplerSection::ModDestination::EnvAttack));
    ui->samplerLFODestBox->addItem("Envelope Hold", QVariant::fromValue(SE::SamplerSection::ModDestination::EnvHold));
    ui->samplerLFODestBox->addItem("Envelope Release", QVariant::fromValue(SE::SamplerSection::ModDestination::EnvRelease));
    ui->samplerLFODestBox->addItem("All Envelope Rates", QVariant::fromValue(SE::SamplerSection::ModDestination::EnvAll));
    ui->samplerLFODestBox->addItem("None", QVariant::fromValue(SE::SamplerSection::ModDestination::None));
    ui->samplerLFODestBox->setCurrentText("None");
    connect(ui->samplerLFODestBox, &QComboBox::activated, this, &MainWindow::updateSamplerLFOMod);
    connect(ui->samplerLFOAmtEntry, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateSamplerLFOMod);

    connect(ui->outputStartButton, &QPushButton::pressed, this, [=] () {
        if (!endpoint_->isActive()) {
            endpoint_->playStart();
        }
    });
    connect(ui->outputStopButton, &QPushButton::pressed, this, [=] () {
        if (endpoint_->isActive()) {
            endpoint_->playStop();
        }
    });

    connect(this, &MainWindow::underrunDetected, this, &MainWindow::underrunMessage);
    connect(this, &MainWindow::stallDetected, this, &MainWindow::stallMessage);
    connect(&stallDetector_, &QTimer::timeout, this, [=] () {
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

    synthSection_.initializeModules();
    // samplerSection_.initializeModules();

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


void MainWindow::updateSynthLFOMod(double) {
    synthSection_.changeMod(static_cast<SE::SynthSection::ModDestination>(ui->synthLFODestBox->currentData().toInt()),
                            ui->synthLFOAmtEntry->value());
}


void MainWindow::updateSamplerLFOMod(double) {
    samplerSection_.changeMod(static_cast<SE::SamplerSection::ModDestination>(ui->samplerLFODestBox->currentData().toInt()),
                              ui->samplerLFOAmtEntry->value());
}


void MainWindow::deviceChanged(int device) {

}


void MainWindow::loadSample() {
    std::string fn = QFileDialog::getOpenFileName(this, tr("Load Sample"), tr("C:\Documents")).toStdString();
    samplerSection_.getPlayer().loadSample(fn);
}

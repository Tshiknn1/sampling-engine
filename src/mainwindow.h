#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioFormat>
#include <QAudioSink>
#include <QTimer>
#include <QCheckBox>
#include <QSlider>

#include "qendpoint.h"
#include "masterbus.h"
#include "synthsection.h"
#include "samplersection.h"

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initializeWindow();
    void initializeAudio();
    void initializeAudioElements();

    void underrunMessage();
    void stallMessage();

public slots:
    void deviceChanged(int device);
    void updateSynthLFOMod(double);
    void updateSamplerLFOMod(double);
    void loadSample();

signals:
    void underrunDetected();
    void stallDetected();

private:
    inline void connectCheckWithSeqSlot(QCheckBox* cb, const size_t& index) {
        connect(cb, &QCheckBox::stateChanged, this, [&] (int state) {
                    samplerSection_.getTrigSeq().at(index) = (state == Qt::Checked) ? 1 : 0;
                });
    }

    inline void connectSliderWithSeqSlot(QSlider* slider, const size_t& index) {
        connect(slider, &QSlider::valueChanged, this, [&] (int val) {
                    samplerSection_.getPitchSeq().at(index) = val;
                });
    }

    Ui::MainWindow *ui;
    int buftime_;
    bool running_;

    QAudioFormat format_;
    SE::AudioFormat se_format_;
    QScopedPointer<QAudioSink> output_;
    QTimer stallDetector_;

    std::unique_ptr<QEndpoint> endpoint_;
    SE::MasterBus engine_;
    SE::SynthSection synthSection_;
    SE::SamplerSection samplerSection_;

    const size_t EnvSliderScalingFactor = 1;
};
#endif // MAINWINDOW_H

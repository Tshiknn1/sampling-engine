#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioFormat>
#include <QAudioSink>
#include <QTimer>
#include "qendpoint.h"
#include "sebase.h"

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

    void underrunMessage();
    void stallMessage();

public slots:
    void freqChanged(int freq);
    void deviceChanged(int device);

signals:
    void underrunDetected();
    void stallDetected();

private:
    Ui::MainWindow *ui;
    int buftime_;
    bool running_;

    QAudioFormat format_;
    QScopedPointer<QAudioSink> output_;
    QTimer stallDetector_;

    std::unique_ptr<QEndpoint> endpoint_;
    SE::AudioGen* engine_;
};
#endif // MAINWINDOW_H

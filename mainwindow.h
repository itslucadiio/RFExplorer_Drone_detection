#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include "rfexplorer.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QCPGraph;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void newRf1Threshold(int value);
    void newRf2Threshold(int value);
    void newRf1Frequency(double startFreq,double endFreq);
    void newRf2Frequency(double startFreq,double endFreq);

public slots:

    // Reset all plots
    void resetPlots();
    void newRFExplorer(RFExplorer* device);

protected slots:
    void on_newRf1Config(int start_freq, int sweep_steps, int step_size, int threshold);
    void on_newRf1SweepData(QVector<float> powerVector, QVector<double> freqsVector);
    void on_newRf1Detections(QVector<Detection> detections);

private slots:

    void on_vslider_rf1_valueChanged(int value);
    void on_vslider_rf2_valueChanged(int value);

    void on_vslider_rf1_sliderReleased();
    void on_vslider_rf2_sliderReleased();

    void on_btn_rf1_58_clicked();

    void on_btn_rf1_24_clicked();

    void on_btn_rf1_sidebar_clicked();

    void on_ds_rf1_final_freq_valueChanged(double arg1);

    void on_ds_rf1_ini_freq_valueChanged(double arg1);

    void on_ds_rf2_ini_freq_valueChanged(double arg1);

    void on_ds_rf2_final_freq_valueChanged(double arg1);

    void on_btn_rf2_sidebar_clicked();

    void on_btn_rf2_58_clicked();

    void on_btn_rf2_51_clicked();

    void on_btn_rf2_24_clicked();

    void on_btn_rf2_900_clicked();

    void on_btn_rf2_433_clicked();

    void on_btn_rf2_modify_clicked();

    void on_btn_rf1_51_clicked();

    void on_btn_rf1_900_clicked();

    void on_btn_rf1_433_clicked();

    void on_btn_rf1_modify_clicked();

private:
    Ui::MainWindow *m_ui;

    int m_rf1_sweep_steps=100;
    int m_rf1_sweep_step_size=100;
    double m_rf1_start_freq = 1;
    double m_rf1_end_freq = 100;
    int m_rf1_threshold = -25;

    int m_rf2_sweep_steps=100;
    int m_rf2_sweep_step_size=100;

    // Spectrum graph
    QCPGraph* m_spectrumGraph1;
    QCPGraph* m_spectrumGraph2;
    QCPGraph* m_spectrumGraph3;

    // threshold graph (line)
    QCPGraph* m_rf1ThresholdGraph;
    QCPGraph* m_rf2ThresholdGraph;
    QCPGraph* m_rf3ThresholdGraph;

    void updateRf1Threshold(int value);
    void updateRf2Threshold(int value);
};
#endif // MAINWINDOW_H

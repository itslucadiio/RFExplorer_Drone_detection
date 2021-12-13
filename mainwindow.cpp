#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    resetPlots();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::newRFExplorer(RFExplorer* device)
{
    //Signals from RFExplorer
    connect(device, SIGNAL(new_config(int,int,int, int)), this, SLOT(on_newRf1Config(int,int,int,int)), Qt::DirectConnection);
    connect(device, SIGNAL(powers_freqs(QVector<float>,QVector<double>)), this, SLOT(on_newRf1SweepData(QVector<float>,QVector<double>)), Qt::DirectConnection);
    connect(device, SIGNAL(active_detections(QVector<Detection>)),this, SLOT(on_newRf1Detections(QVector<Detection>)), Qt::DirectConnection);

    //Signals from UI
    connect(this, SIGNAL(newRf1Threshold(int)),device, SLOT(edit_threshold(int)));
    connect(this, SIGNAL(newRf1Frequency(double,double)), device, SLOT(send_config(double,double)));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
void MainWindow::resetPlots()
{
    //***********************************************************
    // Cleanup all plots
    m_ui->rfPlot1->clearGraphs();
    m_ui->rfPlot1->clearItems();
    m_ui->rfPlot1->plotLayout()->clear();
    m_ui->rfPlot1->setBackground(QBrush(0x4D4D4D));

    // Build axes rectangle
    QCPAxisRect* spectrumAxisRect = new QCPAxisRect(m_ui->rfPlot1);
    spectrumAxisRect->setupFullAxesBox(true);
    spectrumAxisRect->axis(QCPAxis::atBottom)->setRange(2000.0, 2500.0);
    spectrumAxisRect->axis(QCPAxis::atLeft)->setRange(-100.0, -20.0);
    spectrumAxisRect->axis(QCPAxis::atRight)->setRange(-100.0, -20.0);
    spectrumAxisRect->axis(QCPAxis::atLeft)->setTickLabels(true);
    //spectrumAxisRect->axis(QCPAxis::atLeft)->setLabel("Power, dBFS");
    //spectrumAxisRect->axis(QCPAxis::atBottom)->setLabel("Frequency, MHz");
    m_ui->rfPlot1->plotLayout()->addElement(spectrumAxisRect);

    QCPRange yRange = m_ui->rfPlot1->yAxis->range();
    yRange.expand(-20);
    yRange.expand(-110);
    m_ui->rfPlot1->yAxis->setRange(yRange);
    m_ui->rfPlot1->rescaleAxes(true);

    // Add current spectrum plot
    m_spectrumGraph1 = m_ui->rfPlot1->addGraph(spectrumAxisRect->axis(QCPAxis::atBottom), spectrumAxisRect->axis(QCPAxis::atLeft));
    m_spectrumGraph1->setPen(QPen(Qt::green, 1.0));
    m_spectrumGraph1->setAntialiased(true);

    // Add detection threshold plot
    m_rf1ThresholdGraph = m_ui->rfPlot1->addGraph(spectrumAxisRect->axis(QCPAxis::atBottom), spectrumAxisRect->axis(QCPAxis::atLeft));
    m_rf1ThresholdGraph->setPen(QPen(Qt::red, 2.0, Qt::SolidLine));
    //m_rf1ThresholdGraph->setAntialiased(true);


    // Update
    m_ui->rfPlot1->rescaleAxes();
    m_ui->rfPlot1->replot();

    m_ui->vslider_rf1->setValue(m_rf1_threshold);
    updateRf1Threshold(m_rf1_threshold);

    //***********************************************************
    // RF2 PLOT

    m_ui->rfPlot2->clearGraphs();
    m_ui->rfPlot2->clearItems();
    m_ui->rfPlot2->plotLayout()->clear();
    m_ui->rfPlot2->setBackground(QBrush(0x4D4D4D));

    //***********************************************************
    QCPAxisRect* spectrumAxisRect2 = new QCPAxisRect(m_ui->rfPlot2);
    spectrumAxisRect2->setupFullAxesBox(true);
    spectrumAxisRect2->axis(QCPAxis::atBottom)->setRange(2451.0, 2550.0);
    spectrumAxisRect2->axis(QCPAxis::atLeft)->setRange(-100, -20.0);
    spectrumAxisRect2->axis(QCPAxis::atRight)->setRange(2551.0, 2651.0);
    spectrumAxisRect2->axis(QCPAxis::atLeft)->setTickLabels(true);
   // spectrumAxisRect2->axis(QCPAxis::atLeft)->setLabel("Power, dBFS");
    //spectrumAxisRect2->axis(QCPAxis::atBottom)->setLabel("Frequency, MHz");
    m_ui->rfPlot2->plotLayout()->addElement(spectrumAxisRect2);

    QCPRange yRange2 = m_ui->rfPlot2->yAxis->range();
    yRange2.expand(-20);
    yRange2.expand(-110);
    m_ui->rfPlot2->yAxis->setRange(yRange2);
    m_ui->rfPlot2->rescaleAxes(true);

    // Add current spectrum plot
    m_spectrumGraph2 = m_ui->rfPlot2->addGraph(spectrumAxisRect2->axis(QCPAxis::atBottom), spectrumAxisRect2->axis(QCPAxis::atLeft));
    m_spectrumGraph2->setPen(QPen(Qt::red, 1.0));
    m_spectrumGraph2->setAntialiased(true);

    // Add detection threshold plot
    m_rf2ThresholdGraph = m_ui->rfPlot2->addGraph(spectrumAxisRect2->axis(QCPAxis::atBottom), spectrumAxisRect2->axis(QCPAxis::atLeft));
    m_rf2ThresholdGraph->setPen(QPen(Qt::red, 2.0, Qt::SolidLine));


    // Update
    m_ui->rfPlot2->rescaleAxes();
    m_ui->rfPlot2->replot();

    m_ui->vslider_rf2->setValue(-60);

    m_ui->rf1_sidebar->setVisible(false);
    m_ui->rf2_sidebar->setVisible(false);

}



void MainWindow::on_newRf1Config(int start_freq, int sweep_steps, int step_size, int threshold)
{
    int bw = sweep_steps * step_size;
    int end_freq = start_freq + (bw/1000);


    m_rf1_start_freq = start_freq/1000;
    m_rf1_end_freq = end_freq/1000;
    m_rf1_threshold = threshold;

    //Update UI visible data
    m_ui->lbl_rf1_freq_min->setText(QString::number(m_rf1_start_freq));
    m_ui->lbl_rf1_freq_max->setText(QString::number(m_rf1_end_freq));
    m_ui->vslider_rf1->setValue(m_rf1_threshold);

    //Update Graph params
    updateRf1Threshold(m_rf1_threshold);

    //m_ui->rfPlot1->rescaleAxes();
    //m_ui->rfPlot1->replot();

}

void MainWindow::on_newRf1SweepData(QVector<float> powerVector, QVector<double> freqsVector)
{
    qDebug()<<"NEW SWEEP DATA";

    int samplesCount = powerVector.size();
    QVector<QCPGraphData> detectedSignalGraphData(samplesCount);
    for (int n =0; n < samplesCount; n++)
    {
        detectedSignalGraphData[n].key =freqsVector[n];
        detectedSignalGraphData[n].value =powerVector[n];
    }
    m_spectrumGraph1->data()->set(detectedSignalGraphData);
    m_ui->rfPlot1->xAxis->setRange(freqsVector.first()-1,freqsVector.last()+1);
    //m_ui->rfPlot1->rescaleAxes();
    m_ui->rfPlot1->replot();


}

void MainWindow::on_newRf1Detections(QVector<Detection> detections)
{
    qDebug()<<"HEY THERE";
    m_ui->lbl_rf1_detections->setText(QString::number(detections.size()));

}

void MainWindow::updateRf2Threshold(int value)
{
    int samplesCount = 100;

    QVector<QCPGraphData> thresholdLevelGraphData2(samplesCount);
    for (int i = 0; i < samplesCount; i++)
    {
        thresholdLevelGraphData2[i].key = i ; //spectrumScale[i];
        thresholdLevelGraphData2[i].value = value;
    }

    m_rf2ThresholdGraph->data()->set(thresholdLevelGraphData2);

}


// RF1 SIDEBAR CONTROLS
//*************************************************************
void MainWindow::on_vslider_rf1_valueChanged(int value)
{
    m_ui->lbl_rf1_db->setText(QString::number(value));
    //updateRf1Threshold(value);

    // Update
    //m_ui->rfPlot1->xAxis->rescale();
    //m_ui->rfPlot1->replot();
}


void MainWindow::on_vslider_rf1_sliderReleased()
{
    int value = m_ui->vslider_rf1->value();
    updateRf1Threshold(value);
    emit newRf1Threshold(value);
}


void MainWindow::updateRf1Threshold(int value)
{

    m_rf1_threshold = value;
    //int samplesCount = m_rf1_sweep_steps;
    //QCPRange range = m_ui->rfPlot1->xAxis->range();
    //int step_size = (range.maxRange -range.minRange)/m_rf1_sweep_steps;

    /*QVector<QCPGraphData> thresholdLevelGraphData(samplesCount);
    for (int i = 0; i < samplesCount; i++)
    {
        thresholdLevelGraphData[i].key = range.maxRange+(step_size*i); //spectrumScale[i];
        thresholdLevelGraphData[i].value = m_rf1_threshold;
    }
    */
    QVector<QCPGraphData> thresholdLevelGraphData(2);
    thresholdLevelGraphData[0].key = m_rf1_start_freq;
    thresholdLevelGraphData[1].key = m_rf1_end_freq;
    thresholdLevelGraphData[0].value = m_rf1_threshold;
    thresholdLevelGraphData[1].value = m_rf1_threshold;
    m_rf1ThresholdGraph->data()->set(thresholdLevelGraphData);

}


void MainWindow::on_btn_rf1_24_clicked()
{
    double startFreq = 2400000;
    double endFreq = 2500000;
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_51_clicked()
{
    double startFreq = 5100000;
    double endFreq = 5200000;
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_58_clicked()
{
    double startFreq = 5800000;
    double endFreq = 5900000;
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_900_clicked()
{
    double startFreq = 800000;
    double endFreq = 900000;
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_433_clicked()
{
    double startFreq = 430000;
    double endFreq = 440000;
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_modify_clicked()
{
    double startFreq = m_ui->ds_rf1_ini_freq->value()*1000;
    double endFreq = m_ui->ds_rf1_final_freq->value()*1000;
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_sidebar_clicked()
{
     m_ui->rf1_sidebar->setVisible(m_ui->rf1_sidebar->isHidden());
     if(m_ui->rf1_sidebar->isHidden())
     {
         m_ui->btn_rf1_sidebar->setText("<<");
     }else {
         m_ui->btn_rf1_sidebar->setText(">>");
     }
}

void MainWindow::on_ds_rf1_final_freq_valueChanged(double arg1)
{
    if (m_ui->ds_rf1_ini_freq->value()>arg1)
    {
        m_ui->ds_rf1_ini_freq->setValue(arg1);
    }
}

void MainWindow::on_ds_rf1_ini_freq_valueChanged(double arg1)
{
    if (m_ui->ds_rf1_final_freq->value()<arg1)
    {
        m_ui->ds_rf1_final_freq->setValue(arg1);
    }
}


/***********************************************************
* RF2 Interface Controls
*/

void MainWindow::on_vslider_rf2_valueChanged(int value)
{
    m_ui->lbl_rf2_db->setText(QString::number(value));

    updateRf2Threshold(value);
    // Update
    m_ui->rfPlot2->xAxis->rescale();
    m_ui->rfPlot2->replot();
}

void MainWindow::on_vslider_rf2_sliderReleased()
{
    int value = m_ui->vslider_rf2->value();
    emit newRf2Threshold(value);
}

void MainWindow::on_ds_rf2_ini_freq_valueChanged(double arg1)
{
    if (m_ui->ds_rf2_final_freq->value()<arg1)
    {
        m_ui->ds_rf2_final_freq->setValue(arg1);
    }
}

void MainWindow::on_ds_rf2_final_freq_valueChanged(double arg1)
{
    if (m_ui->ds_rf2_ini_freq->value()>arg1)
    {
        m_ui->ds_rf2_ini_freq->setValue(arg1);
    }
}

void MainWindow::on_btn_rf2_sidebar_clicked()
{
    m_ui->rf2_sidebar->setVisible(m_ui->rf2_sidebar->isHidden());
    if(m_ui->rf2_sidebar->isHidden())
    {
        m_ui->btn_rf2_sidebar->setText("<<");
    }else {
        m_ui->btn_rf2_sidebar->setText(">>");
    }
}

void MainWindow::on_btn_rf2_24_clicked()
{
    double startFreq = 2400000;
    double endFreq = 2500000;
    emit newRf2Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf2_51_clicked()
{
    double startFreq = 5100000;
    double endFreq = 5200000;
    emit newRf2Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf2_58_clicked()
{
    double startFreq = 5800000;
    double endFreq = 5900000;
    emit newRf2Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf2_900_clicked()
{
    double startFreq = 800000;
    double endFreq = 900000;
    emit newRf2Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf2_433_clicked()
{
    double startFreq = 430000;
    double endFreq = 440000;
    emit newRf2Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf2_modify_clicked()
{
    double startFreq = m_ui->ds_rf2_ini_freq->value()*1000;
    double endFreq = m_ui->ds_rf2_final_freq->value()*1000;
    emit newRf2Frequency(startFreq,endFreq);
}




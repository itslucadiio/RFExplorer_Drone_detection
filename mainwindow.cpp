#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    //QMainWindow::showFullScreen();
    resetPlots();

    m_ui->lbl_rf1_alert->setVisible(false);
    m_ui->lbl_rf2_alert->setVisible(false);
    m_ui->asdt_logo_loading->setVisible(true);
    m_ui->gb_rf1->setVisible(false);
    m_ui->gb_rf2->setVisible(false);

    //Create plots update timer
    m_drawTimer = new QTimer(this);
    m_drawTimer->setInterval(1000.0 /10.0);
    m_drawTimer->setTimerType(Qt::PreciseTimer);
    connect(m_drawTimer,SIGNAL(timeout()),this,SLOT(handleDrawTimerTick()));
    m_drawTimer->start();

    QTimer::singleShot(20000, [this](){
        m_ui->gb_rf1->setVisible(true);
        m_ui->gb_rf2->setVisible(true);
        m_ui->asdt_logo_loading->setVisible(false);
    });
}

MainWindow::~MainWindow()
{
    //Cleanup allocated resources
    delete m_drawTimer;
    delete m_ui;
}

void MainWindow::newRF1Explorer(RFExplorer* device)
{
    m_rf1 = device;
    //Signals from RFExplorer
    connect(device, SIGNAL(new_config(int,int,int,int,QString)), this, SLOT(on_newRf1Config(int,int,int,int,QString)), Qt::DirectConnection);
    connect(device, SIGNAL(new_module_info()), this, SLOT(on_newRf1ModuleInfo()), Qt::DirectConnection);

    //Signals from UI
    connect(this, SIGNAL(newRf1Threshold(int)),device, SLOT(edit_threshold(int)));
    connect(this, SIGNAL(newRf1Frequency(double,double)), device, SLOT(send_config(double,double)));


    double startFreq = 5800000;
    double endFreq = 5900000;
    emit newRf1Frequency(startFreq,endFreq);

}

void MainWindow::on_newRf1ModuleInfo()
{
    m_rf1_module_ext = m_rf1->getExpModule();
    m_rf1_module = m_rf1->getModel();
    m_rf1_fw_ver = m_rf1->getFwVer();

    if(m_rf1_module!="006")
    {
        m_ui->btn_rf1_51->setVisible(false);
        m_ui->btn_rf1_58->setVisible(false);
        qDebug() << "24 AL RF1";

    }
    else
    {
        qDebug("RF1 is 006");
        qDebug() << "58 AL RF1";
        m_rf1_006 = true;
    }
}

void MainWindow::newRF2Explorer(RFExplorer* device)
{
    m_rf2 = device;
    //Signals from RFExplorer
    connect(device, SIGNAL(new_config(int,int,int,int,QString)), this, SLOT(on_newRf2Config(int,int,int,int,QString)), Qt::DirectConnection);
    connect(device, SIGNAL(new_module_info()), this, SLOT(on_newRf2ModuleInfo()), Qt::DirectConnection);

    //Signals from UI
    connect(this, SIGNAL(newRf2Threshold(int)),device, SLOT(edit_threshold(int)));
    connect(this, SIGNAL(newRf2Frequency(double,double)), device, SLOT(send_config(double,double)));

    if(m_rf1_006)
    {
        double startFreq = 2400000;
        double endFreq = 2500000;
        emit newRf2Frequency(startFreq,endFreq);
        qDebug() << "24 AL RF2";
    }

    else {
        double startFreq = 5800000;
        double endFreq = 5900000;
        emit newRf2Frequency(startFreq,endFreq);
        qDebug() << "58 AL RF2";
    }

}

void MainWindow::on_newRf2ModuleInfo()
{
    m_rf2_module_ext = m_rf2->getExpModule();
    m_rf2_module = m_rf2->getModel();
    m_rf2_fw_ver = m_rf2->getFwVer();

    if(m_rf2_module!="006")
    {
        m_ui->btn_rf2_51->setVisible(false);
        m_ui->btn_rf2_58->setVisible(false);
    }
    else
    {
        qDebug("RF2 is 006");
    }

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void MainWindow::resetPlots()
{
    //***********************************************************
    // Cleanup all plots
    m_ui->rfPlot1->clearGraphs();
    m_ui->rfPlot1->clearItems();
    m_ui->rfPlot1->plotLayout()->clear();
    m_ui->rfPlot1->setBackground(QBrush(QColor(220,220,220,10)));

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
    m_spectrumGraph1->setPen(QPen(Qt::darkGreen, 2.0));
    m_spectrumGraph1->setBrush(QBrush(0xFFFFFF));
    m_spectrumGraph1->setAntialiased(true);

    // Add detection threshold plot
    m_rf1ThresholdGraph = m_ui->rfPlot1->addGraph(spectrumAxisRect->axis(QCPAxis::atBottom), spectrumAxisRect->axis(QCPAxis::atLeft));
    m_rf1ThresholdGraph->setPen(QPen(Qt::red, 2.0, Qt::SolidLine));

    // Max values
    m_maxSpectrumGraph1 = m_ui->rfPlot1->addGraph(spectrumAxisRect->axis(QCPAxis::atBottom), spectrumAxisRect->axis(QCPAxis::atLeft));
    m_maxSpectrumGraph1-> setPen(QPen(Qt::gray,1.0));

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
    m_ui->rfPlot2->setBackground(QBrush(QColor(220,220,220,10)));

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
    m_spectrumGraph2->setPen(QPen(Qt::darkGreen, 2.0));
    m_spectrumGraph2->setBrush(QBrush(0xFFFFFF));
    m_spectrumGraph2->setAntialiased(true);

    // Add detection threshold plot
    m_rf2ThresholdGraph = m_ui->rfPlot2->addGraph(spectrumAxisRect2->axis(QCPAxis::atBottom), spectrumAxisRect2->axis(QCPAxis::atLeft));
    m_rf2ThresholdGraph->setPen(QPen(Qt::red, 2.0, Qt::SolidLine));

    // Max values
    m_maxSpectrumGraph2 = m_ui->rfPlot2->addGraph(spectrumAxisRect2->axis(QCPAxis::atBottom), spectrumAxisRect2->axis(QCPAxis::atLeft));
    m_maxSpectrumGraph2-> setPen(QPen(Qt::gray,1.0));

    // Update
    m_ui->rfPlot2->rescaleAxes();
    m_ui->rfPlot2->replot();

    m_ui->vslider_rf2->setValue(m_rf2_threshold);
    updateRf2Threshold(m_rf2_threshold);

    m_ui->rf1_sidebar->setVisible(false);
    m_ui->rf2_sidebar->setVisible(false);

}

void MainWindow::handleDrawTimerTick()
{
    //Manage RFExplorer 1
    if (m_rf1!=nullptr)
    {
        //Main graph

        QVector<float> powerVector =m_rf1->getPowerVector();
        QVector<double> freqsVector = m_rf1->getFreqsVector();


        //Verify vectors data coherence
        if((!powerVector.isEmpty() && !freqsVector.isEmpty()) && (powerVector.size() == freqsVector.size()))
        {
            int samplesCount = powerVector.size();

            QVector<QCPGraphData> detectedSignalGraphData(samplesCount);
            for (int n =0; n < samplesCount; n++)
            {
                detectedSignalGraphData[n].key =freqsVector[n];
                detectedSignalGraphData[n].value =powerVector[n];
            }

            // Max detection block
            QVector<QCPGraphData> detectedMaxGraph(samplesCount);
            if(samplesCount == m_maxPower1.length()){
                for(int i = 0; i < samplesCount; i++){
                    if(m_maxPower1[i] < powerVector[i]){
                        m_maxPower1[i] = powerVector[i];
                    }
                    detectedMaxGraph[i].key = freqsVector[i];
                    detectedMaxGraph[i].value = m_maxPower1[i];
                }
            } else {
                m_maxPower1 = powerVector;
                for(int i = 0; i < samplesCount; i++){
                    detectedMaxGraph[i].key = freqsVector[i];
                    detectedMaxGraph[i].value = m_maxPower1[i];
                }
            }

            m_spectrumGraph1->data()->set(detectedSignalGraphData);
            m_maxSpectrumGraph1->data()->set(detectedMaxGraph);
            m_ui->rfPlot1->xAxis->setRange(freqsVector.first()-1,freqsVector.last()+1);

        }

        //Detections block
        QVector<Detection> detections;
        detections = m_rf1->getDetections();

        m_ui->lbl_rf1_detections->setText(QString::number(detections.size()));

        int maxlevel =0;
        foreach (Detection det, detections)
        {
            int sublevel = det.counter;
            if(sublevel>maxlevel){ maxlevel=sublevel;}
        }
        if (maxlevel >10){maxlevel=10;}

        QString danger = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 );border-bottom-right-radius: 2px;border-bottom-left-radius: 2px;border: .px solid black;}";
        QString safe= "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #080,stop: 0.4999 #060,stop: 0.5 #050,stop: 1 #030 );border-bottom-right-radius: 2px;border-bottom-left-radius: 2px;border: 1px solid black;}";

        //Setting progress bar color
        if(maxlevel < 6)
        {
            m_ui->pb_rf1_meter->setStyleSheet(safe);
            //m_ui->lbl_rf1_alert->setVisible(false);
        }
        else
        {
            m_ui->pb_rf1_meter->setStyleSheet(danger);
            if (!m_lbl_rf1_alert_visible)
            {
                m_ui->lbl_rf1_alert->setVisible(true);
                m_lbl_rf1_alert_visible = true;

                QTimer::singleShot(10000, [this](){
                    m_ui->lbl_rf1_alert->setVisible(false);
                    m_lbl_rf1_alert_visible = false;
                });
            }
        }

        m_ui->pb_rf1_meter->setValue(maxlevel);


        //Update PLOT1 Graphs
        m_ui->rfPlot1->replot();

    }

    //Manage RFExplorer 2
    if (m_rf2!=nullptr)
    {
        //Main graph

        QVector<float> powerVector =m_rf2->getPowerVector();
        QVector<double> freqsVector = m_rf2->getFreqsVector();


        //Verify vectors data coherence
        if((!powerVector.isEmpty() && !freqsVector.isEmpty()) && (powerVector.size() == freqsVector.size()))
        {
            int samplesCount = powerVector.size();

            QVector<QCPGraphData> detectedSignalGraphData(samplesCount);
            for (int n =0; n < samplesCount; n++)
            {
                detectedSignalGraphData[n].key =freqsVector[n];
                detectedSignalGraphData[n].value =powerVector[n];
            }

            QVector<QCPGraphData> detectedMaxGraph(samplesCount);
            if(samplesCount == m_maxPower2.length()){
                for(int i = 0; i < samplesCount; i++){
                    if(m_maxPower2[i] < powerVector[i]){
                        m_maxPower2[i] = powerVector[i];
                    }
                    detectedMaxGraph[i].key = freqsVector[i];
                    detectedMaxGraph[i].value = m_maxPower2[i];
                }
            } else {
                m_maxPower2 = powerVector;
                for(int i = 0; i < samplesCount; i++){
                    detectedMaxGraph[i].key = freqsVector[i];
                    detectedMaxGraph[i].value = m_maxPower2[i];
                }
            }

            m_spectrumGraph2->data()->set(detectedSignalGraphData);
            m_maxSpectrumGraph2->data()->set(detectedMaxGraph);
            m_ui->rfPlot2->xAxis->setRange(freqsVector.first()-1,freqsVector.last()+1);

        }

        //Detections block
        QVector<Detection> detections;
        detections = m_rf2->getDetections();

        m_ui->lbl_rf2_detections->setText(QString::number(detections.size()));

        int maxlevel =0;
        foreach (Detection det, detections)
        {
            int sublevel = det.counter;
            if(sublevel>maxlevel){ maxlevel=sublevel;}
        }
        if (maxlevel >10){maxlevel=10;}

        QString danger = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 );border-bottom-right-radius: 2px;border-bottom-left-radius: 2px;border: .px solid black;}";
        QString safe= "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #080,stop: 0.4999 #060,stop: 0.5 #050,stop: 1 #030 );border-bottom-right-radius: 2px;border-bottom-left-radius: 2px;border: 1px solid black;}";

        //Setting progress bar color
        if(maxlevel < 6)
        {
            m_ui->pb_rf2_meter->setStyleSheet(safe);
            //m_ui->lbl_rf2_alert->setVisible(false);
        }
        else
        {
            m_ui->pb_rf2_meter->setStyleSheet(danger);
            if (!m_lbl_rf2_alert_visible)
            {
                m_ui->lbl_rf2_alert->setVisible(true);
                m_lbl_rf2_alert_visible = true;

                QTimer::singleShot(10000, [this](){
                    m_ui->lbl_rf2_alert->setVisible(false);
                    m_lbl_rf2_alert_visible = false;
                });
            }
        }

        m_ui->pb_rf2_meter->setValue(maxlevel);

        //Update PLOT1 Graphs
        m_ui->rfPlot2->replot();

    }


}

void MainWindow::on_newRf1Config(int start_freq, int sweep_steps, int step_size, int threshold, QString sn)
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
    m_ui->lbl_rf1_serialnumber->setText(sn);

    //Update Graph params
    updateRf1Threshold(m_rf1_threshold);

    //m_ui->rfPlot1->rescaleAxes();
    //m_ui->rfPlot1->replot();

}

void on_newRf1ModuleInfo()
{

}

void on_newRf2ModuleInfo()
{

}

void MainWindow::on_newRf2Config(int start_freq, int sweep_steps, int step_size, int threshold, QString sn)
{
    int bw = sweep_steps * step_size;
    int end_freq = start_freq + (bw/1000);


    m_rf2_start_freq = start_freq/1000;
    m_rf2_end_freq = end_freq/1000;
    m_rf2_threshold = threshold;

    //Update UI visible data
    m_ui->lbl_rf2_freq_min->setText(QString::number(m_rf2_start_freq));
    m_ui->lbl_rf2_freq_max->setText(QString::number(m_rf2_end_freq));
    m_ui->vslider_rf2->setValue(m_rf2_threshold);
    m_ui->lbl_rf2_serialnumber->setText(sn);

    //Update Graph params
    updateRf2Threshold(m_rf2_threshold);

    //m_ui->rfPlot1->rescaleAxes();
    //m_ui->rfPlot1->replot();

}

// RF1 SIDEBAR CONTROLS
//*************************************************************
void MainWindow::on_vslider_rf1_valueChanged(int value)
{
    m_ui->lbl_rf1_db->setText(QString::number(value));
}

void MainWindow::on_vslider_rf1_sliderReleased()
{
    int value = m_ui->vslider_rf1->value();
    updateRf1Threshold(value);
    emit newRf1Threshold(value);
}


void MainWindow::on_vslider_rf1_sliderPressed()
{
    int value = m_ui->vslider_rf1->value();
    updateRf1Threshold(value);
    emit newRf1Threshold(value);
}

void MainWindow::updateRf1Threshold(int value)
{
    m_rf1_threshold = value;
    QVector<QCPGraphData> thresholdLevelGraphData(2);
    thresholdLevelGraphData[0].key = m_rf1_start_freq;
    thresholdLevelGraphData[1].key = m_rf1_end_freq;
    thresholdLevelGraphData[0].value = m_rf1_threshold;
    thresholdLevelGraphData[1].value = m_rf1_threshold;
    m_rf1ThresholdGraph->data()->set(thresholdLevelGraphData);
    m_ui->vslider_rf1->setValue(m_rf1_threshold);
}

void MainWindow::on_btn_rf1_24_clicked()
{
    double startFreq = 2400000;
    double endFreq = 2500000;
    m_maxPower1.clear();
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_51_clicked()
{
    double startFreq = 5100000;
    double endFreq = 5200000;
    m_maxPower1.clear();
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_58_clicked()
{
    double startFreq = 5800000;
    double endFreq = 5900000;
    m_maxPower1.clear();
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_900_clicked()
{
    double startFreq = 850000;
    double endFreq = 950000;
    m_maxPower1.clear();
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_433_clicked()
{
    double startFreq = 400000;
    double endFreq = 500000;
    m_maxPower1.clear();
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_modify_clicked()
{
    double startFreq = m_ui->ds_rf1_ini_freq->value()*1000;
    double endFreq = m_ui->lbl_rf1_end_freq->text().toDouble()*1000;
    m_maxPower1.clear();
    emit newRf1Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf1_sidebar_clicked()
{
     m_ui->rf1_sidebar->setVisible(m_ui->rf1_sidebar->isHidden());
     if(m_ui->rf1_sidebar->isHidden())
     {
         m_ui->btn_rf1_sidebar->setText("<");
     }else {
         m_ui->btn_rf1_sidebar->setText(">");
     }
}

void MainWindow::on_ds_rf1_ini_freq_valueChanged(double arg1)
{
    m_ui->lbl_rf1_end_freq->setText(QString::number(arg1+100,'f',3));
    /*
     * if (m_ui->ds_rf1_final_freq->value()<arg1)
    {
        m_ui->ds_rf1_final_freq->setValue(arg1);

    }
    */
}


// RF2 SIDEBAR CONTROLS
//*************************************************************

void MainWindow::on_vslider_rf2_valueChanged(int value)
{
    m_ui->lbl_rf2_db->setText(QString::number(value));
}

void MainWindow::on_vslider_rf2_sliderReleased()
{
    int value = m_ui->vslider_rf2->value();
    updateRf2Threshold(value);
    emit newRf2Threshold(value);
}



void MainWindow::on_vslider_rf2_sliderPressed()
{
    int value = m_ui->vslider_rf2->value();
    updateRf2Threshold(value);
    emit newRf2Threshold(value);
}


void MainWindow::updateRf2Threshold(int value)
{
    m_rf2_threshold = value;
    QVector<QCPGraphData> thresholdLevelGraphData(2);
    thresholdLevelGraphData[0].key = m_rf2_start_freq;
    thresholdLevelGraphData[1].key = m_rf2_end_freq;
    thresholdLevelGraphData[0].value = m_rf2_threshold;
    thresholdLevelGraphData[1].value = m_rf2_threshold;
    m_rf2ThresholdGraph->data()->set(thresholdLevelGraphData);
    m_ui->vslider_rf2->setValue(m_rf2_threshold);

}

void MainWindow::on_ds_rf2_ini_freq_valueChanged(double arg1)
{

        m_ui->lbl_rf2_end_freq->setText(QString::number(arg1+100,'f',3));
}



void MainWindow::on_btn_rf2_sidebar_clicked()
{
    m_ui->rf2_sidebar->setVisible(m_ui->rf2_sidebar->isHidden());
    if(m_ui->rf2_sidebar->isHidden())
    {
        m_ui->btn_rf2_sidebar->setText("<");
    }else {
        m_ui->btn_rf2_sidebar->setText(">");
    }
}

void MainWindow::on_btn_rf2_24_clicked()
{
    double startFreq = 2400000;
    double endFreq = 2500000;
    m_maxPower2.clear();
    emit newRf2Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf2_51_clicked()
{
    double startFreq = 5100000;
    double endFreq = 5200000;
    m_maxPower2.clear();
    emit newRf2Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf2_58_clicked()
{
    double startFreq = 5800000;
    double endFreq = 5900000;
    m_maxPower2.clear();
    emit newRf2Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf2_900_clicked()
{
    double startFreq = 850000;
    double endFreq = 950000;
    m_maxPower2.clear();
    emit newRf2Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf2_433_clicked()
{
    double startFreq = 400000;
    double endFreq = 500000;
    m_maxPower2.clear();
    emit newRf2Frequency(startFreq,endFreq);
}

void MainWindow::on_btn_rf2_modify_clicked()
{
    double startFreq = m_ui->ds_rf2_ini_freq->value()*1000;
    double endFreq = m_ui->lbl_rf2_end_freq->text().toDouble()*1000;
    m_maxPower2.clear();
    emit newRf2Frequency(startFreq,endFreq);
}




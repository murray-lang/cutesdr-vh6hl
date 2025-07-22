#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSlider>
#include <QPushButton>
#include <QLineSeries>
#include <QValueAxis>
#include <QLogValueAxis>
#include <QThreadPool>
#include "io/iq/audio/IqAudioDevice.h"
#include <cmath>
#include "io/control/usb/UsbException.h"
#include "io/control/device/FunCubeDongle/FunCubeDongle.h"
#include "io/control/device/DeviceControlException.h"

#define FFT_SIZE 2048
#define SAMPLE_RATE 192000

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_mediaDevices(new QMediaDevices(this))
//    , m_iqProcessor(2048)
    , m_pIqReceiver(nullptr)
    , m_spectrumLineSeries()
    , m_spectrumAreaSeries()
    , m_timeseriesLineSeries()
    , m_chartMutex()
    , ui(new Ui::MainWindow)
    , m_panadapterXmin(0)
    , m_panadapterXmax(FFT_SIZE)
    , m_timeSeriesXmin(0)
    , m_timeSeriesXmax(FFT_SIZE)
{
    //m_pIqReceiver = new IqReceiver(2048);
    ui->setupUi(this);

    initializeWindow();
    initialiseRadio();
    //initializeAudio(QMediaDevices::defaultAudioInput());
    initializeAudio();

    //m_iqProcessor.start();
    //QThreadPool::globalInstance()->start(m_pIqReceiver);

//    QLineSeries *series = new QLineSeries();
//    series->append(0, 6);
//    series->append(2, 4);
//    series->append(3, 8);
//    series->append(7, 4);
//    series->append(10, 5);
//    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

    //ui->panadapterView->chart()
    //QChart* pChart = ui->panadapterView->chart();

    ui->panadapterView->setRenderHint(QPainter::Antialiasing);
    ui->timeseriesView->setRenderHint(QPainter::Antialiasing);

   //pChart->legend()->hide();

    //pChart->addSeries(&m_fftSeries);
    //pChart->addSeries(&m_realSeries);
    //pChart->addSeries(&m_imagSeries);
    //QPen pen = m_fftSeries.pen();
    //pen.setWidth(0);



    //pChart->createDefaultAxes();
    //pChart->axisY()->setRange(0, 25);
    //pChart->axisY()->setRange(0.005, 0.025);
    //pChart->axisX()->setRange(0, FFT_SIZE);
    //pChart->setTitle("FunCube FFT");
    configurePanadapter();
    configureTimeseriesChart();
//    connect(&m_iqProcessor, &IqReceiver::signalFftAvailable, this, &MainWindow::newFft, Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
  m_audioSink->stop();
  m_audioOutput->stop();
  if (m_pIqReceiver != nullptr) {
    disconnect(m_pIqReceiver, &IqReceiver::signalRealFftAvailable, this, &MainWindow::newRealFft);
    disconnect(m_pIqReceiver, &IqReceiver::signalComplexFftAvailable, this, &MainWindow::newComplexFft);
    disconnect(m_pIqReceiver, &IqReceiver::signalRealTimeseriesAvailable, this, &MainWindow::newRealTimeseries);
    disconnect(m_pIqReceiver, &IqReceiver::signalComplexTimeseriesAvailable, this, &MainWindow::newComplexTimeseries);
    disconnect(m_pIqReceiver, &IqReceiver::signalAudioDataAvailable, this, &MainWindow::newAudioData);
    delete m_pIqReceiver;
  }
  delete ui;
}

void
MainWindow::configurePanadapter()
{
  QChart* pChart = ui->panadapterView->chart();
//    m_spectrumAreaSeries.setUpperSeries(&m_spectrumLineSeries);

//    QGradient plotAreaGradient(QGradient::Preset::MorpheusDen);
//    QBrush plotAreaBrush(plotAreaGradient);
//    pChart->setBackgroundBrush(plotAreaBrush);

  //QPen pen(QRgb(0xccd0e1)); //0x0080ff
  QPen pen(QRgb(0x0080ff));
  pen.setWidth(0);
//  m_spectrumAreaSeries.setPen(pen);
  m_spectrumLineSeries.setPen(pen);

//    QGradient seriesGradient(QGradient::Preset::EternalConstance);
//    QBrush seriesBrush(seriesGradient);
//    m_spectrumAreaSeries.setBrush(seriesBrush);

//  pChart->addSeries(&m_spectrumAreaSeries);
  pChart->addSeries(&m_spectrumLineSeries);
  pChart->setTitle("Panadapter");
  pChart->createDefaultAxes();
  pChart->axes(Qt::Horizontal).first()->setRange(m_panadapterXmin, m_panadapterXmax);
  pChart->axes(Qt::Vertical).first()->setRange(-140, 0);
  pChart->legend()->hide();
    /*
    pChart->addSeries(pSeries);
    pChart->legend()->hide();
    pChart->setTitle("Panadapter");
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Frequency");
    axisX->setLabelFormat("%i");
    //axisX->setTickCount(pSeries->count());
    axisX->setRange(0, FFT_SIZE);
    pChart->addAxis(axisX, Qt::AlignBottom);
    pSeries->attachAxis(axisX);
    */
    /*
    //QLogValueAxis *axisY = new QLogValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Level");
    axisY->setLabelFormat("%g");
    //axisY->setBase(10.0);
    axisY->setRange(-140, 0);
    //axisY->setMinorTickCount(-1);
    pChart->addAxis(axisY, Qt::AlignLeft);
    pSeries->attachAxis(axisY);
    */
}

void
MainWindow::setPanadapterX(uint32_t xMin, uint32_t xMax)
{
    m_panadapterXmin = xMin;
    m_panadapterXmax = xMax;
    QChart* pChart = ui->panadapterView->chart();
    pChart->axes(Qt::Horizontal).first()->setRange(xMin, xMax);
}

void
MainWindow::configureTimeseriesChart()
{
    QChart* pChart = ui->timeseriesView->chart();

    QPen pen(QRgb(0x0080ff));
    pen.setWidth(0);
    m_timeseriesLineSeries.setPen(pen);

  pChart->addSeries(&m_timeseriesLineSeries);
  pChart->setTitle("Timeseries");
  pChart->createDefaultAxes();
  pChart->axes(Qt::Horizontal).first()->setRange(m_timeSeriesXmin / 100.0, m_timeSeriesXmax/100.0);

//  pChart->axes(Qt::Vertical).first()->setRange(2038, 2058);
  pChart->axes(Qt::Vertical).first()->setRange(-0.02, 0.02);

//  pChart->axes(Qt::Vertical).first()->setRange(-0.0000001, 0.0000003);

  pChart->legend()->hide();
}

void
MainWindow::setTimeSeriesX(uint32_t xMin, uint32_t xMax)
{
    m_timeSeriesXmin = xMin;
    m_timeSeriesXmax = xMax;
    QChart* pChart = ui->timeseriesView->chart();
    pChart->axes(Qt::Horizontal).first()->setRange(m_timeSeriesXmin, m_timeSeriesXmax);
}

void
MainWindow::newRealFft(const SharedRealSeriesData& fftData)
{

    //m_chartMutex.lock();
    //m_fftSeries.clear();
    if (fftData.data()->size() != m_panadapterXmax) {
      setPanadapterX(0, fftData.data()->size());
    }
    calcSpectrumSeries(fftData.data(), m_spectrumLineSeries);
//    QPen pen = m_fftSeries.pen();
//    pen.setWidth(0);
    //QChart* pChart = ui->panadapterView->chart();
    //pChart->createDefaultAxes();

    //m_chartMutex.unlock();
    //m_iqProcessor.recycleFftOutput(fftOut);

}

void
MainWindow::newComplexFft(const SharedComplexSeriesData& fftData)
{

  //m_chartMutex.lock();
  //m_fftSeries.clear();
  if (fftData.data()->size() != m_panadapterXmax) {
    setPanadapterX(0, fftData.data()->size());
  }
  calcSpectrumSeries(fftData.data(), m_spectrumLineSeries);
//    QPen pen = m_fftSeries.pen();
//    pen.setWidth(0);
  //QChart* pChart = ui->panadapterView->chart();
  //pChart->createDefaultAxes();

  //m_chartMutex.unlock();
  //m_iqProcessor.recycleFftOutput(fftOut);

}

void
MainWindow::newRealTimeseries(const SharedRealSeriesData& timeseries)
{
    //if (timeseries.data()->size() != m_timeSeriesXmax) {
        setTimeSeriesX(0, timeseries.data()->size());
//  setTimeSeriesX(0, 48);

    //}
  QList<QPointF> timeseriesPoints;
  uint32_t plotX = 0;
  size_t seriesSize = timeseries->size();
  for (size_t i = 0; i < seriesSize; i++) {
      timeseriesPoints.append(QPointF(plotX++, timeseries->at(i)));
  }

  m_timeseriesLineSeries.replace(timeseriesPoints);
}

void
MainWindow::newComplexTimeseries(const SharedComplexSeriesData& timeseries)
{
  uint32_t seriesSize = timeseries->size();
  //if (timeseries.data()->size() != m_timeSeriesXmax) {
    setTimeSeriesX(0, seriesSize);
//  setTimeSeriesX(2038, 2058);
//  setTimeSeriesX(2048 - 1952, 2048 - 1904);
//  qDebug() << timeseries.data()->size() << ", ";

  //}
  QList<QPointF> timeseriesPoints;
  uint32_t plotX = 0;
  for (size_t i = 0; i < seriesSize; i++) {
    const sdrcomplex& cpx = timeseries->at(i);
    timeseriesPoints.append(QPointF(plotX++, std::hypot(cpx.real(), cpx.imag())));
//    timeseriesPoints.append(QPointF(plotX++, cpx.real()));

  }

  m_timeseriesLineSeries.replace(timeseriesPoints);
}

void
MainWindow::newAudioData(const SharedRealSeriesData& audioData) const
{
  m_audioOutput->addAudioData(*audioData.data());
}

void
MainWindow::calcSpectrumSeries(const std::vector<sdrreal> * spectrumData, QLineSeries& spectrumSeries )
{
  QList<QPointF> spectrumPoints;
  uint32_t plotX = 0;
  size_t fftSize = spectrumData->size();
//  for (size_t bin = fftSize/2; bin < fftSize; bin++) {
//      //double absolute = std::log10(std::abs(fftOut->at(bin)));
//      spectrumPoints.append(QPointF(plotX++, spectrumData->at(bin)));
//  }
//  for (size_t bin = 0; bin < fftSize/2 -1; bin++) {
//      //double absolute = std::log10(std::abs(fftOut->at(bin)));
//      spectrumPoints.append(QPointF(plotX++, spectrumData->at(bin)));
//  }
  for (size_t bin = 0; bin < fftSize; bin++) {
    //double absolute = std::log10(std::abs(fftOut->at(bin)));
    spectrumPoints.append(QPointF(plotX++, spectrumData->at(bin)));
  }

    spectrumSeries.replace(spectrumPoints);
}

void
MainWindow::calcSpectrumSeries(const std::vector<sdrcomplex> * spectrumData, QLineSeries& spectrumSeries )
{
  QList<QPointF> spectrumPoints;
  uint32_t plotX = 0;
  size_t fftSize = spectrumData->size();
//  for (size_t bin = fftSize/2; bin < fftSize; bin++) {
//    //double absolute = std::log10(std::abs(fftOut->at(bin)));
//    spectrumPoints.append(QPointF(plotX++, std::abs(spectrumData->at(bin))));
//  }
//  for (size_t bin = 0; bin < fftSize/2 -1; bin++) {
//    //double absolute = std::log10(std::abs(fftOut->at(bin)));
//    spectrumPoints.append(QPointF(plotX++, std::abs(spectrumData->at(bin))));
//  }
  for (size_t bin = 0; bin < fftSize; bin++) {
    //double absolute = std::log10(std::abs(fftOut->at(bin)));
    const sdrcomplex& cpx = spectrumData->at(bin);
//    spectrumPoints.append(QPointF(plotX++, std::hypot(cpx.real(), cpx.imag())));
    spectrumPoints.append(QPointF(plotX++, cpx.real()));

  }

  spectrumSeries.replace(spectrumPoints);
}

void MainWindow::initializeWindow()
{
    const QAudioDevice &defaultOutputDeviceInfo = QMediaDevices::defaultAudioOutput();
//
//    ui->deviceList->addItem(defaultDeviceInfo.description(), QVariant::fromValue(defaultDeviceInfo));
//    for (auto &deviceInfo : m_devices->audioInputs()) {
//        if (deviceInfo != defaultDeviceInfo)
//            ui->deviceList->addItem(deviceInfo.description(), QVariant::fromValue(deviceInfo));
//    }
//
//    connect(ui->deviceList, &QComboBox::activated, this, &MainWindow::deviceChanged);

    //m_volumeSlider = new QSlider(Qt::Horizontal, this);
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(100);
    //connect(ui->volumeSlider, &QSlider::valueChanged, this, &MainWindow::sliderChanged);
//    layout->addWidget(m_volumeSlider);

//    m_modeButton = new QPushButton(this);
//    connect(ui->modeButton, &QPushButton::clicked, this, &MainWindow::toggleMode);
//    layout->addWidget(m_modeButton);

//    m_suspendResumeButton = new QPushButton(this);
//    connect(ui->suspendButton, &QPushButton::clicked, this, &MainWindow::toggleSuspend);
//    layout->addWidget(m_suspendResumeButton);
}

void MainWindow::initializeAudio()
{
  if (m_pIqReceiver != nullptr) {
    disconnect(m_pIqReceiver, &IqReceiver::signalRealFftAvailable, this, &MainWindow::newRealFft);
    disconnect(m_pIqReceiver, &IqReceiver::signalComplexFftAvailable, this, &MainWindow::newComplexFft);
    disconnect(m_pIqReceiver, &IqReceiver::signalRealTimeseriesAvailable, this, &MainWindow::newRealTimeseries);
    disconnect(m_pIqReceiver, &IqReceiver::signalComplexTimeseriesAvailable, this, &MainWindow::newComplexTimeseries);
    disconnect(m_pIqReceiver, &IqReceiver::signalAudioDataAvailable, this, &MainWindow::newAudioData);

    delete m_pIqReceiver;
    m_pIqReceiver = nullptr;
  }
  try {
    const QAudioDevice outputDeviceInfo = QMediaDevices::defaultAudioOutput();
    QAudioFormat outputFormat = outputDeviceInfo.preferredFormat();

    m_audioOutput.reset(new AudioOutput(outputFormat));
    m_audioSink.reset(new QAudioSink(outputDeviceInfo, outputFormat));
    m_audioOutput->start();
    m_audioSink->start(m_audioOutput.data());
    m_audioSink->setVolume(1.0);

    QAudioDevice deviceInfo = IqAudioDevice::findDevice("FUNcube");
    // QAudioDevice deviceInfo = IqAudioDevice::findDevice("Built-in");
    QAudioFormat format;
    //format.setSampleRate(8000);
    //format.setChannelCount(1);
    // format.setSampleRate(deviceInfo.preferredFormat().sampleRate());
    // format.setChannelCount(deviceInfo.preferredFormat().channelCount());
    format.setSampleRate(192000);
    format.setChannelCount(2);
    format.setSampleFormat(deviceInfo.preferredFormat().sampleFormat());
    // format.setSampleFormat(QAudioFormat::Int16);
    format.setChannelConfig(QAudioFormat::ChannelConfigStereo);
    //format.setSampleFormat(QAudioFormat::Int16);

    m_pIqReceiver = new IqReceiver(SAMPLE_RATE, FFT_SIZE);
    connect(m_pIqReceiver, &IqReceiver::signalRealFftAvailable, this, &MainWindow::newRealFft);
    connect(m_pIqReceiver, &IqReceiver::signalComplexFftAvailable, this, &MainWindow::newComplexFft);
    connect(m_pIqReceiver, &IqReceiver::signalRealTimeseriesAvailable, this, &MainWindow::newRealTimeseries);
    connect(m_pIqReceiver, &IqReceiver::signalComplexTimeseriesAvailable, this, &MainWindow::newComplexTimeseries);
    connect(m_pIqReceiver, &IqReceiver::signalAudioDataAvailable, this, &MainWindow::newAudioData);

    //IqAudioDevice* newInfo = new IqAudioDevice(format, &m_iqProcessor);
    auto *newInfo = new IqAudioDevice(format, m_pIqReceiver);
    m_audioDevice.reset(newInfo);

    QAudioSource* newSource = new QAudioSource(deviceInfo, format);
    // qDebug() << "Source format:" << newSource->format();
    m_audioInput.reset(newSource);

//    QAudioSource* newSource = new QAudioSource(deviceInfo, format);
//    m_audioInput.reset(newSource);
//    qreal initialVolume = QAudio::convertVolume(m_audioInput->volume(), QAudio::LinearVolumeScale,
//                                                QAudio::LogarithmicVolumeScale);
//    ui->volumeSlider->setValue(qRound(initialVolume * 100));

    //m_pDemodulator->start();
    m_audioDevice->start();
    m_audioInput->stop();
    m_audioInput->start(m_audioDevice.data());
    //toggleMode();
  }
  catch (const IqIoException &ex) {

  }
}

//void MainWindow::toggleMode()
//{
//    m_audioInput->stop();
//    toggleSuspend();

    // Change between pull and push modes
//    if (m_pullMode) {
//        ui->modeButton->setText(tr("Enable push mode"));
//        m_audioInput->start(m_audioDevice.data());
//    } else {
//        ui->modeButton->setText(tr("Enable pull mode"));
//        auto *io = m_audioInput->start();
//        if (!io)
//            return;
//
//        connect(io, &QIODevice::readyRead, [this, io]() {
//            static const qint64 BufferSize = 4096;
//            const qint64 bytesAvailable = m_audioInput->bytesAvailable();
//            const qint64 len = qMin(bytesAvailable, BufferSize);
//
//            QByteArray buffer(len, 0);
//            qint64 l = io->read(buffer.data(), len);
////            if (l > 0) {
////                const qreal level = m_audioDevice->calculateLevel(buffer.constData(), l);
////                m_canvas->setLevel(level);
////            }
//        });
//    }

//    m_pullMode = !m_pullMode;
//}

//void MainWindow::toggleSuspend()
//{
    // toggle suspend/resume
//    switch (m_audioInput->state()) {
//    case QAudio::SuspendedState:
//    case QAudio::StoppedState:
//        m_audioInput->resume();
//        ui->suspendButton->setText(tr("Suspend recording"));
//        break;
//    case QAudio::ActiveState:
//        m_audioInput->suspend();
//        ui->suspendButton->setText(tr("Resume recording"));
//        break;
//    case QAudio::IdleState:
//        // no-op
//        break;
//    }
//}

//void MainWindow::deviceChanged(int index)
//{
//    m_audioDevice->stop();
//    m_audioInput->stop();
//    m_audioInput->disconnect(this);
//
//    initializeAudio(ui->deviceList->itemData(index).value<QAudioDevice>());
//}

//void
//MainWindow::sliderChanged(int value)
//{
//    qreal linearVolume = QAudio::convertVolume(value / qreal(100), QAudio::LogarithmicVolumeScale,
//                                               QAudio::LinearVolumeScale);
//
//    m_audioInput->setVolume(linearVolume);
//}

void
MainWindow::initialiseRadio()
{
  const QList<QAudioDevice> inputs = QMediaDevices::audioInputs();
  for (const QAudioDevice &device : inputs) {
    qDebug() << "Device:" << device.description() << device.id();

    qDebug() << "  Minimum sample rate:" << device.minimumSampleRate();
    qDebug() << "  Maximum sample rate:" << device.maximumSampleRate();

    qDebug() << "  Minimum channel count:" << device.minimumChannelCount();
    qDebug() << "  Maximum channel count:" << device.maximumChannelCount();
    qDebug() << "  Channel Config:" << device.channelConfiguration();

    qDebug() << "  Sample formats supported:";
    for (auto format : device.supportedSampleFormats()) {
      qDebug() << "    " << format;
    }
  }

  FunCubeDongle fcd;
  RadioSettings radioSettings;
  radioSettings.rxSettings.push_back({
   .rfSettings = { .frequency = 10000000, .gain = 0.0 },
   .ifSettings = { .bandwidth = 200000, .gain = 0.0 }
  });
  try {
    fcd.initialise();
    if (fcd.discover()) {
        qDebug() << "Discovered!";
    } else {
        qDebug() << "NOT discovered!";
    }
    fcd.open();

    fcd.applySettings(radioSettings);

    fcd.close();
  } catch(UsbException& usbEx) {
    qDebug() << "USB Error: " << usbEx.what();
  } catch(DeviceControlException& deviceEx) {
    qDebug() << "DeviceControl Error: " << deviceEx.what();
  }
}

//#include "moc_mainwindow.cpp"


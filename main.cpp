#include "mainwindow.h"

#include <QApplication>
#include "dsp/utils/FftData.h"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main(int argc, char *argv[])
{
  // std::this_thread::sleep_for(std::chrono::seconds(5));
  RadioConfig radioConfig;
  std::ifstream f("/home/murray/dev/sdr/cutesdr-vk6hl/config.json");
  json config = json::parse(f);
  if (config.contains("radio")) {
    radioConfig = RadioConfig::fromJson(config["radio"]);
  } else {
    qDebug() << "No radio config found";
  }

  //qRegisterMetaType<QSharedPointer<vcomplex>>("SharedFftData");
  qRegisterMetaType<QSharedPointer< std::vector<float> >>("SharedFftData");

  QApplication a(argc, argv);
  MainWindow w(radioConfig);
  w.show();
  return a.exec();
}

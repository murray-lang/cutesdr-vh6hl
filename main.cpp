#include "mainwindow.h"

#include <QApplication>
#include "dsp/utils/FftData.h"

int main(int argc, char *argv[])
{
  // std::this_thread::sleep_for(std::chrono::seconds(5));

  //qRegisterMetaType<QSharedPointer<vcomplex>>("SharedFftData");
  qRegisterMetaType<QSharedPointer< std::vector<float> >>("SharedFftData");

  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}

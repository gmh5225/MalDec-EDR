#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int
main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  mainwindow w;
  w.show();
  return a.exec();
}

#include "main_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  a.setOrganizationName(QStringLiteral("Master Gekus"));
  a.setApplicationName(QStringLiteral("MG Simple Certificate Autrhrity"));

  MainWindow w;
  w.show();
  return a.exec();
}

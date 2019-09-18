#include "main_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  a.setOrganizationName(QStringLiteral("Master Gekus"));
  a.setApplicationName(QStringLiteral("MG Simple Certificate Authority"));

  MainWindow w;
  w.show();

  if (1 < argc) {
    QMetaObject::invokeMethod(&w, "open", Qt::QueuedConnection, Q_ARG(QString, QObject::trUtf8(argv[1])));
  }

  return a.exec();
}

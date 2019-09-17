#include "main_window.h"
#include "./ui_main_window.h"

#include <QApplication>
#include <QString>
#include <QSettings>

#define WINDOW_TITLE QStringLiteral("%1 :: %2")
#define MAIN_WINDOW_GROUP QStringLiteral("Main Window")
#define GEOMETRY_KEY QStringLiteral("Geometry")
#define STATE_KEY QStringLiteral("State")

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  QSettings settings;
  settings.beginGroup(MAIN_WINDOW_GROUP);
  restoreGeometry(settings.value(GEOMETRY_KEY).toByteArray());
  restoreState(settings.value(STATE_KEY).toByteArray());

  ui->actionOpen->setShortcut(QKeySequence::Open);
  ui->actionSave->setShortcut(QKeySequence::Save);
  ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
  ui->actionQuit->setShortcut(QKeySequence::Quit);

  update_ui();
}

MainWindow::~MainWindow()
{
  QSettings settings;
  settings.beginGroup(MAIN_WINDOW_GROUP);
  settings.setValue(GEOMETRY_KEY, saveGeometry());
  settings.setValue(STATE_KEY, saveState());

  delete ui;
}

void MainWindow::update_ui()
{
  setWindowTitle(WINDOW_TITLE.arg(current_file_.isFile() ? current_file_.fileName() : QStringLiteral("<untitled>"),
                                  QApplication::applicationName()));
}

void MainWindow::on_actionOpen_triggered()
{
}

void MainWindow::on_actionSave_triggered()
{

}

void MainWindow::on_actionSaveAs_triggered()
{
}

void MainWindow::on_actionQuit_triggered()
{
}


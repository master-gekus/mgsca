#include "main_window.h"
#include "./ui_main_window.h"

#include <QApplication>
#include <QString>
#include <QSettings>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

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

  connect(ui->actionQuit, SIGNAL(triggered()), SLOT(close()));

  update_ui();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (current_ca_.modified()) {
    switch (QMessageBox::question(this, QApplication::applicationName(),
                                  QStringLiteral("File \"%1\" is not saved.\nSave file?").arg(current_file_name()),
                                  QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No
                                  | QMessageBox::StandardButton::Cancel)) {
    case QMessageBox::Yes:
      event->ignore();
      save(false, true);
      break;

    case QMessageBox::No:
      event->accept();
      break;

    default:
      event->ignore();
      break;
    }
  } else {
    event->accept();
  }
}

MainWindow::~MainWindow()
{
  QSettings settings;
  settings.beginGroup(MAIN_WINDOW_GROUP);
  settings.setValue(GEOMETRY_KEY, saveGeometry());
  settings.setValue(STATE_KEY, saveState());

  delete ui;
}

inline QString MainWindow::current_file_name() const
{
  return current_file_.isEmpty() ? QStringLiteral("<untitled>") : QFileInfo(current_file_).completeBaseName();
}

void MainWindow::update_ui()
{
  setWindowTitle(WINDOW_TITLE.arg(current_file_name(), QApplication::applicationName()));
}

void MainWindow::save(bool ask_name, bool close_on_success)
{
  QString name{".sca"};
  if (current_file_.isEmpty()) {
    ask_name = true;
  }
  else {
    name = QFileInfo{current_file_}.canonicalFilePath();
  }

  if (ask_name) {
    name = QFileDialog::getSaveFileName(this, QStringLiteral("Save SCA database"), name,
                                        QStringLiteral("SCA Databases (*.sca);;All files (*.*)"));
    if (name.isEmpty()) {
      return;
    }
  }

  if (current_ca_.save(this, name)) {
    current_file_ = name;
    update_ui();
    if (close_on_success) {
      QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
    }
  }
}

void MainWindow::open(QString file_name)
{
  QFileInfo fi{file_name};
  if (!fi.exists()) {
    QMessageBox::critical(this, QApplication::applicationName(), QStringLiteral("\"%1\": File not found").arg(file_name));
    return;
  }

  SCADocument newca;
  if (!newca.load(this, fi.canonicalFilePath())) {
    return;
  }

  current_ca_ = ::std::move(newca);
  current_file_ = fi.canonicalFilePath();
  update_ui();
}

void MainWindow::on_actionOpen_triggered()
{
  QString name = QFileDialog::getOpenFileName(this, QStringLiteral("Open SCA database"), QString{},
                                              QStringLiteral("SCA Databases (*.sca);;All files (*.*)"));
  if (name.isEmpty()) {
    return;
  }
  open(name);
}

void MainWindow::on_actionSave_triggered()
{
  save(false, false);
}

void MainWindow::on_actionSaveAs_triggered()
{
  save(true, false);
}

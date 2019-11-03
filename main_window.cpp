#include "main_window.h"
#include "./ui_main_window.h"

#include <QApplication>
#include <QString>
#include <QSettings>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>

#include "certificate_item.h"
#include "certificate_edit_dialog.h"

#define WINDOW_TITLE QStringLiteral("%1%2 :: %3")
#define MAIN_WINDOW_GROUP QStringLiteral("Main Window")
#define GEOMETRY_KEY QStringLiteral("Geometry")
#define STATE_KEY QStringLiteral("State")

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow{parent},
  ui{new Ui::MainWindow},
  idle_handler_{this}
{
  ui->setupUi(this);

  QSettings settings;
  settings.beginGroup(MAIN_WINDOW_GROUP);
  restoreGeometry(settings.value(GEOMETRY_KEY).toByteArray());
  restoreState(settings.value(STATE_KEY).toByteArray());

  ui->actionNew->setShortcut(QKeySequence::New);
  ui->actionOpen->setShortcut(QKeySequence::Open);
  ui->actionSave->setShortcut(QKeySequence::Save);
  ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
  ui->actionQuit->setShortcut(QKeySequence::Quit);

  connect(ui->actionQuit, SIGNAL(triggered()), SLOT(close()));
  connect(&idle_handler_, SIGNAL(idle()), SLOT(idle_update_ui()));

  current_ca_.attachTree(ui->tree);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (!check_modified()) {
    event->ignore();
  }
  else {
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

void MainWindow::idle_update_ui()
{
  setWindowTitle(WINDOW_TITLE.arg(current_ca_.displayName(),
                                  current_ca_.modified() ? QStringLiteral("*") : QStringLiteral(""),
                                  QApplication::applicationName()));
  bool has_selected{nullptr != selectedCert()};
  ui->actionCertDelete->setEnabled(has_selected);
  ui->actionCertEdit->setEnabled(has_selected);
}

void MainWindow::set_document(SCADocument&& newca)
{
  current_ca_ = ::std::move(newca);
  current_ca_.attachTree(ui->tree);
}

bool MainWindow::check_modified()
{
  if (current_ca_.modified()) {
    switch (QMessageBox::question(this, QApplication::applicationName(),
                                  QStringLiteral("File \"%1\" is not saved.\nSave file?").arg(current_ca_.displayName()),
                                  QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No
                                  | QMessageBox::StandardButton::Cancel)) {
    case QMessageBox::Yes:
      if (!save(false)) {
        return false;
      }
      break;

    case QMessageBox::No:
      return true;

    default:
      return false;
    }
  }

  return (!current_ca_.modified());
}

bool MainWindow::save(bool ask_name)
{
  QString file_name{current_ca_.fileName()};
  if (file_name.isEmpty()) {
    file_name = QStringLiteral(".sca");
    ask_name = true;
  }

  if (ask_name) {
    file_name = QFileDialog::getSaveFileName(this, QStringLiteral("Save SCA database"), file_name,
                                        QStringLiteral("SCA Databases (*.sca);;All files (*.*)"));
    if (file_name.isEmpty()) {
      return false;
    }
  }

  if (!current_ca_.save(file_name)) {
    QMessageBox::critical(this, QApplication::applicationName(),
                          QStringLiteral("Error while saving file.\n%1").arg(current_ca_.errorString()));
    return false;
  }

  return true;
}

bool MainWindow::open(QString file_name)
{
  SCADocument newca;
  if (!newca.load(file_name)) {
    QMessageBox::critical(this, QApplication::applicationName(),
                          QStringLiteral("Error open file.\n%1").arg(newca.errorString()));
    return false;
  }

  set_document(::std::move(newca));
  return true;
}

CertificateItem* MainWindow::selectedCert() const
{
  const auto items{ui->tree->selectedItems()};

  if (items.isEmpty()) {
    return nullptr;
  }
  else {
    return dynamic_cast<CertificateItem*>(items.first());
  }
}

inline QTreeWidgetItem* MainWindow::safe_issuer(QTreeWidgetItem* issuer) const
{
  return (nullptr == issuer) ? ui->tree->invisibleRootItem() : issuer;
}

void MainWindow::update_and_select(QTreeWidgetItem* issuer, QTreeWidgetItem* cert)
{
  issuer->sortChildren(0, Qt::AscendingOrder);
  ui->tree->scrollToItem(cert);
  ui->tree->setCurrentItem(cert);
  cert->setSelected(true);
}

void MainWindow::on_actionNew_triggered()
{
  if (!check_modified()) {
    return;
  }

  set_document(SCADocument{});
}

void MainWindow::on_actionOpen_triggered()
{
  if (!check_modified()) {
    return;
  }

  QString name = QFileDialog::getOpenFileName(this, QStringLiteral("Open SCA database"), QString{},
                                              QStringLiteral("SCA Databases (*.sca);;All files (*.*)"));
  if (name.isEmpty()) {
    return;
  }
  open(name);
}

void MainWindow::on_actionSave_triggered()
{
  save(false);
}

void MainWindow::on_actionSaveAs_triggered()
{
  save(true);
}

void MainWindow::on_actionCertDelete_triggered()
{
}

void MainWindow::on_actionCertNew_triggered()
{
  CertificateEditDialog ced{this, current_ca_};
  ced.setIssuer(selectedCert());

  if (QDialog::Accepted != ced.exec()) {
    return;
  }

  QTreeWidgetItem *issuer{safe_issuer(ced.issuer())};

  CertificateItem *cert = ced.certificate();
  issuer->addChild(cert);
  update_and_select(issuer, cert);
}

void MainWindow::on_actionCertEdit_triggered()
{
  auto cert{selectedCert()};
  if (!cert) {
    return;
  }
  QTreeWidgetItem* old_issuer{safe_issuer(cert->parent())};

  CertificateEditDialog ced{this, current_ca_, cert};
  if (QDialog::Accepted != ced.exec()) {
    return;
  }

  QTreeWidgetItem* issuer{safe_issuer(ced.issuer())};
  if (issuer != old_issuer) {
    old_issuer->takeChild(old_issuer->indexOfChild(cert));
    issuer->addChild(cert);
  }
  else {
    cert->updateView();
  }

  update_and_select(issuer, cert);
}

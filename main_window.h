#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "idle_handler.h"
#include "document.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CertificateItem;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

protected:
  void closeEvent(QCloseEvent *event) override;

private slots:
  void on_actionNew_triggered();
  void on_actionOpen_triggered();
  void on_actionSave_triggered();
  void on_actionSaveAs_triggered();

  void on_actionCertDelete_triggered();
  void on_actionCertNewClient_triggered();
  void on_actionCertNewRoot_triggered();

  void idle_update_ui();

private:
  void set_document(SCADocument&& newca);
  bool check_modified();
  bool open(QString file_name);
  bool save(bool ask_name);
  CertificateItem* selectedCert() const;

private:
  Ui::MainWindow *ui;

private:
  IdleHandler idle_handler_;
  SCADocument current_ca_;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "document.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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

  void on_actionCertNew_triggered();

private:
  void update_ui();
  void set_document(SCADocument&& newca);
  bool check_modified();
  bool open(QString file_name);
  bool save(bool ask_name);

private:
  Ui::MainWindow *ui;

private:
  SCADocument current_ca_;
};
#endif // MAINWINDOW_H

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
  void on_actionOpen_triggered();
  void on_actionSave_triggered();
  void on_actionSaveAs_triggered();

  void open(QString file_name);

private:
  QString current_file_name() const;
  void update_ui();
  void save(bool ask_name, bool close_on_success);

private:
  Ui::MainWindow *ui;

private:
  QString current_file_;
  SCADocument current_ca_;
};
#endif // MAINWINDOW_H

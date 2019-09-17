#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void on_actionOpen_triggered();
  void on_actionSave_triggered();
  void on_actionSaveAs_triggered();
  void on_actionQuit_triggered();

private:
  void update_ui();

private:
  Ui::MainWindow *ui;

private:
  QFileInfo current_file_;
};
#endif // MAINWINDOW_H

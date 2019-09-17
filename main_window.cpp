#include "main_window.h"
#include "./ui_main_window.h"

#include <openssl/ssl.h>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ::SSL_library_init();
}

MainWindow::~MainWindow()
{
  delete ui;
}


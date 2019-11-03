#include "certificate_edit_dialog.h"
#include "ui_certificate_edit_dialog.h"

#include <QMenu>

CertificateEditDialog::CertificateEditDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CertificateEditDialog)
{
  ui->setupUi(this);

  setWindowFlag(Qt::WindowContextHelpButtonHint, false);

  ui->radioVersion1->setChecked(true);

  QMenu *quickIntervalMenu{new QMenu};
  quickIntervalMenu->addAction(trUtf8("1 year"));
  quickIntervalMenu->addAction(trUtf8("2 years"));
  quickIntervalMenu->addAction(trUtf8("3 years"));
  quickIntervalMenu->addAction(trUtf8("5 years"));
  quickIntervalMenu->addAction(trUtf8("10 years"));
  quickIntervalMenu->addAction(trUtf8("30 years"));
  ui->btnQuickInterval->setMenu(quickIntervalMenu);
}

CertificateEditDialog::~CertificateEditDialog()
{
  delete ui;
}

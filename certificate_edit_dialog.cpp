#include "certificate_edit_dialog.h"
#include "ui_certificate_edit_dialog.h"

#include <QMenu>

#include "certificate_item.h"
#include "document.h"

CertificateEditDialog::CertificateEditDialog(QWidget *parent, SCADocument& doc, CertificateItem* cert) :
  QDialog(parent),
  doc_(doc),
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

  ui->comboIssuer->addItem("<self-signed>", QVariant::fromValue(nullptr));
  add_childs_to_issuers_combo(doc_.invisibleRootItem(), cert);

  if (nullptr != cert) {
    cert_data_ = cert->certificateData();
    select_issuer(cert->issuer());
  }

  setup_dialog_data();
}

CertificateEditDialog::~CertificateEditDialog()
{
  delete ui;
}

QTreeWidgetItem* CertificateEditDialog::issuer() const
{
  int index = ui->comboIssuer->currentIndex();
  if (0 > index) {
    return nullptr;
  }
  return static_cast<QTreeWidgetItem*>(ui->comboIssuer->itemData(index).value<void*>());
}

void CertificateEditDialog::setIssuer(CertificateItem* issuer)
{
  select_issuer(issuer);
}

void CertificateEditDialog::setup_dialog_data()
{
}

void CertificateEditDialog::add_childs_to_issuers_combo(QTreeWidgetItem* parent, CertificateItem* const exclude)
{
  for (int i = 0; i < parent->childCount(); ++i) {
    CertificateItem *item = dynamic_cast<CertificateItem*>(parent->child(i));
    if ((nullptr == item) || (exclude == item)) {
      continue;
    }
    ui->comboIssuer->addItem(item->text(0), QVariant::fromValue(static_cast<void*>(item)));
    add_childs_to_issuers_combo(item, exclude);
  }
}

void CertificateEditDialog::select_issuer(CertificateItem* issuer)
{
  for (int i = 0; i < ui->comboIssuer->count(); ++i) {
    if (static_cast<CertificateItem*>(ui->comboIssuer->itemData(i).value<void*>()) == issuer) {
      ui->comboIssuer->setCurrentIndex(i);
      return;
    }
  }
  ui->comboIssuer->setCurrentIndex(0);
}

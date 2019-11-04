#include "certificate_edit_dialog.h"
#include "ui_certificate_edit_dialog.h"

#include <QMenu>
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>
#include <QSettings>

#include "utils.h"
#include "certificate_item.h"
#include "document.h"

#define DIALOG_GROUP QStringLiteral("Certificate Edit Dialog")
#define GEOMETRY_KEY QStringLiteral("Geometry")

CertificateEditDialog::CertificateEditDialog(QWidget *parent, SCADocument& doc, CertificateItem* cert) :
  QDialog(parent),
  doc_(doc),
  ui(new Ui::CertificateEditDialog)
{
  ui->setupUi(this);

  setWindowFlag(Qt::WindowContextHelpButtonHint, false);

  QSettings settings;
  settings.beginGroup(DIALOG_GROUP);
  restoreGeometry(settings.value(GEOMETRY_KEY).toByteArray());
  Utils::restoreElementsState(this, settings);

  ui->radioVersion1->setChecked(true);

  QMenu *quickIntervalMenu{new QMenu};
  quickIntervalMenu->addAction(trUtf8("1 year"), [=](){set_year_interval(1);});
  quickIntervalMenu->addAction(trUtf8("2 years"), [=](){set_year_interval(2);});
  quickIntervalMenu->addAction(trUtf8("3 years"), [=](){set_year_interval(3);});
  quickIntervalMenu->addAction(trUtf8("5 years"), [=](){set_year_interval(5);});
  quickIntervalMenu->addAction(trUtf8("10 years"), [=](){set_year_interval(10);});
  quickIntervalMenu->addAction(trUtf8("30 years"), [=](){set_year_interval(30);});
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
  QSettings settings;
  settings.beginGroup(DIALOG_GROUP);
  settings.setValue(GEOMETRY_KEY, saveGeometry());
  Utils::saveElementsState(this, settings);

  delete ui;
}

void CertificateEditDialog::done(int r)
{
  if ((QDialog::Accepted == r) && (!validate_and_save())) {
    return;
  }

  QDialog::done(r);
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
  // Version
  if (1 == cert_data_.version()) {
    ui->radioVersion1->setChecked(true);
  }
  else {
    ui->radioVersion3->setChecked(true);
  }

  // Validity
  ui->dateValidFrom->setDateTime(cert_data_.validFrom());
  ui->dateValidTill->setDateTime(cert_data_.validTill());
}

bool CertificateEditDialog::validate_and_save()
{
  cert_data_.setVersion(ui->radioVersion1->isChecked() ? 1 : 3);

  return validate_and_save_validity();
}

bool CertificateEditDialog::validate_and_save_validity()
{
  const QDateTime valid_from{ui->dateValidFrom->dateTime()};
  const QDateTime valid_till{ui->dateValidTill->dateTime()};
  const QDateTime now{QDateTime::currentDateTime()};

  if (valid_from >= valid_till) {
    QMessageBox::critical(this, QApplication::applicationName(),
                          QStringLiteral("Invalid validity interval: \"Till\" before \"From\"."));
   focus_widget_and_activate_tab(ui->dateValidTill);
   return false;
  }

  if (now > valid_till) {
    if (QMessageBox::Yes
        != QMessageBox::question(this, QApplication::applicationName(),
                          QStringLiteral("Certificate validity is expired. Are youe sure?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
      focus_widget_and_activate_tab(ui->dateValidTill);
      return false;
    }
  }

  if (now < valid_from) {
    if (QMessageBox::Yes
        != QMessageBox::question(this, QApplication::applicationName(),
                          QStringLiteral("Certificate validity will starts in the future. Are youe sure?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
      focus_widget_and_activate_tab(ui->dateValidTill);
      return false;
    }
  }

  cert_data_.setValidFrom(valid_from);
  cert_data_.setValidTill(valid_till);
  return true;
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

inline void CertificateEditDialog::set_year_interval(int years) noexcept
{
  ui->dateValidTill->setDateTime(ui->dateValidFrom->dateTime().addYears(years));
}

void CertificateEditDialog::focus_widget_and_activate_tab(QWidget* widget)
{
  if (nullptr == widget) {
    return;
  }

  // TODO: Activate tab of widget
  widget->setFocus();
}

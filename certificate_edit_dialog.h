#ifndef CERTIFICATE_EDIT_DIALOG_H
#define CERTIFICATE_EDIT_DIALOG_H

#include <QDialog>

#include "certificate_data.h"

class SCADocument;
class CertificateItem;
class QTreeWidgetItem;

namespace Ui {
class CertificateEditDialog;
}

class CertificateEditDialog : public QDialog
{
  Q_OBJECT

public:
  CertificateEditDialog(QWidget *parent, SCADocument& doc, CertificateItem* cert = nullptr);
  ~CertificateEditDialog() override;

protected:
  void done(int r) override;

public:
  QTreeWidgetItem* issuer() const;
  void setIssuer(CertificateItem* issuer);

  const CertificateData& certificateData() const noexcept;

private:
  void setup_dialog_data();
  bool validate_and_save();
  bool validate_and_save_validity();

  void add_childs_to_issuers_combo(QTreeWidgetItem* parent, CertificateItem* const exclude);
  void select_issuer(CertificateItem* issuer);
  void set_year_interval(int years) noexcept;
  void focus_widget_and_activate_tab(QWidget* widget);

private:
  SCADocument& doc_;
  CertificateData cert_data_;

private:
  Ui::CertificateEditDialog *ui;
};

inline const CertificateData& CertificateEditDialog::certificateData() const noexcept
{
  return cert_data_;
}

#endif // CERTIFICATE_EDIT_DIALOG_H

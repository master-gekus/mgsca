#ifndef CERTIFICATE_EDIT_DIALOG_H
#define CERTIFICATE_EDIT_DIALOG_H

#include <QDialog>

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
  ~CertificateEditDialog();

public:
  CertificateItem* certificate() const;
  QTreeWidgetItem* issuer() const;
  void setIssuer(CertificateItem* issuer);

private:
  void add_childs_to_issuers_combo(QTreeWidgetItem* parent);
  void select_issuer(CertificateItem* issuer);

private:
  SCADocument& doc_;
  CertificateItem* cert_;

private:
  Ui::CertificateEditDialog *ui;
};

#endif // CERTIFICATE_EDIT_DIALOG_H

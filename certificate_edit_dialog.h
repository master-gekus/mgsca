#ifndef CERTIFICATE_EDIT_DIALOG_H
#define CERTIFICATE_EDIT_DIALOG_H

#include <QDialog>

namespace Ui {
class CertificateEditDialog;
}

class CertificateEditDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CertificateEditDialog(QWidget *parent = nullptr);
  ~CertificateEditDialog();

private:
  Ui::CertificateEditDialog *ui;
};

#endif // CERTIFICATE_EDIT_DIALOG_H

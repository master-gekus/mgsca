#ifndef CERTIFICATEITEM_H
#define CERTIFICATEITEM_H

#include <QTreeWidgetItem>

class CertificateItem : public QTreeWidgetItem
{
public:
  CertificateItem();
  explicit CertificateItem(CertificateItem* parent);

protected:
  QVariant data(int column, int role) const override;

private:
  Q_DISABLE_COPY(CertificateItem)
};

#endif // CERTIFICATEITEM_H

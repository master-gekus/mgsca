#include "certificate_item.h"

CertificateItem::CertificateItem()
{
}

CertificateItem::CertificateItem(CertificateItem* parent) :
  QTreeWidgetItem{parent}
{

}

QVariant CertificateItem::data(int column, int role) const
{
  switch (column) {
  case 0:
    switch (role) {
    case Qt::DisplayRole:
      return QStringLiteral("Name of certificate");

    default:
      break;
    }
    break;

  default:
    break;
  }

  return QTreeWidgetItem::data(column, role);
}

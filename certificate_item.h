#ifndef CERTIFICATEITEM_H
#define CERTIFICATEITEM_H

#include <QString>
#include <QTreeWidgetItem>

namespace YAML {
class Node;
class Emitter;
}

class CertificateData;

class CertificateItem final : public QTreeWidgetItem
{
public:
  CertificateItem();
  explicit CertificateItem(const CertificateData& certificate_data);
  ~CertificateItem() override;

private:
  Q_DISABLE_COPY(CertificateItem)

protected:
  QVariant data(int column, int role) const override;

public:
  void save(::YAML::Emitter& emitter) const noexcept;
  bool load(const YAML::Node& node, QString& error_string) noexcept;

public:
  CertificateItem* issuer() const;

  const CertificateData& certificateData() const noexcept;
  void setCertificateData(const CertificateData& certificate_data) noexcept;

private:
  class Private;
  Private *d_;
};

inline CertificateItem* CertificateItem::issuer() const
{
  return dynamic_cast<CertificateItem*>(parent());
}

#endif // CERTIFICATEITEM_H

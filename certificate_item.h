#ifndef CERTIFICATEITEM_H
#define CERTIFICATEITEM_H

#include <QString>
#include <QTreeWidgetItem>

namespace YAML {
class Node;
class Emitter;
}

class CertificateItem : public QTreeWidgetItem
{
public:
  CertificateItem();
  ~CertificateItem() override;

protected:
  QVariant data(int column, int role) const override;

public:
  virtual void save(::YAML::Emitter& emitter) const noexcept;
  virtual bool load(const YAML::Node& node, QString& error_string) noexcept;

public:
  void updateView();

private:
  Q_DISABLE_COPY(CertificateItem)

private:
  class Private;
  Private *d_;
};

#endif // CERTIFICATEITEM_H

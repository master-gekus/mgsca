#ifndef CERTIFICATEDATA_H
#define CERTIFICATEDATA_H

#include <QSharedDataPointer>
#include <QString>

namespace YAML {
class Node;
class Emitter;
}

class CertificateData
{
public:
  CertificateData();
  CertificateData(const CertificateData&);
  CertificateData &operator=(const CertificateData &);
  ~CertificateData();

public:
  void save(::YAML::Emitter& emitter) const noexcept;
  bool load(const YAML::Node& node, QString& error_string) noexcept;

public:
  int version() const noexcept;
  void setVersion(int new_version) noexcept;

  // TODO: temporary, to remove
  const QString& name() const noexcept;
  void setName(const QString& new_name) noexcept;

private:
  class Private;
  QSharedDataPointer<Private> d_;
};

#endif // CERTIFICATEDATA_H

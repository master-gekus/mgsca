#include "certificate_data.h"

#include <QByteArray>

#include <yaml-cpp/yaml.h>

namespace YAML {

template<>
struct convert<QString> {
  static Node encode(const QString& rhs) {
    return Node{rhs.toUtf8().toStdString()};
  }

  static bool decode(const Node& node, QString& rhs) {
    rhs = QString::fromUtf8(QByteArray::fromStdString(node.as<::std::string>()));
    return true;
  }
};

template<>
struct convert<QDateTime> {
  static Node encode(const QDateTime& rhs) {
    return Node{rhs.toTime_t()};
  }

  static bool decode(const Node& node, QDateTime& rhs) {
    rhs = QDateTime::fromTime_t(node.as<uint>());
    return true;
  }
};

} // namespace YAML

class CertificateData::Private : public QSharedData
{
public:
  Private() noexcept;

public:
  void save(::YAML::Emitter& emitter) const noexcept;
  bool load(const YAML::Node& node, QString& error_string) noexcept;

public:
  template<typename T>
  static void save(::YAML::Emitter& emitter, const char* key, const T& value) noexcept;

  template<typename T>
  static void load(const ::YAML::Node& node, const char* key, T& value);

public:
  int version_;
  QDateTime valid_from_;
  QDateTime valid_till_;

  // TODO: temporary, to remove
  QString name_;

public:
  static const constexpr char* name_key_ = "Name";
  static const constexpr char* version_key_ = "Version";
  static const constexpr char* valid_from_key_ = "Valid from";
  static const constexpr char* valid_till_key_ = "Valid till";
};

inline CertificateData::Private::Private() noexcept :
  version_{3},
  valid_from_{QDateTime::currentDateTime()},
  valid_till_{valid_from_.addYears(1)}
{
  // TODO: temporary, to remove
  static int counter = 0;
  name_ = QStringLiteral("Certificate #%1").arg(++counter);
}

template<typename T>
inline void CertificateData::Private::save(::YAML::Emitter& emitter, const char* key, const T& value) noexcept
{
  emitter << ::YAML::Key << key << ::YAML::Value << ::YAML::Node{value};
}

template<typename T>
inline void CertificateData::Private::load(const ::YAML::Node& node, const char* key, T& value)
{
  value = node[key].as<T>(value);
}

void CertificateData::Private::save(::YAML::Emitter& emitter) const noexcept
{
  save(emitter, version_key_, version_);
  save(emitter, valid_from_key_, valid_from_);
  save(emitter, valid_till_key_, valid_till_);

  // TODO: temporary, to remove
  save(emitter, name_key_, name_);
}

bool CertificateData::Private::load(const YAML::Node& node, QString& error_string) noexcept
{
  try {
    load(node, version_key_, version_);
    load(node, valid_from_key_, valid_from_);
    load(node, valid_till_key_, valid_till_);

    // TODO: temporary, to remove
    load(node, name_key_, name_);
  }
  catch (::YAML::Exception& e)
  {
    error_string = QStringLiteral("[%1:%2]: %3").arg(e.mark.line).arg(e.mark.column)
                   .arg(QString::fromUtf8(QByteArray::fromStdString(e.msg)));
    return false;
  }
  catch (...)
  {
    error_string = QStringLiteral("CertificateItem::load(): unknown exception.");
    return false;
  }

  return true;
}

CertificateData::CertificateData() :
  d_(new Private)
{
}

CertificateData::CertificateData(const CertificateData &other)
  : d_(other.d_)
{
}

CertificateData &CertificateData::operator=(const CertificateData &other)
{
  if (this != &other)
    d_.operator=(other.d_);
  return *this;
}

CertificateData::~CertificateData()
{
}

void CertificateData::save(::YAML::Emitter& emitter) const noexcept
{
  d_->save(emitter);
}

bool CertificateData::load(const YAML::Node& node, QString& error_string) noexcept
{
  return d_->load(node, error_string);
}

int CertificateData::version() const noexcept
{
  return d_->version_;
}

void CertificateData::setVersion(int new_version) noexcept
{
  switch (new_version) {
  case 1:
  case 3:
    d_->version_ = new_version;
    break;

  default:
    break;
  }
}

// TODO: temporary, to remove
const QString& CertificateData::name() const noexcept
{
  return d_->name_;
}

void CertificateData::setName(const QString& new_name) noexcept
{
  d_->name_ = new_name;
}

QDateTime CertificateData::validFrom() const noexcept
{
  return d_->valid_from_;
}

void CertificateData::setValidFrom(QDateTime date_time) noexcept
{
  d_->valid_from_ = date_time;
}

QDateTime CertificateData::validTill() const noexcept
{
  return d_->valid_till_;
}

void CertificateData::setValidTill(QDateTime date_time) noexcept
{
  d_->valid_till_ = date_time;
}

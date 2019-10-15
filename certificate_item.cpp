#include "certificate_item.h"

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

} // namespace YAML

class CertificateItem::Private final
{
public:
  explicit Private(CertificateItem* owner) noexcept;

public:
  const QString& name() const noexcept;

public:
  void save(::YAML::Emitter& emitter) const noexcept;
  bool load(const ::YAML::Node& node, QString& error_string) noexcept;

public:
  template<typename T>
  static void save(::YAML::Emitter& emitter, const char* key, const T& value) noexcept;

  template<typename T>
  static void load(const ::YAML::Node& node, const char* key, T& value);

private:
  CertificateItem* owner_;
  unsigned version_;
  QString name_;

private:
  static const constexpr char* name_key_ = "Name";
  static const constexpr char* version_key_ = "Version";
  static const constexpr char* certificates_key_ = "Certificates";
};

inline CertificateItem::Private::Private(CertificateItem* owner) noexcept :
  owner_{owner},
  version_{3}
{
  static int counter = 0;
  name_ = QStringLiteral("Certificate #%1").arg(++counter);
}

template<typename T>
inline void CertificateItem::Private::save(::YAML::Emitter& emitter, const char* key, const T& value) noexcept
{
  emitter << ::YAML::Key << key << ::YAML::Value << ::YAML::Node{value};
}

template<typename T>
inline void CertificateItem::Private::load(const ::YAML::Node& node, const char* key, T& value)
{
  value = node[key].as<T>(value);
}

inline const QString& CertificateItem::Private::name() const noexcept
{
  return name_;
}

inline void CertificateItem::Private::save(::YAML::Emitter& emitter) const noexcept
{
  emitter << ::YAML::BeginMap;

  save(emitter, version_key_, version_);
  save(emitter, name_key_, name_);

  // Nestes certificates
  emitter << ::YAML::Key << certificates_key_ << ::YAML::Value << ::YAML::BeginSeq;
  for (int i = 0; i < owner_->childCount(); ++i) {
    auto cert = dynamic_cast<const CertificateItem*>(owner_->child(i));
    if (nullptr != cert) {
      cert->save(emitter);
    }
  }
  emitter << ::YAML::EndSeq;

  emitter << ::YAML::EndMap;
}

inline bool CertificateItem::Private::load(const ::YAML::Node& node, QString& error_string) noexcept
{
  if (!node.IsMap()) {
    error_string = QStringLiteral("[%1:%2]: Node is not a map.").arg(node.Mark().line).arg(node.Mark().column);
    return false;
  }

  try {
    load(node, version_key_, version_);
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

  ::YAML::Node nested = node[certificates_key_];
  if (nested.IsSequence()) {
    for (const auto& nested_node : nested) {
      CertificateItem* cert{new CertificateItem{}};
      owner_->addChild(cert);
      if (!cert->load(nested_node, error_string)) {
        return false;
      }
    }
  }

  return true;
}

CertificateItem::CertificateItem() :
  d_{new Private{this}}
{
}

CertificateItem::~CertificateItem()
{
  delete d_;
}

QVariant CertificateItem::data(int column, int role) const
{
  switch (column) {
  case 0:
    switch (role) {
    case Qt::DisplayRole:
      return d_->name();

    default:
      break;
    }
    break;

  default:
    break;
  }

  return QTreeWidgetItem::data(column, role);
}

void CertificateItem::save(::YAML::Emitter& emitter) const noexcept
{
  d_->save(emitter);
}

bool CertificateItem::load(const ::YAML::Node& node, QString& error_string) noexcept
{
  return d_->load(node, error_string);
}

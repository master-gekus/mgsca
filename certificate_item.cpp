#include "certificate_item.h"

#include <QByteArray>

#include <yaml-cpp/yaml.h>

#include "certificate_data.h"

class CertificateItem::Private final
{
public:
  explicit Private(CertificateItem* owner) noexcept;
  Private(CertificateItem* owner, const CertificateData& certificate_data) noexcept;

public:
  const CertificateData& certificateData() const noexcept;
  void setCertificateData(const CertificateData& certificate_data) noexcept;

public:
  void save(::YAML::Emitter& emitter) const noexcept;
  bool load(const ::YAML::Node& node, QString& error_string) noexcept;

private:
  CertificateItem* owner_;
  CertificateData data_;

private:
  static const constexpr char* certificates_key_ = "Certificates";
};

inline CertificateItem::Private::Private(CertificateItem* owner) noexcept :
  owner_{owner}
{
}

inline CertificateItem::Private::Private(CertificateItem* owner, const CertificateData& certificate_data) noexcept :
  owner_{owner},
  data_{certificate_data}
{
}

const CertificateData& CertificateItem::Private::certificateData() const noexcept
{
  return data_;
}

void CertificateItem::Private::setCertificateData(const CertificateData& certificate_data) noexcept
{
  data_ = certificate_data;
}

inline void CertificateItem::Private::save(::YAML::Emitter& emitter) const noexcept
{
  emitter << ::YAML::BeginMap;

  data_.save(emitter);

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

  if (!data_.load(node, error_string)) {
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

CertificateItem::CertificateItem(const CertificateData& certificate_data) :
  d_{new Private{this, certificate_data}}
{
}

CertificateItem::~CertificateItem()
{
  delete d_;
}

QVariant CertificateItem::data(int column, int role) const
{
  switch (role) {
  case Qt::DisplayRole:
    switch (column) {
    case 0:
      return d_->certificateData().name();

    case 1:
      return d_->certificateData().validFrom().toString(Qt::SystemLocaleShortDate);

    case 2:
      return d_->certificateData().validTill().toString(Qt::SystemLocaleShortDate);

    default:
      break;
    }
    break;

  case Qt::TextAlignmentRole:
    switch (column) {
    case 1:
    case 2:
      return static_cast<int>(Qt::AlignVCenter | Qt::AlignHCenter);

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

const CertificateData& CertificateItem::certificateData() const noexcept
{
  return d_->certificateData();
}

void CertificateItem::setCertificateData(const CertificateData& certificate_data) noexcept
{
  d_->setCertificateData(certificate_data);
  emitDataChanged();
}

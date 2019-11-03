#include "document.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <sstream>

#include <yaml-cpp/yaml.h>

#include "certificate_item.h"

class SCADocument::Private : public QSharedData
{
public:
  Private() noexcept;
  ~Private() noexcept;

private:
  Q_DISABLE_COPY(Private)

public:
  bool modified() const noexcept;
  const QString& errorString() const noexcept;
  QString fileName() const noexcept;
  QString displayName() const noexcept;
  bool save(const QString& fileName) const noexcept;
  bool load(const QString& fileName) noexcept;

  void attachTree(QTreeWidget* tree) const noexcept;

  QTreeWidgetItem* invisibleRootItem() const noexcept;

private:
  void detachTree() const noexcept;
  void clearFakeTopLevelItem() const noexcept;

private:
  mutable bool modified_;
  mutable QString error_string_;
  mutable QFileInfo file_info_;
  mutable QMetaObject::Connection widget_destroyed_connection_;
  mutable QMetaObject::Connection data_changed_connection_;
  mutable QMetaObject::Connection rows_inserted_connection_;
  mutable QMetaObject::Connection rows_moved_connection_;
  mutable QMetaObject::Connection rows_removed_connection_;

  mutable QTreeWidget *tree_;
  mutable QTreeWidgetItem fake_top_level_item_;

private:
  static const char* root_certificates_key_;
};

const char* SCADocument::Private::root_certificates_key_ = "Root Certificates";

inline SCADocument::Private::Private() noexcept :
  modified_{false},
  tree_{nullptr}
{
}

inline SCADocument::Private::~Private() noexcept
{
  detachTree();
  clearFakeTopLevelItem();
}

inline bool SCADocument::Private::modified() const noexcept
{
  return modified_;
}

inline const QString& SCADocument::Private::errorString() const noexcept
{
  return error_string_;
}

inline QString SCADocument::Private::fileName() const noexcept
{
  return file_info_.canonicalFilePath();
}

inline QString SCADocument::Private::displayName() const noexcept
{
  return file_info_.isFile() ? file_info_.baseName() : QStringLiteral("<untitled>");
}

inline bool SCADocument::Private::save(const QString& file_name) const noexcept
{
  QFile file{file_name};
  if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
    error_string_ = QStringLiteral("Error create file \"%1\": %2").arg(QDir::toNativeSeparators(file_name),
                                                                       file.errorString());
    return false;
  }

  ::std::stringstream ss;
  ::YAML::Emitter e{ss};
  e << ::YAML::Comment(
         "This file is created by " + QApplication::applicationName().toUtf8().toStdString() + "\n"
         "Do not edit it directly without strong necessity."
         );

  e << ::YAML::BeginMap << root_certificates_key_ << ::YAML::BeginSeq;

  QTreeWidgetItem* root{tree_ ? tree_->invisibleRootItem() : &fake_top_level_item_};
  for (int i = 0; i < root->childCount(); ++i) {
    CertificateItem const* cert = dynamic_cast<CertificateItem const*>(root->child(i));
    if (cert) {
      cert->save(e);
    }
  }

  e << ::YAML::EndSeq << ::YAML::EndMap;

  auto res{ss.str()};
  file.write(res.data(), static_cast<qint64>(res.size()));
  file.close();

  file_info_.setFile(file);
  modified_ = false;

  error_string_.clear();
  return true;
}

inline bool SCADocument::Private::load(const QString& file_name) noexcept
{
  QFile file{file_name};
  if (!file.open(QFile::ReadOnly)) {
    error_string_ = QStringLiteral("Error open file \"%1\": %2").arg(QDir::toNativeSeparators(file_name), file.errorString());
    return false;
  }

  if ((1024 * 1024) < file.size()) {
    error_string_ = QStringLiteral("\"%1\": File is too big.").arg(QDir::toNativeSeparators(file_name));
    return false;
  }

  ::YAML::Node newwhole;
  try {
    newwhole = ::YAML::Load(file.readAll().toStdString());
  }
  catch (::std::exception& e) {
    error_string_ = QStringLiteral("\"%1\": Error parsing file: %2").arg(QDir::toNativeSeparators(file_name),
                                                                         QObject::trUtf8(e.what()));
    return false;
  }
  catch (...) {
    error_string_ = QStringLiteral("\"%1\": Error parsing file: unknown exception").arg(QDir::toNativeSeparators(file_name));
    return false;
  }
  file.close();

  QList<QTreeWidgetItem*> newcerts;

  ::YAML::Node certs = newwhole[root_certificates_key_];
  if ((!certs.IsDefined()) || (!certs.IsSequence())) {
    error_string_ = QStringLiteral("\"%1\": Invalid file format (required key \"%2\" not found or not a sequence.")
                    .arg(QDir::toNativeSeparators(file_name), QObject::trUtf8(root_certificates_key_));
    return false;
  }

  for (const auto& cert : certs) {
    QString error_string;
    CertificateItem *item = new CertificateItem{};
    if (!item->load(cert, error_string)) {
      error_string_ = error_string;
      for (auto& i : newcerts) {
        delete i;
      }
      return false;
    }
    newcerts.append(item);
  }

  if (tree_) {
    tree_->addTopLevelItems(newcerts);
  }
  else {
    clearFakeTopLevelItem();
    fake_top_level_item_.addChildren(newcerts);
  }

  file_info_.setFile(file);
  modified_ = false;

  error_string_.clear();
  return true;
}

void SCADocument::Private::attachTree(QTreeWidget* tree) const noexcept
{
  Q_ASSERT(tree);

  if (tree == tree_) {
    return;
  }

  detachTree();

  tree_ = tree;

  tree_->invisibleRootItem()->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<void*>(static_cast<const void*>(this))));
  tree->addTopLevelItems(fake_top_level_item_.takeChildren());

  widget_destroyed_connection_ = QObject::connect(tree_, &QTreeWidget::destroyed, [this](QObject* obj){
    if (obj == tree_) {
      detachTree();
    }
  });

  auto data_changed_fn = [this](){
    modified_ = true;
  };
  auto model = tree->model();

  data_changed_connection_  = QObject::connect(model, &QAbstractItemModel::dataChanged, data_changed_fn);
  rows_inserted_connection_ = QObject::connect(model, &QAbstractItemModel::rowsInserted, data_changed_fn);
  rows_moved_connection_    = QObject::connect(model, &QAbstractItemModel::rowsMoved, data_changed_fn);
  rows_removed_connection_  = QObject::connect(model, &QAbstractItemModel::rowsRemoved, data_changed_fn);
}

QTreeWidgetItem* SCADocument::Private::invisibleRootItem() const noexcept
{
  return (nullptr == tree_) ? (&fake_top_level_item_) : tree_->invisibleRootItem();
}

void SCADocument::Private::detachTree() const noexcept
{
  QObject::disconnect(widget_destroyed_connection_);
  QObject::disconnect(data_changed_connection_);
  QObject::disconnect(rows_inserted_connection_);
  QObject::disconnect(rows_moved_connection_);
  QObject::disconnect(rows_removed_connection_);

  QTreeWidget* tree{nullptr};
  ::std::swap(tree, tree_);

  if (nullptr != tree) {
    Private* old_doc{static_cast<Private*>(tree->invisibleRootItem()->data(0, Qt::UserRole).value<void*>())};
    if (nullptr != old_doc) {
      old_doc->detachTree();
    }

    clearFakeTopLevelItem();
    fake_top_level_item_.addChildren(tree->invisibleRootItem()->takeChildren());
  }
}

void SCADocument::Private::clearFakeTopLevelItem() const noexcept
{
  for (auto& i : fake_top_level_item_.takeChildren()) {
    delete i;
  }
}

SCADocument::SCADocument() noexcept:
  d_{new Private}
{
}

SCADocument::~SCADocument() noexcept
{
}

SCADocument& SCADocument::operator=(const SCADocument& other) noexcept
{
  d_.operator=(other.d_);
  return *this;
}

SCADocument& SCADocument::operator=(SCADocument&& other) noexcept
{
  d_.operator=(::std::move(other.d_));
  return *this;
}

bool SCADocument::modified() const noexcept
{
  return d_->modified();
}

QString SCADocument::errorString() const noexcept
{
  return d_->errorString();
}

QString SCADocument::fileName() const noexcept
{
  return d_->fileName();
}

QString SCADocument::displayName() const noexcept
{
  return d_->displayName();
}

bool SCADocument::save(const QString& file_name) const noexcept
{
  return d_->save(file_name);
}

bool SCADocument::load(const QString& file_name) noexcept
{
  return d_->load(file_name);
}

void SCADocument::attachTree(QTreeWidget* tree) noexcept
{
  d_->attachTree(tree);
}

QTreeWidgetItem* SCADocument::invisibleRootItem() const noexcept
{
  return d_->invisibleRootItem();
}

#include "document.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTreeWidgetItem>

#include <sstream>

#include <yaml-cpp/yaml.h>

#include "certificate_item.h"

class SCADocument::Private : public QSharedData
{
public:
  Private();

public:
  bool modified() const noexcept;
  const QString& error_string() const noexcept;
  QString file_name() const noexcept;
  QString display_name() const noexcept;
  bool save(const QString& file_name) const noexcept;
  bool load(const QString& file_name) noexcept;

  // Temporary, to be removed
  void set_modified() noexcept;

private:
  mutable bool modified_;
  mutable QString error_string_;
  mutable QFileInfo file_info_;

private:
  static const char* certificates_key_;
};

const char* SCADocument::Private::certificates_key_ = "Certificates";

inline SCADocument::Private::Private() :
  modified_{false}
{
}

inline bool SCADocument::Private::modified() const noexcept
{
  return modified_;
}

inline const QString& SCADocument::Private::error_string() const noexcept
{
  return error_string_;
}

inline QString SCADocument::Private::file_name() const noexcept
{
  return file_info_.canonicalFilePath();
}

inline QString SCADocument::Private::display_name() const noexcept
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
         "Do not edit it directly."
         );

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

//  ::YAML::Node newcerts = newwhole[certificates_key_];
//  if ((!newcerts.IsDefined()) || (!newcerts.IsSequence())) {
//    error_string_ = QStringLiteral("\"%1\": Error parsing file: invalid file format").arg(QDir::toNativeSeparators(file_name));
//    return false;
//  }

  file_info_.setFile(file);
  modified_ = false;

  error_string_.clear();
  return true;
}

// Temporary, to be removed
void SCADocument::Private::set_modified() noexcept
{
  modified_ = true;
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

QString SCADocument::error_string() const noexcept
{
  return d_->error_string();
}

QString SCADocument::file_name() const noexcept
{
  return d_->file_name();
}

QString SCADocument::display_name() const noexcept
{
  return d_->display_name();
}

bool SCADocument::save(const QString& file_name) const noexcept
{
  return d_->save(file_name);
}

bool SCADocument::load(const QString& file_name) noexcept
{
  return d_->load(file_name);
}

void SCADocument::set_modified() noexcept
{
  d_->set_modified();
}

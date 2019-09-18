#include "document.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>

#include <sstream>

#include <yaml-cpp/yaml.h>

class SCADocument::Private : public QSharedData
{
public:
  Private();

public:
  bool modified() const noexcept;
  bool save(QWidget* parent, const QString& file_name) const noexcept;
  bool load(QWidget* parent, const QString& file_name) noexcept;

private:
  mutable bool modified_;
  ::YAML::Node whole_;
  ::YAML::Node certificates_;

private:
  static const char* certificates_key_;
};

const char* SCADocument::Private::certificates_key_ = "Certificates";

inline SCADocument::Private::Private() :
  modified_{false},
  whole_{::YAML::NodeType::Map},
  certificates_{::YAML::NodeType::Sequence}
{
  whole_[certificates_key_] = certificates_;
}

inline bool SCADocument::Private::modified() const noexcept
{
  return modified_;
}

inline bool SCADocument::Private::save(QWidget* parent, const QString& file_name) const noexcept
{
  QFile file{file_name};
  if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
    if (parent) {
      QMessageBox::critical(parent, QApplication::applicationName(),
                            QStringLiteral("Error create file \"%1\": %2").arg(file_name, file.errorString()));
    }
    return false;
  }

  ::std::stringstream ss;
  ::YAML::Emitter e{ss};
  e << ::YAML::Comment(
         "This file is created by " + QApplication::applicationName().toUtf8().toStdString() + "\n"
         "Do not edit it directly."
         );
  e << whole_;

  auto res{ss.str()};
  file.write(res.data(), static_cast<qint64>(res.size()));
  file.close();

  modified_ = false;
  return true;
}

inline bool SCADocument::Private::load(QWidget* parent, const QString& file_name) noexcept
{
  QFile file{file_name};
  if (!file.open(QFile::ReadOnly)) {
    if (parent) {
      QMessageBox::critical(parent, QApplication::applicationName(),
                            QStringLiteral("Error open file \"%1\": %2").arg(file_name, file.errorString()));
    }
    return false;
  }

  if ((1024 * 1024) < file.size()) {
    if (parent) {
      QMessageBox::critical(parent, QApplication::applicationName(),
                            QStringLiteral("\"%1\": File is too big.").arg(file_name));
    }
    return false;
  }

  ::YAML::Node newwhole;
  try {
    newwhole = ::YAML::Load(file.readAll().toStdString());
  }
  catch (::std::exception& e) {
    if (parent) {
      QMessageBox::critical(parent, QApplication::applicationName(),
                            QStringLiteral("\"%1\": Error parsing file: %2").arg(file_name, trUtf8(e.what())));
    }
    return false;
  }
  catch (...) {
    if (parent) {
      QMessageBox::critical(parent, QApplication::applicationName(),
                            QStringLiteral("\"%1\": Error parsing file: unknown exception").arg(file_name));
    }
    return false;
  }
  file.close();

  ::YAML::Node newcerts = newwhole[certificates_key_];
  if ((!newcerts.IsDefined()) || (!newcerts.IsSequence())) {
    if (parent) {
      QMessageBox::critical(parent, QApplication::applicationName(),
                            QStringLiteral("\"%1\": Error parsing file: invalid file format").arg(file_name));
    }
    return false;
  }

  whole_.reset(newwhole);
  certificates_.reset(newcerts);
  modified_ = false;

  return true;
}

SCADocument::SCADocument(QObject *parent) :
  QObject{parent},
  d_{new Private}
{
}

SCADocument::~SCADocument()
{
}

SCADocument& SCADocument::operator=(const SCADocument& other)
{
  d_.operator=(other.d_);
  return *this;
}

SCADocument& SCADocument::operator=(SCADocument&& other)
{
  d_.operator=(::std::move(other.d_));
  return *this;
}

bool SCADocument::modified() const
{
  return d_->modified();
}

bool SCADocument::save(QWidget* parent, const QString& file_name) const
{
  return d_->save(parent, file_name);
}

bool SCADocument::load(QWidget* parent, const QString& file_name)
{
  return d_->load(parent, file_name);
}

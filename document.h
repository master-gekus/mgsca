#ifndef SCADOCUMENT_H
#define SCADOCUMENT_H

#include <QString>
#include <QExplicitlySharedDataPointer>

class QWidget;
class QTreeWidget;

class SCADocument final
{
public:
  SCADocument() noexcept;
  SCADocument(const SCADocument& other) noexcept;
  SCADocument(SCADocument&& other) noexcept;
  ~SCADocument() noexcept;

public:
  SCADocument& operator=(const SCADocument& other) noexcept;
  SCADocument& operator=(SCADocument&& other) noexcept;

public:
  bool modified() const noexcept;
  QString error_string() const noexcept;
  QString file_name() const noexcept;
  QString display_name() const noexcept;
  bool save(const QString& file_name) const  noexcept;
  bool load(const QString& file_name) noexcept;

  // Temporary, to be removed
  void set_modified() noexcept;

  void show_in_tree(QTreeWidget *tree) const;

private:
  class Private;
  QExplicitlySharedDataPointer<Private> d_;
};

#endif // SCADOCUMENT_H

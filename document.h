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
  QString errorString() const noexcept;
  QString fileName() const noexcept;
  QString displayName() const noexcept;
  bool save(const QString& fileName) const  noexcept;
  bool load(const QString& fileName) noexcept;

  void attachTree(QTreeWidget* tree) noexcept;

private:
  class Private;
  QExplicitlySharedDataPointer<Private> d_;
};

#endif // SCADOCUMENT_H

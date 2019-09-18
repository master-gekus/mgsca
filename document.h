#ifndef SCADOCUMENT_H
#define SCADOCUMENT_H

#include <QObject>
#include <QExplicitlySharedDataPointer>

class QWidget;
class SCADocument : public QObject
{
  Q_OBJECT

public:
  explicit SCADocument(QObject *parent = nullptr);
  SCADocument(const SCADocument& other);
  SCADocument(SCADocument&& other);
  ~SCADocument() override;

public:
  SCADocument& operator=(const SCADocument& other);
  SCADocument& operator=(SCADocument&& other);

public:
  bool modified() const;
  bool save(QWidget* parent, const QString& file_name) const;
  bool load(QWidget* parent, const QString& file_name);

signals:

public slots:

private:
  class Private;
  QExplicitlySharedDataPointer<Private> d_;
};

#endif // SCADOCUMENT_H

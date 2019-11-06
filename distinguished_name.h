#ifndef DISTINGUISHEDNAME_H
#define DISTINGUISHEDNAME_H

#include <QSharedDataPointer>

class DistinguishedName
{
public:
  DistinguishedName();
  DistinguishedName(const DistinguishedName &);
  DistinguishedName &operator=(const DistinguishedName &);
  ~DistinguishedName();

private:
  class Private;
  QSharedDataPointer<Private> d_;
};

#endif // DISTINGUISHEDNAME_H

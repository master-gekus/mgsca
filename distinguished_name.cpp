#include "distinguished_name.h"

class DistinguishedName::Private : public QSharedData
{
public:

};

DistinguishedName::DistinguishedName() :
  d_(new Private)
{
}

DistinguishedName::DistinguishedName(const DistinguishedName &other) :
  d_(other.d_)
{
}

DistinguishedName &DistinguishedName::operator=(const DistinguishedName &other)
{
  if (this != &other)
    d_.operator=(other.d_);
  return *this;
}

DistinguishedName::~DistinguishedName()
{
}

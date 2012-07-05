#pragma once

#include <boost/uuid/uuid.hpp>
#include <string>

namespace hns
{

class ID : public boost::uuids::uuid
{
public:
  ID();
  ID(const boost::uuids::uuid& id);

  const std::string short_string() const;
  static ID create();
  static const ID& null();

};

}

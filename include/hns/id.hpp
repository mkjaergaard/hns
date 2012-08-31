#pragma once

#include <boost/uuid/uuid.hpp>
#include <string>
#include <set>
#include <boost/uuid/uuid_serialize.hpp>

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

typedef std::set<ID> IDListType;

}

namespace boost
{
namespace serialization
{

template<class Archive>
void serialize(Archive & ar, hns::ID& id, const unsigned int version)
{
//  ar & boost::serialization::base_object<boost::uuids::uuid>(&id);
}

} // namespace serialization
} // namespace boost


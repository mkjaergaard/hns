#include <hns/id.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace hns
{

boost::uuids::random_generator id_random_generator_;

const ID id_null_ = boost::uuids::nil_uuid();

ID::ID() :
  boost::uuids::uuid(boost::uuids::nil_uuid())
{
}

ID::ID(const boost::uuids::uuid& id) :
  boost::uuids::uuid(id)
{
}

const std::string ID::short_string() const
{
  return to_string(*this).substr(0, 8);
}

ID ID::create()
{
  return id_random_generator_();
}

const ID& ID::null()
{
  return id_null_;
}

}

#pragma once

#include <boost/uuid/uuid.hpp>

namespace hns
{

class MyCallback
{
public:
  virtual void tagEntryAdded(const boost::uuids::uuid& tag_id,
			     const boost::uuids::uuid& alias_id,
			     const boost::uuids::uuid& entity_id) = 0;
  virtual void tagEntryRemoved(const boost::uuids::uuid& tag_id,
			       const boost::uuids::uuid& entity_id) = 0;

};

}

#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <darc/id.hpp>

using namespace darc;

namespace hns
{

class PseudoTag
{
protected:

  ID id_;
  std::string name1_;
  ID namespace1_id_;

  std::string name2_;
  ID namespace2_id_;

public:
  PseudoTag(const ID& id,
	    const ID& namespace1_id,
	    const std::string& name1,
	    const ID& namespace2_id,
	    const std::string& name2) :
    id_(id),
    namespace1_id_(namespace1_id),
    name1_(name1),
    namespace2_id_(namespace2_id),
    name2_(name2)
  {
  }

// Getters
  const ID& getID() const
  {
    return id_;
  }

  const std::string& getName1() const
  {
    return name1_;
  }

  const std::string& getName2() const
  {
    return name2_;
  }

  const ID& getNamespace1() const
  {
    return namespace1_id_;
  }

  const ID& getNamespace2() const
  {
    return namespace2_id_;
  }

};

typedef boost::shared_ptr<PseudoTag> PseudoTagPtr;

}

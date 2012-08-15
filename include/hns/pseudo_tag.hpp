#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <hns/id.hpp>

namespace hns
{

class PseudoTag
{
protected:
  typedef ID IDType;

  IDType id_;
  std::string name1_;
  IDType namespace1_id_;

  std::string name2_;
  IDType namespace2_id_;

public:
  PseudoTag(const IDType& id,
	    const IDType& namespace1_id,
	    const std::string& name1,
	    const IDType& namespace2_id,
	    const std::string& name2) :
    id_(id),
    namespace1_id_(namespace1_id),
    name1_(name1),
    namespace2_id_(namespace2_id),
    name2_(name2)
  {
  }

// Getters
  const IDType& getID() const
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

  const IDType& getNamespace1() const
  {
    return namespace1_id_;
  }

  const IDType& getNamespace2() const
  {
    return namespace2_id_;
  }

};

typedef boost::shared_ptr<PseudoTag> PseudoTagPtr;

}

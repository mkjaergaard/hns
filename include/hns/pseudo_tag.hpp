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

  std::string name_;
  IDType id_;
  IDType namespace_id_;

  IDType target_namespace_id_;
  std::string target_name_;
  // peger på?

public:
  PseudoTag(const IDType& id,
	    const IDType& namespace_id,
	    const std::string& name,
	    const IDType& target_namespace_id,
	    const std::string& target_name) :
    id_(id),
    namespace_id_(namespace_id),
    name_(name),
    target_namespace_id_(target_namespace_id),
    target_name_(target_name)
  {
  }

// Getters
  const std::string& getName() const
  {
    return name_;
  }

  const std::string& getTargetName() const
  {
    return target_name_;
  }

  const IDType& getTargetNamespace() const
  {
    return target_namespace_id_;
  }

};

typedef boost::shared_ptr<PseudoTag> PseudoTagPtr;

}

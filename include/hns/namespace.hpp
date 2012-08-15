#pragma once

#include <set>
#include <string>
#include <boost/shared_ptr.hpp>
#include <hns/id.hpp>

namespace hns
{

class Namespace
{
public:
  typedef ID IDType;

  typedef std::set<IDType> IDListType;

protected:
  IDListType node_list_;
  IDListType child_namespace_list_;
  IDListType tag_list_;
  IDListType pseudo_tag_list_;

  IDType id_;
  IDType parent_id_;

  std::string name_;

public:
  Namespace(const IDType& id,
	    const IDType& parent_id,
	    const std::string& name) :
    id_(id),
    parent_id_(parent_id),
    name_(name)
  {
  }

// Getters
  IDListType& accessChildNamespaceList()
  {
    return child_namespace_list_;
  }

  const IDListType& accessChildNamespaceList() const
  {
    return child_namespace_list_;
  }

  IDListType& accessTagList()
  {
    return tag_list_;
  }

  const IDListType& accessTagList() const
  {
    return tag_list_;
  }

  IDListType& accessPseudoTagList()
  {
    return pseudo_tag_list_;
  }

  const IDListType& accessPseudoTagList() const
  {
    return pseudo_tag_list_;
  }

  const std::string& getName() const
  {
    return name_;
  }

  const IDType& getID() const
  {
    return id_;
  }

  const IDType& getParentNamespace() const
  {
    return parent_id_;
  }

};

typedef boost::shared_ptr<Namespace> NamespacePtr;

}

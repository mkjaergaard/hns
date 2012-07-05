#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <hns/id.hpp>
#include <hns/tag_callback_i.hpp>

namespace hns
{

class Tag
{
protected:
  typedef ID IDType;
  typedef std::set<IDType> NodeListType;

  std::string name_;
  IDType id_;
  NodeListType node_list_;

  IDType namespace_id_;

  size_t users_;

  typedef std::vector<MyCallback*> SubscriberListType;
  SubscriberListType subscribers_;

public:
  Tag(const IDType& id, const IDType& namespace_id, const std::string& name) :
    id_(id),
    namespace_id_(namespace_id),
    name_(name)
  {
  }

  const std::string& getName() const
  {
    return name_;
  }

  const IDType& getNamespace()
  {
    return namespace_id_;
  }

  void addSubscriber(MyCallback* callback)
  {
    subscribers_.push_back(callback);
  }

  void triggerSubscribers(const IDType& tag_id,
			  const IDType& alias_id,
			  const IDType& entity_id)
  {
    for(SubscriberListType::iterator it = subscribers_.begin();
	it != subscribers_.end();
	it++)
    {
      (*it)->tagEntryAdded(tag_id, alias_id, entity_id);
    }
  }

};

typedef boost::shared_ptr<Tag> TagPtr;

}

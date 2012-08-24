#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <hns/id.hpp>
#include <hns/tag_callback_i.hpp>

namespace hns
{

typedef enum
{
  TagRemoved = 0,
  TagAdded = 1
} TagEvent;

class Tag
{
protected:
  typedef ID IDType;
  typedef std::set<IDType> NodeListType;

  IDType id_;
  IDType namespace_id_;

  std::string name_;
  //NodeListType node_list_;

public:
  typedef boost::function<void(IDType, IDType, TagEvent)> TagListenerType;

protected:
  typedef boost::signal<void(IDType, IDType, TagEvent)> TagListenerSignalType;
  TagListenerSignalType tag_listeners_;

public:
  Tag(const IDType& id, const IDType& namespace_id, const std::string& name) :
    id_(id),
    namespace_id_(namespace_id),
    name_(name)
  {
  }

  void triggerAddedAlias(const IDType& alias_tag_id)
  {
    tag_listeners_(id_, alias_tag_id, TagAdded);
  }

  void triggerRemovedAlias(const IDType& alias_tag_id)
  {
    tag_listeners_(id_, alias_tag_id, TagRemoved);
  }

  const std::string& getName() const
  {
    return name_;
  }

  const IDType& getID()
  {
    return id_;
  }

  const IDType& getNamespace()
  {
    return namespace_id_;
  }

  boost::signals::connection addSubscriber(TagListenerType callback)
  {
    return tag_listeners_.connect(callback);
  }

};

typedef boost::shared_ptr<Tag> TagPtr;

}

#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <darc/id.hpp>
#include <hns/tag_callback_i.hpp>

using namespace darc;

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
  typedef std::set<ID> NodeListType;

  ID id_;
  ID namespace_id_;

  std::string name_;

public:
  typedef boost::function<void(ID, ID, TagEvent)> TagListenerType;

protected:
  typedef boost::signal<void(ID, ID, TagEvent)> TagListenerSignalType;
  TagListenerSignalType tag_listeners_;

public:
  Tag(const ID& id, const ID& namespace_id, const std::string& name) :
    id_(id),
    namespace_id_(namespace_id),
    name_(name)
  {
  }

  void triggerAddedAlias(const ID& alias_tag_id)
  {
    tag_listeners_(id_, alias_tag_id, TagAdded);
  }

  void triggerRemovedAlias(const ID& alias_tag_id)
  {
    tag_listeners_(id_, alias_tag_id, TagRemoved);
  }

  const std::string& getName() const
  {
    return name_;
  }

  const ID& getID() const
  {
    return id_;
  }

  const ID& getNamespace()
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

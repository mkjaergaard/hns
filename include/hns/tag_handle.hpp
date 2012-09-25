#pragma once

#include <boost/signals/connection.hpp>

#include <hns/tag.hpp>
#include <hns/tree.hpp>

namespace hns
{

class TagHandle
{
protected:
  TagPtr tag_instance_;
  Tree * tree_;

  boost::signals::connection connection_;

public:
  TagHandle() :
    tree_(0)
  {
  }

  TagHandle(Tree * tree, TagPtr tag_instance) :
    tag_instance_(tag_instance),
    tree_(tree)
  {
  }

  ~TagHandle()
  {
    if(tag_instance_.get() != 0)
    {
      ID id = tag_instance_->getID();
      connection_.disconnect();
      tag_instance_.reset();
      tree_->flushTag(id);
    }
  }

  void addSubscriber(Tag::TagListenerType listener)
  {
    // todo: make sure we dont have a previous connection
    connection_ = tag_instance_->addSubscriber(listener);
  }

  size_t count()
  {
    return tree_->countTags(
      tag_instance_->getNamespace(),
      tag_instance_->getName()
      );
  }

  const ID& id() const
  {
    return tag_instance_->getID();
  }

};

}

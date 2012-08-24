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
  TagHandle(TagPtr tag_instance) :
    tag_instance_(tag_instance)
  {
  }

  ~TagHandle()
  {
    ID id = tag_instance_->getID();
    tag_instance_.reset();
    tree_->flushTag(id);
  }

  void addSubscriber(Tag::TagListenerType listener)
  {
    // todo: make sure we dont have a previous connection
    connection_ = tag_instance_->addSubscriber(listener);
  }


};

}

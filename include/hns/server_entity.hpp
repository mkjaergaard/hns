#pragma once

#include <map>
#include <set>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <hns/tag_parser.hpp>
#include <hns/tag.hpp>
#include <hns/pseudo_tag.hpp>
#include <hns/namespace.hpp>
#include <hns/tree.hpp>
//#include <llog/context.hpp>

namespace hns
{

class ServerEntity
{
protected:
  Tree tree_;
  typedef ID IDType;

public:
  ServerEntity()
  {
  }

  const IDType& registerPseudoTag(const std::string& tag, const std::string& target_tag)
  {
    return tree_.registerPseudoTag(tag, target_tag);
  }

  void subscribeTag(const std::string& tag, MyCallback* callback)
  {
    return tree_.subscribeTag(tag, callback);
  }

  const IDType& registerTag(const std::string& tag)
  {
    return tree_.registerTag(tag);
  }

};

}

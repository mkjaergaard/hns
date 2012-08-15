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

#include <llog/context.hpp>
#include <llog/logger.hpp>

#include <hns/tag_parser_arg.hpp>

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

  // based on full string
  const IDType& registerTag(const std::string& tag)
  {
    IDType current_ns_id = tree_.getRootNS();

    TagParser tag_parser(tag);

    llog::log(llog::Severity::Debug, "registerTag",
	      "Tag", llog::Argument<TagParser>(tag_parser));

    for(TagParser::NamespaceListType::const_iterator it = tag_parser.getNamespaces().begin();
	it != tag_parser.getNamespaces().end();
	it++)
    {
      current_ns_id = tree_.registerNamespace(current_ns_id, *it);
    }

    return tree_.registerTag(current_ns_id, tag_parser.getTag());
  }

/*
  void subscribeTag(const std::string& tag, MyCallback* callback)
  {
    const IDType& tag_id = registerTag(tag);

    TagPtr& tag_entry = findTag(tag_id);

    //tag_entry->addSubscriber(callback);
    std::set<IDType> group;
    populateGroupFromTag(tag_id, group);
    std::cout << "Size of group " << group.size() << std::endl;
  }
*/
  const IDType& registerPseudoTag(const std::string& tag1, const std::string& tag2)
  {
    // **********
    // Source Tag
    IDType ns1_id = tree_.getRootNS();

    TagParser tag1_parser(tag1);
    for(TagParser::NamespaceListType::const_iterator it = tag1_parser.getNamespaces().begin();
	it != tag1_parser.getNamespaces().end();
	it++)
    {
      ns1_id = tree_.registerNamespace(ns1_id, *it);
    }

    ///////
    // Target tag:
    IDType ns2_id = tree_.getRootNS();

    TagParser tag2_parser(tag2);
    for(TagParser::NamespaceListType::const_iterator it = tag2_parser.getNamespaces().begin();
	it != tag2_parser.getNamespaces().end();
	it++)
    {
      ns2_id = tree_.registerNamespace(ns2_id, *it);
    }

    llog::log(llog::Severity::Debug, "registerPseudoTag",
	      "Tag1", llog::Argument<TagParser>(tag1_parser),
	      "Tag2", llog::Argument<TagParser>(tag2_parser));


    return tree_.registerPseudoTag(ns1_id, tag1_parser.getTag(),
				   ns2_id, tag2_parser.getTag());
  }

  IDType createNS(const std::vector<std::string>& name_list)
  {
    IDType ns_id = tree_.getRootNS();
    for(TagParser::NamespaceListType::const_iterator it = name_list.begin();
	it != name_list.end();
	it++)
    {
      ns_id = tree_.registerNamespace(ns_id, *it);
    }
    return ns_id;
  }

  int countTags(const std::string& tag)
  {
    TagParser tag_parser(tag);
    IDType ns_id = createNS(tag_parser.getNamespaces());
    return tree_.countTags(ns_id, tag_parser.getTag());
  }

  void subscribeTag(const std::string& tag, MyCallback* callback)
  {
    // return tree_.subscribeTag(tag, callback);
  }


};

}

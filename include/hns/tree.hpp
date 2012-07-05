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
#include <hns/id.hpp>
//#include <llog/context.hpp>

namespace hns
{

class Tree
{
protected:
  typedef hns::ID IDType;

  typedef std::set<IDType> IDListType;
  typedef std::map<IDType, NamespacePtr> NamespaceListType;
  typedef std::map<IDType, TagPtr> TagListType;
  typedef std::map<IDType, PseudoTagPtr> PseudoTagListType;

  NamespaceListType namespace_list_;
  TagListType tag_list_;
  PseudoTagListType pseudo_tag_list_;

  IDType current_ns_id_;

public:
  Tree() :
    current_ns_id_(IDType::create())
  {
    namespace_list_.insert(NamespaceListType::value_type(current_ns_id_,
							 boost::make_shared<Namespace>(current_ns_id_,
										       IDType::null(),
										       ".") ));
  }

  TagPtr& findTag(const IDType& tag_id)
  {
    TagListType::iterator item = tag_list_.find(tag_id);
    assert(item != tag_list_.end());
    return item->second;
  }

  PseudoTagPtr& findPseudoTag(const IDType& pseudo_tag_id)
  {
    PseudoTagListType::iterator item = pseudo_tag_list_.find(pseudo_tag_id);
    assert(item != pseudo_tag_list_.end());
    return item->second;
  }

  NamespacePtr& findNamespace(const IDType& ns_id)
  {
    NamespaceListType::iterator item = namespace_list_.find(ns_id);
    assert(item != namespace_list_.end());
    return item->second;
  }

  void populateGroupFindLocalPseudoTags(const std::string& target_tag_name,
					const NamespacePtr target_ns,
					std::set<IDType>& group,
					std::set<IDType>& visited_ns_list)
  {
//    std::cout << "populateGroupFindLocalPseudoTags " << target_tag_name << std::endl;

    for(IDListType::const_iterator it = target_ns->accessPseudoTagList().begin();
	it != target_ns->accessPseudoTagList().end();
	it++)
    {
      PseudoTagPtr pseudo_tag = findPseudoTag(*it);
      if (pseudo_tag->getTargetName() == target_tag_name)
      {
	if(visited_ns_list.find(*it) == visited_ns_list.end())
	{
	  visited_ns_list.insert(*it);

	  populateGroupFromTagName(pseudo_tag->getName(), target_ns, group, visited_ns_list);
	}
      }
      else if(pseudo_tag->getName() == target_tag_name)
      {
	if(visited_ns_list.find(*it) == visited_ns_list.end())
	{
	  visited_ns_list.insert(*it);
	  populateGroupFromTagName(pseudo_tag->getTargetName(),
				   findNamespace(pseudo_tag->getTargetNamespace()),
				   group,
				   visited_ns_list);
	}
      }
    }
  }

  void populateGroupFindParentPseudoTags(const std::string& target_tag_name,
					 const NamespacePtr target_ns,
					 std::set<IDType>& group,
					 std::set<IDType>& visited_ns_list)
  {
//    std::cout << "populateGroupFindParentPseudoTags " << target_tag_name << std::endl;

    // 3 - search for remappings in parent namespace.
    if(target_ns->getParentNamespace() == IDType::null())
    {
      std::cout << "Not searching entity parent namespace for pseudo tags" << std::endl;
    }
    else
    {
      NamespacePtr parent_ns = findNamespace(target_ns->getParentNamespace());
      for(IDListType::const_iterator it = parent_ns->accessPseudoTagList().begin();
	  it != parent_ns->accessPseudoTagList().end();
	  it++)
      {
	PseudoTagPtr pseudo_tag = findPseudoTag(*it);
	if(pseudo_tag->getTargetNamespace() == target_ns->getID() &&
	   pseudo_tag->getTargetName() == target_tag_name)
	{
	  // match!
//	    std::cout << "PSeudo Match.." << target_tag_name << std::endl;
	  if(visited_ns_list.find(*it) == visited_ns_list.end())
	  {
	    visited_ns_list.insert(*it);

	    populateGroupFromTagName(pseudo_tag->getName(), parent_ns, group, visited_ns_list);
	  }
	}
      }
    }
  }

// based on tag
  void populateGroupFromTagName(const std::string& tag_name,
				const NamespacePtr ns,
				std::set<IDType>& group,
				std::set<IDType>& visited_ns_list)
  {
//    std::cout << "populateGroupFromTagName " << tag_name << std::endl;

    // 2 - search for tags with same name
    for(IDListType::const_iterator it = ns->accessTagList().begin();
	it != ns->accessTagList().end();
	it++)
    {
      TagPtr child_tag = findTag(*it);
//      std::cout << "testing tag " << child_tag->getName() << std::endl;
      if(child_tag->getName() == tag_name)
      {
	if(group.find(*it) == group.end())
	{
//	  std::cout << "Insert" << std::endl;
	  group.insert(*it);
	}
	else
	{
	  return; // allready visited, bail out
	}
      }
    }
    populateGroupFindLocalPseudoTags(tag_name, ns, group, visited_ns_list);
    populateGroupFindParentPseudoTags(tag_name, ns, group, visited_ns_list);
  }

  void populateGroupFromTag(const IDType& tag_id,
			    std::set<IDType>& group)
  {
    std::set<IDType> visited_ns_list;
    TagPtr tag = findTag(tag_id);
    populateGroupFromTagName(tag->getName(),
			     findNamespace(tag->getNamespace()),
			     group,
			     visited_ns_list);
  }

  // not thread safe, only call from Server thread
  const IDType& hasChildNamespace(const boost::shared_ptr<const Namespace>& ns, const std::string& child_name)
  {
    for(IDListType::const_iterator it = ns->accessChildNamespaceList().begin();
	it != ns->accessChildNamespaceList().end();
	it++)
    {
      const IDType& child_ns_id = *it;
      NamespaceListType::iterator item = namespace_list_.find(child_ns_id);
      assert( item != namespace_list_.end() );

      if(item->second->getName() == child_name)
      {
	return child_ns_id;
      }
    }
    return IDType::null();
  }

  // todo: collect these in a templated function
  const IDType& hasTag(const boost::shared_ptr<const Namespace>& ns, const std::string& tag_name)
  {
    for(IDListType::const_iterator it = ns->accessTagList().begin();
	it != ns->accessTagList().end();
	it++)
    {
      const IDType& tag_id = *it;
      TagListType::iterator item = tag_list_.find(tag_id);
      assert( item != tag_list_.end() );

      if(item->second->getName() == tag_name)
      {
	return tag_id;
      }
    }
    return IDType::null();
  }

  // todo: collect these in a templated function
  const IDType& hasPseudoTag(const boost::shared_ptr<const Namespace>& ns, const std::string& tag_name)
  {
    for(IDListType::const_iterator it = ns->accessTagList().begin();
	it != ns->accessTagList().end();
	it++)
    {
      const IDType& tag_id = *it;
      PseudoTagListType::iterator item = pseudo_tag_list_.find(tag_id);
      assert( item != pseudo_tag_list_.end() );

      if(item->second->getName() == tag_name)
      {
	return tag_id;
      }
    }
    return IDType::null();
  }

  const IDType& registerNamespace(const IDType& parent_ns_id, const std::string& name)
  {
    NamespacePtr& parent = findNamespace(parent_ns_id);

    const IDType& child_id = hasChildNamespace(parent, name);

    if(child_id == IDType::null())
    {
      IDType new_id = IDType::create();
      NamespacePtr ns = boost::make_shared<Namespace>(new_id, parent_ns_id, name);
      parent->accessChildNamespaceList().insert(new_id);
      NamespaceListType::iterator new_item = namespace_list_.insert(NamespaceListType::value_type(new_id, ns)).first;
      std::cout << "Adding new Namespace " << name << std::endl;
      return new_item->first;
    }
    else
    {
      std::cout << "Namespace Already Exists " << name << std::endl;
      return child_id;
    }
  }

  // based on correct parent ns id
  const IDType& registerPseudoTag(const IDType& parent_ns_id,
					      const std::string& name,
					      const IDType& target_ns_id,
					      const std::string& target_name)
  {
    NamespacePtr& parent_ns = findNamespace(parent_ns_id);

    //todo: manage duplicate tags

    IDType new_id = IDType::create();
    PseudoTagPtr tag = boost::make_shared<PseudoTag>(new_id,
								     parent_ns_id,
								     name,
								     target_ns_id,
								     target_name);
    parent_ns->accessPseudoTagList().insert(new_id);
    PseudoTagListType::iterator new_item = pseudo_tag_list_.insert(PseudoTagListType::value_type(new_id, tag)).first;
    std::cout << "Adding new Pseudo Tag " << name << " -> " << target_name << std::endl;
    return new_item->first;
  }

  const IDType& registerTag(const IDType& parent_ns_id, const std::string& name)
  {
    //todo: collect in function
    NamespacePtr& parent = findNamespace(parent_ns_id);

    const IDType& tag_id = hasTag(parent, name);

    if(tag_id == IDType::null())
    {
      IDType new_id = IDType::create();
      TagPtr tag = boost::make_shared<Tag>(new_id, parent_ns_id, name);
      parent->accessTagList().insert(new_id);
      TagListType::iterator new_item = tag_list_.insert(TagListType::value_type(new_id, tag)).first;
      std::cout << "Adding new Tag " << name << std::endl;
      return new_item->first;
    }
    else
    {
      std::cout << "Tag Already Exists " << name << std::endl;
      return tag_id;
    }
  }

  // based on full string
  const IDType& registerTag(const std::string& tag)
  {
    IDType current_ns_id = current_ns_id_;

    TagParser tag_parser(tag);
    for(TagParser::NamespaceListType::const_iterator it = tag_parser.getNamespaces().begin();
	it != tag_parser.getNamespaces().end();
	it++)
    {
      std::cout << "NS:" << *it << std::endl;
      current_ns_id = registerNamespace(current_ns_id, *it);
    }
    std::cout << "Tag:" << tag_parser.getTag() << std::endl;
    return registerTag(current_ns_id, tag_parser.getTag());
  }

  void subscribeTag(const std::string& tag, MyCallback* callback)
  {
    const IDType& tag_id = registerTag(tag);

    TagPtr& tag_entry = findTag(tag_id);

    //tag_entry->addSubscriber(callback);
    std::set<IDType> group;
    populateGroupFromTag(tag_id, group);
    std::cout << "Size of group " << group.size() << std::endl;
  }

  const IDType& registerPseudoTag(const std::string& tag, const std::string& target_tag)
  {
    //////
    // Handle the tag name
    IDType current_ns_id = current_ns_id_;

    TagParser tag_parser(tag);
    for(TagParser::NamespaceListType::const_iterator it = tag_parser.getNamespaces().begin();
	it != tag_parser.getNamespaces().end();
	it++)
    {
      std::cout << "NS:" << *it << std::endl;
      current_ns_id = registerNamespace(current_ns_id, *it);
    }
    std::cout << "PseudoTag:" << tag_parser.getTag() << std::endl;

    ///////
    // Target tag:
    IDType target_ns_id = current_ns_id;

    TagParser target_tag_parser(target_tag);
    for(TagParser::NamespaceListType::const_iterator it = target_tag_parser.getNamespaces().begin();
	it != target_tag_parser.getNamespaces().end();
	it++)
    {
      target_ns_id = registerNamespace(current_ns_id, *it);
    }

    return registerPseudoTag(current_ns_id, tag_parser.getTag(), target_ns_id, target_tag_parser.getTag());
  }

  // callback ved alias tags

};

}

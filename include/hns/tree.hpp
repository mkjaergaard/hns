#pragma once

#include <map>
#include <set>
#include <string>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <hns/tag_parser.hpp>
#include <hns/tag.hpp>
#include <hns/pseudo_tag.hpp>
#include <hns/namespace.hpp>
#include <hns/id.hpp>

#include <llog/logger.hpp>
#include <hns/id_arg.hpp>
#include <hns/namespace_arg.hpp>

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

public:
  NamespaceListType namespace_list_;
  TagListType tag_list_;
  PseudoTagListType pseudo_tag_list_;

  IDType root_ns_id_;

public:
  Tree() :
    root_ns_id_(IDType::create())
  {
    NamespacePtr root_ns = boost::make_shared<Namespace>(root_ns_id_,
							 IDType::null(),
							 ".");
    namespace_list_.insert(NamespaceListType::value_type(root_ns_id_,
							 root_ns));
  }

  const IDType& getRootNS()
  {
    return root_ns_id_;
  }

private:
  // *************************************
  // Functions for looking up in the lists
  // *************************************
  template<class T>
  boost::shared_ptr<T>& find(std::map<IDType, boost::shared_ptr<T> >& list,
			     const IDType& id)
  {
    typename std::map<IDType, boost::shared_ptr<T> >::iterator item = list.find(id);
    assert(item != list.end());
    return item->second;
  }

  TagPtr& findTag(const IDType& tag_id)
  {
    return find<Tag>(tag_list_, tag_id);
  }

  PseudoTagPtr& findPseudoTag(const IDType& pseudo_tag_id)
  {
    return find<PseudoTag>(pseudo_tag_list_, pseudo_tag_id);
  }

  NamespacePtr& findNamespace(const IDType& ns_id)
  {
    return find<Namespace>(namespace_list_, ns_id);
  }

private:
  // *****************************
  // Functions for finding aliases
  // *****************************
  void searchPseudoTags(const std::string& tag_name,
			const NamespacePtr ns,
			std::set<IDType>& found_tags_list,
			std::set<IDType>& found_pseudo_tags_list)
  {
    llog::log(llog::Severity::Trace, "searchPseudoTags",
	      "In NS",   llog::Argument<std::string>(ns->getName()),
	      "For Tag", llog::Argument<std::string>(tag_name));

    for(IDListType::const_iterator it = ns->accessPseudoTagList().begin();
	it != ns->accessPseudoTagList().end();
	it++)
    {
      PseudoTagPtr pseudo_tag = findPseudoTag(*it);
      const ID& tag_id = pseudo_tag->getID();
      if(found_pseudo_tags_list.find(tag_id) == found_pseudo_tags_list.end())
      {
	if (pseudo_tag->getNamespace1() == ns->getID() && pseudo_tag->getName1() == tag_name)
	{
	  llog::log(llog::Severity::Trace, "Matched 1st",
		    "N1", llog::Argument<std::string>(pseudo_tag->getName1()),
		    "N2", llog::Argument<std::string>(pseudo_tag->getName2()));

	  found_pseudo_tags_list.insert(tag_id);
	  searchFromTagName(pseudo_tag->getName2(),
			    findNamespace(pseudo_tag->getNamespace2()),
			    found_tags_list,
			    found_pseudo_tags_list);
	}
	else if (pseudo_tag->getNamespace2() == ns->getID() && pseudo_tag->getName2() == tag_name)
	{
	  llog::log(llog::Severity::Trace, "Matched 2nd",
		    "N1", llog::Argument<std::string>(pseudo_tag->getName1()),
		    "N2", llog::Argument<std::string>(pseudo_tag->getName2()));

	  found_pseudo_tags_list.insert(tag_id);
	  searchFromTagName(pseudo_tag->getName1(),
			    findNamespace(pseudo_tag->getNamespace1()),
			    found_tags_list,
			    found_pseudo_tags_list);
	}
      }
      else
      {
	llog::log(llog::Severity::Trace, "Skipping Pseudotag",
		    "N1", llog::Argument<std::string>(pseudo_tag->getName1()),
		    "N2", llog::Argument<std::string>(pseudo_tag->getName2()));
      }

    }
  }

// based on tag name
  void searchFromTagName(const std::string& tag_name,
			 const NamespacePtr ns,
			 std::set<IDType>& found_tags_list,
			 std::set<IDType>& found_pseudo_tags_list)
  {
    llog::log(llog::Severity::Trace, "searchFromTagName",
	      "In NS",   llog::Argument<std::string>(ns->getName()),
	      "For Tag", llog::Argument<std::string>(tag_name));

    // Search for tags with the name
    for(IDListType::const_iterator it = ns->accessTagList().begin();
	it != ns->accessTagList().end();
	it++)
    {
      TagPtr child_tag = findTag(*it);
      if(child_tag->getName() == tag_name)
      {
	if(found_tags_list.find(*it) == found_tags_list.end())
	{
	  llog::log(llog::Severity::Trace, "Found tag");
	  found_tags_list.insert(*it);
	}
	else
	{
	  llog::log(llog::Severity::Trace, "Skipping");
	  return; // Namespace allready visited for this tag, just stop searching
	}
      }
    }
    searchPseudoTags(tag_name, ns, found_tags_list, found_pseudo_tags_list);
  }

private:
  // ************************************************
  // Function for finding childs
  // ************************************************
  template<class T>
  const IDType& hasChild(const boost::shared_ptr<const Namespace>& ns,
			 const std::string& child_name,
			 const IDListType& child_list,
			 const std::map<IDType, boost::shared_ptr<T> > instance_list)
  {
    for(IDListType::const_iterator it = child_list.begin();
	it != child_list.end();
	it++)
    {
      const IDType& child_id = *it;
      typename std::map<IDType, boost::shared_ptr<T> >::const_iterator item =
	instance_list.find(child_id);

      assert(item != instance_list.end());

      if(item->second->getName() == child_name)
      {
	return child_id;
      }
    }
    return IDType::null();
  }

  const IDType& hasChildNamespace(const boost::shared_ptr<const Namespace>& ns, const std::string& child_name)
  {
    return hasChild<Namespace>(ns,
			       child_name,
			       ns->accessChildNamespaceList(),
			       namespace_list_);
  }

  const IDType& hasTag(const boost::shared_ptr<const Namespace>& ns, const std::string& tag_name)
  {
    return hasChild<Tag>(ns,
			 tag_name,
			 ns->accessTagList(),
			 tag_list_);
  }

  void triggerNewTag(const TagPtr& tag, const NamespacePtr& ns)
  {
    std::set<IDType> tags_list;
    std::set<IDType> pseudo_tags_list;
    searchFromTagName(tag->getName(), ns, tags_list, pseudo_tags_list);

    for(std::set<IDType>::const_iterator it = tags_list.begin();
	it != tags_list.end();
	it++)
    {
      if(tag->getID() != *it) // todo: not sure if we want to trigger for our own tag?
      {
	TagPtr alias_tag = findTag(*it);
	tag->triggerAddedAlias(alias_tag->getID());
      }
    }
  }

public:
  // ********************************************
  // Functions for registering entries
  // ********************************************
  const IDType& registerNamespace(const IDType& parent_ns_id,
				  const std::string& name)
  {
    // Lookup
    NamespacePtr& parent = findNamespace(parent_ns_id);
    const IDType& child_id = hasChildNamespace(parent, name);

    if(child_id == IDType::null())
    {
      // Create entry
      IDType new_id = IDType::create();
      NamespacePtr ns = boost::make_shared<Namespace>(new_id, parent_ns_id, name);
      parent->accessChildNamespaceList().insert(new_id);
      NamespaceListType::iterator new_item = namespace_list_.insert(NamespaceListType::value_type(new_id, ns)).first;

      llog::log(llog::Severity::Debug, "Creating Namespace",
		"Tag", llog::Argument<std::string>(name));

      return new_item->first;
    }
    else
    {
      // Return existing id
      return child_id;
    }
  }

  const IDType& registerPseudoTag(const IDType& ns1_id,
				  const std::string& name1,
				  const IDType& ns2_id,
				  const std::string& name2)
  {
    NamespacePtr& ns1 = findNamespace(ns1_id);
    NamespacePtr& ns2 = findNamespace(ns2_id);

    //todo: manage duplicate tags

    IDType new_id = IDType::create();
    PseudoTagPtr tag = boost::make_shared<PseudoTag>(new_id,
						     ns1_id,
						     name1,
						     ns2_id,
						     name2);
    ns1->accessPseudoTagList().insert(new_id);
    ns2->accessPseudoTagList().insert(new_id);

    PseudoTagListType::iterator new_item =
      pseudo_tag_list_.insert(PseudoTagListType::value_type(new_id, tag)).first;

    llog::log(llog::Severity::Debug, "Creating Pseudotag",
	      "Tag1", llog::Argument<std::string>(name1),
	      "Tag2", llog::Argument<std::string>(name2));

    return new_item->first;
  }

  const IDType& registerTag(const IDType& parent_ns_id, const std::string& name)
  {
    return registerTag_(parent_ns_id, name)->getID();
  }

  TagPtr registerTag_(const IDType& parent_ns_id, const std::string& name)
  {
    NamespacePtr& parent = findNamespace(parent_ns_id);

    const IDType& tag_id = hasTag(parent, name);

    if(tag_id == IDType::null())
    {
      IDType new_id = IDType::create();
      TagPtr tag = boost::make_shared<Tag>(new_id, parent_ns_id, name);
      parent->accessTagList().insert(new_id);
      TagListType::iterator new_item = tag_list_.insert(TagListType::value_type(new_id, tag)).first;

      llog::log(llog::Severity::Debug, "Creating Tag",
		"Tag", llog::Argument<std::string>(name));

      return tag;
    }
    else
    {
      // Return existing id
      return findTag(tag_id);
    }
  }

  int countTags(const IDType& ns_id, const std::string& name)
  {
    NamespacePtr& ns = findNamespace(ns_id);

    std::set<IDType> tags_list;
    std::set<IDType> pseudo_tags_list;
    searchFromTagName(name, ns, tags_list, pseudo_tags_list);

    return tags_list.size();
  }

  void subscribeTag(const IDType& ns_id, const std::string& name, Tag::TagListenerType subscriber)
  {
    TagPtr tag = registerTag_(ns_id, name);
    tag->addSubscriber(subscriber);
    triggerNewTag(tag, findNamespace(ns_id));
  }

};

}

#pragma once

#include <map>
#include <set>
#include <string>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <darc/id.hpp>

#include <hns/tag_parser.hpp>
#include <hns/tag.hpp>
#include <hns/pseudo_tag.hpp>
#include <hns/namespace.hpp>

#include <beam/static_scope.hpp>
#include <darc/id_arg.hpp>

namespace hns
{

struct SearchResult
{
  IDListType tags;
  IDListType pseudo_tags;
};

class Tree : public beam::static_scope<beam::Trace>
{
protected:
  typedef darc::ID IDType;

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
			SearchResult& result)
  {
    slog<beam::Trace>(
      "searchPseudoTags",
      "In NS",   beam::arg<std::string>(ns->getName()),
      "For Tag", beam::arg<std::string>(tag_name));

    for(IDListType::const_iterator it = ns->accessPseudoTagList().begin();
	it != ns->accessPseudoTagList().end();
	it++)
    {
      PseudoTagPtr pseudo_tag = findPseudoTag(*it);
      const ID& tag_id = pseudo_tag->getID();
      if(result.pseudo_tags.find(tag_id) == result.pseudo_tags.end())
      {
	if (pseudo_tag->getNamespace1() == ns->getID() && pseudo_tag->getName1() == tag_name)
	{
	  slog<beam::Trace>(
	    "Matched 1st",
	    "N1", beam::arg<std::string>(pseudo_tag->getName1()),
	    "N2", beam::arg<std::string>(pseudo_tag->getName2()));

	  result.pseudo_tags.insert(tag_id);
	  searchFromTagName(pseudo_tag->getName2(),
			    findNamespace(pseudo_tag->getNamespace2()),
			    result);
	}
	else if (pseudo_tag->getNamespace2() == ns->getID() && pseudo_tag->getName2() == tag_name)
	{
	  slog<beam::Trace>(
	    "Matched 2nd",
	    "N1", beam::arg<std::string>(pseudo_tag->getName1()),
	    "N2", beam::arg<std::string>(pseudo_tag->getName2()));

	  result.pseudo_tags.insert(tag_id);
	  searchFromTagName(pseudo_tag->getName1(),
			    findNamespace(pseudo_tag->getNamespace1()),
			    result);
	}
      }
      else
      {
	slog<beam::Trace>(
	  "Skipping Pseudotag",
	  "N1", beam::arg<std::string>(pseudo_tag->getName1()),
	  "N2", beam::arg<std::string>(pseudo_tag->getName2()));
      }

    }
  }

// based on tag name
  void searchFromTagName(const std::string& tag_name,
			 const NamespacePtr ns,
			 SearchResult& result)
  {
    slog<beam::Trace>(
      "searchFromTagName",
      "In NS",   beam::arg<std::string>(ns->getName()),
      "For Tag", beam::arg<std::string>(tag_name));

    // Search for tags with the name
    for(IDListType::const_iterator it = ns->accessTagList().begin();
	it != ns->accessTagList().end();
	it++)
    {
      TagPtr child_tag = findTag(*it);
      if(child_tag->getName() == tag_name)
      {
	if(result.tags.find(*it) == result.tags.end())
	{
	  result.tags.insert(*it);
	}
	else
	{
	  return; // Namespace allready visited for this tag, just stop searching
	}
      }
    }
    searchPseudoTags(tag_name, ns, result);
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

  // ******************************
  // Callbacks
  // ******************************
  void triggerRemovedTag(const TagPtr& tag, const NamespacePtr& ns)
  {
    SearchResult result;
    searchFromTagName(tag->getName(), ns, result);
    result.tags.erase(tag->getID());

    for(std::set<IDType>::const_iterator it = result.tags.begin();
	it != result.tags.end();
	it++)
    {
      TagPtr alias_tag = findTag(*it);
      alias_tag->triggerRemovedAlias(tag->getID());
    }
  }

  // Finds all matching tags and triggers callback
  void triggerNewTag(const TagPtr& tag, const NamespacePtr& ns)
  {
    SearchResult result;
    searchFromTagName(tag->getName(), ns, result);
    result.tags.erase(tag->getID());

    for(std::set<IDType>::const_iterator it = result.tags.begin();
	it != result.tags.end();
	it++)
    {
      TagPtr alias_tag = findTag(*it);
      tag->triggerAddedAlias(alias_tag->getID());
      alias_tag->triggerAddedAlias(tag->getID());
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

      slog<beam::Debug>(
	"Creating Namespace",
	"Tag", beam::arg<std::string>(name));

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
    // Find namespace instances
    NamespacePtr& ns1 = findNamespace(ns1_id);
    NamespacePtr& ns2 = findNamespace(ns2_id);

    // Build trees before new tag is added
    SearchResult tree1;
    SearchResult tree2;
    searchFromTagName(name1, ns1, tree1);
    searchFromTagName(name2, ns2, tree2);

    // Callback
    if(tree2.tags.find(*tree1.tags.begin()) == tree2.tags.end())
    {
      for(IDListType::iterator it1 = tree1.tags.begin();
	  it1 != tree1.tags.end();
	  it1++)
      {
	TagPtr tag1 = findTag(*it1);
	for(IDListType::iterator it2 = tree2.tags.begin();
	    it2 != tree2.tags.end();
	    it2++)
	{
	  TagPtr tag2 = findTag(*it2);
	  tag1->triggerAddedAlias(tag2->getID());
	  tag2->triggerAddedAlias(tag1->getID());
	}
      }
    }

    // Create the pseudo tag
    IDType new_id = IDType::create();
    PseudoTagPtr tag = boost::make_shared<PseudoTag>(new_id,
						     ns1_id,
						     name1,
						     ns2_id,
						     name2);
    // Add to pseudo-tag list
    PseudoTagListType::iterator new_item =
      pseudo_tag_list_.insert(PseudoTagListType::value_type(new_id, tag)).first;

    // Add to both namespaces
    ns1->accessPseudoTagList().insert(new_id);
    ns2->accessPseudoTagList().insert(new_id);

    // Log
    slog<beam::Debug>(
      "Creating Pseudotag",
      "Tag1", beam::arg<std::string>(name1),
      "Tag2", beam::arg<std::string>(name2));

    return new_item->first;
  }

  TagPtr registerTag(const IDType& parent_ns_id, const std::string& name)
  {
    TagPtr tag = registerTag_(parent_ns_id, name);
    triggerNewTag(tag, findNamespace(parent_ns_id));
    return tag;
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

      slog<beam::Debug>(
	"Creating Tag",
	"Tag", beam::arg<std::string>(name));

      return tag;
    }
    else
    {
      // Return existing id
      return findTag(tag_id);
    }
  }

  // Flush Tag
  void flushTag(const IDType& tag_id)
  {
    TagPtr& tag = findTag(tag_id);
    if(tag.use_count() == 1)
    {
      NamespacePtr ns = findNamespace(tag->getNamespace());
      triggerRemovedTag(tag, findNamespace(tag->getNamespace()));
      tag_list_.erase(tag_id);
      ns->accessTagList().erase(tag_id);
    }
  }

  // Tester Function
  int countTags(const IDType& ns_id, const std::string& name)
  {
    NamespacePtr& ns = findNamespace(ns_id);

    SearchResult result;
    searchFromTagName(name, ns, result);

    return result.tags.size();
  }

};

}

#pragma once

#include <boost/tokenizer.hpp>

#include <stdexcept>

// move to external

namespace hns
{

struct InvalidTagException : public std::runtime_error
{
public:
  InvalidTagException(const std::string &err) : std::runtime_error(err)
  {
  }
};

//

class TagParser
{
  typedef boost::tokenizer<boost::char_separator<char> > TokenizerType;
  boost::char_separator<char> seperator_;

  TokenizerType tokenizer_;

public:
  typedef TokenizerType iterator;

  typedef std::vector<std::string> NamespaceListType;

  NamespaceListType namespaces_;
  std::string tag_;

public:
  TagParser(const std::string& full_tag) :
    seperator_("/", "", boost::keep_empty_tokens),
    tokenizer_(full_tag, seperator_)
  {
    for(TokenizerType::iterator it = tokenizer_.begin();
	it != tokenizer_.end();
	it++)
    {
      namespaces_.push_back(*it);
    }

    if(namespaces_.empty() ||
       namespaces_.back() == "")
    {
      throw InvalidTagException(full_tag);
    }

    tag_ = namespaces_.back();
    namespaces_.pop_back();

  }

  const NamespaceListType& getNamespaces() const
  {
    return namespaces_;
  }

  const std::string& getTag() const
  {
    return tag_;
  }

};

}

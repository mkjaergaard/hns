#pragma once

#include <hns/tag_parser.hpp>

namespace beam
{

template<>
std::string to_string(const arg<hns::TagParser>& tag_parser)
{
  std::ostringstream os;
  for(hns::TagParser::NamespaceListType::const_iterator it = tag_parser->getNamespaces().begin();
      it != tag_parser->getNamespaces().end();
      it++)
  {
    os << *it;
    os << " / ";
  }
  os << tag_parser->getTag();
  return os.str();
}

}

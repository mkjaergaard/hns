#pragma once

#include <hns/namespace.hpp>

namespace llog
{

template<>
std::string to_string(const Argument<hns::Namespace>& ns)
{
  std::ostringstream os;
  os << ns->getName() << " [" << ns->getID().short_string() << "] NS[";
  for(hns::Namespace::IDListType::const_iterator it = ns->accessChildNamespaceList().begin();
      it != ns->accessChildNamespaceList().end();
      it++)
  {
    os << ";" << it->short_string();;
  }

  return os.str();
}

}

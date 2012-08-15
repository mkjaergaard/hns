#pragma once

#include <hns/id.hpp>

namespace llog
{

template<>
std::string to_string(const Argument<hns::ID>& id)
{
  return id->short_string();
}

}

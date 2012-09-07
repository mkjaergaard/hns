#pragma once

#include <hns/id.hpp>

namespace hns
{

struct header
{
  const static uint32_t control = 0;
  const static uint32_t update = 1;

  ID dest_instance_id;
  ID src_instance_id;
  uint32_t payload_type;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & dest_instance_id;
    ar & src_instance_id;
    ar & payload_type;
  }

};

}

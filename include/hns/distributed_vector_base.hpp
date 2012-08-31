#pragma once

#include <vector>
#include <boost/function.hpp>
#include <boost/asio/buffer.hpp>
#include <hns/id.hpp>
#include <hns/shared_buffer.hpp>
#include <boost/asio/streambuf.hpp>
#include <hns/distributed_base.hpp>

#include <boost/uuid/uuid_serialize.hpp>

namespace hns
{

struct control
{
  const static uint32_t subscribe = 0;
  const static uint32_t unsubscribe = 1;
  const static uint32_t resend = 2;

  ID client_id;
  uint32_t command;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & client_id;
    ar & command;
  }
};

struct update
{
  const static uint32_t complete = 0;
  const static uint32_t partial = 1;

  ID server_id;
  uint32_t index;
  uint32_t type;
  uint32_t num_entries;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & server_id;
    ar & index;
    ar & type;
    ar & num_entries;
  }
};

class distributed_vector_base : public distributed_base
{
protected:
  uint32_t count_;

  server_id_type id_;

public:
  distributed_vector_base() :
    count_(0)
  {
  }

};

}

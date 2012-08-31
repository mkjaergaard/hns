#pragma once

#include <boost/shared_ptr.hpp>
#include <hns/distributed_id_types.hpp>
#include <hns/distributed_header.hpp>
#include <hns/shared_buffer.hpp>

namespace hns
{

class distributed_manager; // fwd

class distributed_base
{
protected:
  hns::ID id_;
  distributed_manager * manager_;

public:
  distributed_base() :
    manager_(0)
  {
  }

  virtual std::size_t recv(const header& hdr, hns::shared_buffer data) = 0;

  virtual ~distributed_base()
  {
  }

  hns::ID& id()
  {
    return id_;
  }

  void attach(distributed_manager * manager);

};

typedef boost::shared_ptr<distributed_base> distributed_base_ptr;

}


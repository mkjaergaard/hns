#pragma once

#include <boost/shared_ptr.hpp>
#include <hns/distributed_id_types.hpp>
#include <hns/distributed_header.hpp>
#include <hns/shared_buffer.hpp>
#include <hns/outbound_data.hpp>

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
    id_(ID::create()),
    manager_(0)
  {
  }

  virtual void recv(const header& hdr, hns::shared_buffer data) = 0;

  virtual ~distributed_base()
  {
  }

  hns::ID& id()
  {
    return id_;
  }

  void attach(distributed_manager * manager);

  void send_to_location(const ID& dest_location_id,
			const ID& dest_instance_id,
			const uint32_t payload_type,
			const outbound_data_base& data);

  void send_to_instance(const ID& dest_instance_id,
			const uint32_t payload_type,
			const outbound_data_base& data);

};

typedef boost::shared_ptr<distributed_base> distributed_base_ptr;

}


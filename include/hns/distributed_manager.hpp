#pragma once

#include <map>
#include <boost/function.hpp>
#include <hns/distributed_id_types.hpp>
#include <hns/shared_buffer.hpp>
#include <hns/outbound_data.hpp>

namespace hns
{

class distributed_base; // fwd

class distributed_manager
{
public:
  typedef boost::function<void(const ID& destination, hns::shared_buffer)> send_to_function_type;
  send_to_function_type send_to_function_;

protected:
  typedef std::map<ID, distributed_base*> list_type;
  list_type list_;

  typedef std::map<ID, ID> instance_location_map_type;
  instance_location_map_type instance_location_map;

public:
  distributed_manager(send_to_function_type send_to_function);
  void attach(distributed_base* entry);
  void detatch(distributed_base* entry);
  void recv(const location_id_type& remote_location, hns::shared_buffer data);

  void send_to_instance(const ID& src_instance_id,
			const ID& dest_instance_id,
			const uint32_t payload_type,
			const outbound_data_base& data);

  void send_to_location(const ID& src_instance_id,
			const ID& dest_location_id,
			const ID& dest_instance_id,
			const uint32_t payload_type,
			const outbound_data_base& data);


};

}

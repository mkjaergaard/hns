#include <hns/distributed_manager.hpp>
#include <hns/distributed_base.hpp>
#include <hns/distributed_header.hpp>

#include <boost/make_shared.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <hns/buffer.hpp>
#include <hns/const_buffer.hpp>

#include <hns/outbound_data.hpp>
#include <llog/logger.hpp>
#include <hns/id_arg.hpp>

namespace hns
{

distributed_manager::distributed_manager(send_to_function_type send_to_function) :
  send_to_function_(send_to_function)
{
}

void distributed_manager::attach(distributed_base* entry)
{
  list_.insert(list_type::value_type(entry->id(), entry));
}

void distributed_manager::detatch(distributed_base* entry)
{
  list_.erase(entry->id());
}

void distributed_manager::recv(const location_id_type& remote_location, hns::shared_buffer data)
{
  inbound_data<boost_serializer, header> i_hdr(data);

  instance_location_map_type::iterator item1 = instance_location_map.find(i_hdr.get().src_instance_id);
  if(item1 == instance_location_map.end())
  {
    instance_location_map.insert(
      instance_location_map_type::value_type(i_hdr.get().src_instance_id, remote_location));
  }

  list_type::iterator item = list_.find(i_hdr.get().dest_instance_id);
  if(item != list_.end())
  {
    llog::llog<llog::Severity::Trace>(
      "DistributedManager",
      "Data recv for", llog::Argument<ID>(i_hdr.get().dest_instance_id));
    item->second->recv(i_hdr.get(), data);
  }
  else
  {
    llog::llog<llog::Severity::Warning>(
      "DistributedManager: Data recv for unknown instance ID");
  }
}

void distributed_manager::send_to_instance(const ID& src_instance_id,
					   const ID& dest_instance_id,
					   const uint32_t payload_type,
					   const hns::outbound_data_base& data)
{
  instance_location_map_type::iterator item = instance_location_map.find(dest_instance_id);
  if(item != instance_location_map.end())
  {
    send_to_location(src_instance_id,
		     item->second,
		     dest_instance_id,
		     payload_type,
		     data);
  }
  else
  {
    llog::llog<llog::Severity::Warning>(
      "DistributedManager: Unknown node",
      "InstanceID", llog::Argument<ID>(dest_instance_id));
  }
}

void distributed_manager::send_to_location(const ID& src_instance_id,
					   const ID& dest_location_id,
					   const ID& dest_instance_id,
					   const uint32_t payload_type,
					   const hns::outbound_data_base& data)
{
  shared_buffer buffer = boost::make_shared<const_buffer>(2048);

  header hdr;
  hdr.src_instance_id = src_instance_id;
  hdr.dest_instance_id = dest_instance_id;
  hdr.payload_type = payload_type;

  outbound_data<boost_serializer, header> o_hdr(hdr);
  outbound_pair o_pair(o_hdr, data);

  o_pair.pack(buffer);

  send_to_function_(dest_location_id, buffer);
}

}

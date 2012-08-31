#include <hns/distributed_manager.hpp>
#include <hns/distributed_base.hpp>
#include <hns/distributed_header.hpp>

#include <boost/make_shared.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <hns/buffer.hpp>
#include <hns/const_buffer.hpp>

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
  std::istream is(data->streambuf());
  boost::archive::binary_iarchive archive(is);

  header hdr;
  archive >> hdr;

  list_type::iterator item = list_.find(hdr.instance_id);
  if(item != list_.end())
  {
    item->second->recv(hdr, data);
  }
  else
  {
    //log
  }
}

void distributed_manager::send_to(const ID& instance_id,
				  const uint32_t payload_type,
				  hns::shared_buffer data)
{
  instance_location_map_type::iterator item = instance_location_map.find(instance_id);
  if(item != instance_location_map.end())
  {
    const ID& destination_id = item->second;
    shared_buffer data2 = boost::make_shared<const_buffer>(256, data);

    header hdr;
    hdr.instance_id = instance_id;
    hdr.payload_type = payload_type;

    std::ostream os(data2->streambuf());
    boost::archive::binary_oarchive archive(os);

    archive << hdr;

    send_to_function_(destination_id, data2);
  }
  else
  {
    // log
  }
}


}

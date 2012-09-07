#include <hns/distributed_base.hpp>
#include <hns/distributed_manager.hpp>

namespace hns
{

void distributed_base::attach(distributed_manager * manager)
{
  manager_ = manager;
  manager_->attach(this);
}

void distributed_base::send_to_instance(const ID& dest_instance_id,
					const uint32_t payload_type,
					const outbound_data_base& data)
{
  manager_->send_to_instance(id_, dest_instance_id, payload_type, data);
}

void distributed_base::send_to_location(const ID& dest_location_id,
					const ID& dest_instance_id,
					const uint32_t payload_type,
					const outbound_data_base& data)
{
  manager_->send_to_location(id_, dest_location_id, dest_instance_id, payload_type, data);
}


}


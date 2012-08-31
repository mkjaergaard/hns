#include <hns/distributed_base.hpp>
#include <hns/distributed_manager.hpp>

namespace hns
{

void distributed_base::attach(distributed_manager * manager)
{
  manager_ = manager;
  manager_->attach(this);
}

}


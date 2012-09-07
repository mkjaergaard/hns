#include <hns/shared_buffer.hpp>
#include <hns/buffer.hpp>

namespace hns
{

void shared_buffer::flush()
{
  if((*this)->is_empty() && (*this)->next().get() != 0)
  {
    reset((*this)->next().get());
  }
}

}

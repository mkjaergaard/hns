#include <hns/raw_buffer.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <hns/distributed_vector.hpp>
#include <hns/distributed_manager.hpp>

#include <llog/logger.hpp>

#include <hns/id.hpp>
#include <boost/bind.hpp>

#include <boost/uuid/uuid_serialize.hpp>

void send_to_function(const hns::ID& destination, hns::shared_buffer)
{
  llog::llog<llog::Severity::Info>(
    "send_to_function");
}

int main(int argc, const char** argv)
{
  int val = 5;

  char data[512];
  hns::raw_buffer buf(data, 512);
  std::ostream os(buf.streambuf());
	std::cout << buf.free_size() << std::endl;
  boost::archive::binary_oarchive oa(os);
  std::cout << buf.free_size() << std::endl;

  for(unsigned int i = 0; i < 100; i++)
  {
    oa << val;
  }

  std::cout << buf.free_size() << std::endl;

  val = 0;

  hns::raw_buffer buf2(data, 512);
  std::istream os2(buf2.streambuf());
  boost::archive::binary_iarchive oa2(os2);

  oa2 >> val;

  std::cout << val << std::endl;

////
  hns::distributed_manager mngr(boost::bind(&send_to_function, _1, _2));
  hns::distributed_vector<int> my_vec;
}

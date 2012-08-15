
#include <iostream>
#include <boost/bind.hpp>
#include <hns/server_entity.hpp>

void callback(const hns::ID& tag_id, const hns::ID& alias_id, hns::TagEvent event)
{
  std::cout << "Callback" << std::endl;
}

int main()
{
  std::cout << "Running" << std::endl;

  hns::ServerEntity server;

  // Create tree of 6 tags in 4 different namespaces
  server.registerTag("ns1/t1_1");
  server.registerTag("ns1/ns1_1/t1_1_1");
  server.registerTag("ns1/ns1_1/t1_1_2");
  server.registerTag("ns1/ns1_2/t1_2_1");
  server.registerTag("ns2/ns2_1/t2_1_1");
  server.registerTag("ns2/ns2_2/t2_2_1");

  // Link t1_1_1 and t1_1_2 together through t1_1
  server.registerPseudoTag("ns1/t1_1", "ns1/ns1_1/t1_1_1");
  server.registerPseudoTag("ns1/t1_1", "ns1/ns1_1/t1_1_2");

  // Check (Should give 3)
  std::cout << server.countTags("ns1/ns1_1/t1_1_1") << std::endl;
  std::cout << server.countTags("ns1/ns1_1/t1_1_2") << std::endl;
  std::cout << server.countTags("ns1/t1_1") << std::endl;

  // Subscribe
  server.subscribeTag("ns1/t1_1", boost::bind(&callback, _1, _2, _3));
}

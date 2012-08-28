#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <gtest/gtest.h>
#include <hns/server_entity.hpp>
#include <hns/tag.hpp>

class EntityTest : public testing::Test
{
public:
  hns::ServerEntity server;

  typedef std::map<std::string, hns::TagHandle> HandleListType;
  HandleListType handles;

  typedef std::map<std::string, hns::ID> IDListType;
  IDListType id_list;

  struct CallbackEvent
  {
    hns::ID tag_id;
    hns::ID alias_id;
    hns::TagEvent event;

    CallbackEvent(hns::ID tag_id, hns::ID alias_id, hns::TagEvent event) :
      tag_id(tag_id),
      alias_id(alias_id),
      event(event)
    {
    }
  };

  typedef std::vector<CallbackEvent> CallbackEventListType;
  typedef boost::shared_ptr<CallbackEventListType> CallbackEventListTypePtr;

  typedef std::map<std::string, CallbackEventListTypePtr> CallbackListType;

  CallbackListType callbacks;

protected:
  virtual void SetUp()
  {
  }

  void tagCallback(std::string tag_name, hns::ID tag_id, hns::ID alias_id, hns::TagEvent event)
  {
    std::cout << tag_name << " " << event << std::endl;

    std::pair<CallbackListType::iterator, bool> result =
      callbacks.insert(CallbackListType::value_type(tag_name, CallbackEventListTypePtr()));

    CallbackListType::mapped_type& list = result.first->second;
    if(result.second) // instantiate the pointer if a new item was inserted
    {
      list.reset(new CallbackEventListType());
    }

    list->push_back(CallbackEvent(tag_id, alias_id, event));
  }

  bool checkNoCallback()
  {
    return callbacks.empty();
  }

  bool checkCallback(std::string tag_name, std::string alias_name, hns::TagEvent event)
  {
    // Find alias id
    IDListType::iterator alias_item = id_list.find(alias_name);
    if(alias_item == id_list.end())
    {
      return false;
    }

    const hns::ID& alias_id = alias_item->second;

    CallbackListType::iterator item = callbacks.find(tag_name);
    if(item != callbacks.end())
    {
      for(CallbackEventListType::iterator it = item->second->begin();
	  it != item->second->end();
	  it++)
      {
	if(it->alias_id == alias_id && it->event == event)
	{
	  item->second->erase(it);
	  if(item->second->empty())
	  {
	    callbacks.erase(item);
	  }
	  return true;
	}
      }
      return false;
    }
    else
    {
      return false;
    }
  }

  void addTag(const std::string& full_name)
  {
    hns::TagHandle handle = server.registerTag(full_name);
    handles.insert(HandleListType::value_type(full_name, handle));
    id_list.insert(IDListType::value_type(full_name, handle.id()));
  }

  void removeTag(const std::string& full_name)
  {
    handles.erase(full_name);
  }

  void subscribe(std::string full_name)
  {
    HandleListType::iterator item = handles.find(full_name);
    if(item != handles.end())
    {
      item->second.addSubscriber(boost::bind(&EntityTest::tagCallback,
					     this,
					     full_name,
					     _1,
					     _2,
					     _3));
    }
  }

  void addPseudoTag(const std::string& full_name1, const std::string& full_name2)
  {
    server.registerPseudoTag(full_name1, full_name2);
  }

  int countTag(const std::string& full_name)
  {
    return server.countTags(full_name);
  }

};

TEST_F(EntityTest, Tags)
{
  // Empty Tree
  EXPECT_EQ(countTag("ns1/t1_1"), 0);
  EXPECT_EQ(countTag("ns1/t1_2"), 0);

  // Add one tag
  addTag("ns1/t1_1");
  EXPECT_EQ(countTag("ns1/t1_1"), 1);
  EXPECT_EQ(countTag("ns1/t1_2"), 0);

  // Subscribe to the tag
  subscribe("ns1/t1_1");
  EXPECT_TRUE(checkNoCallback());

  // Add another independent tag
  addTag("ns1/t1_2");
  EXPECT_EQ(countTag("ns1/t1_1"), 1);
  EXPECT_EQ(countTag("ns1/t1_2"), 1);
  EXPECT_TRUE(checkNoCallback());

  // Add a link between the two tags
  addPseudoTag("ns1/t1_1", "ns1/t1_2");
  EXPECT_EQ(countTag("ns1/t1_1"), 2);
  EXPECT_EQ(countTag("ns1/t1_2"), 2);
  //EXPECT_TRUE(checkCallback("ns1/t1_1", "ns1/t1_2", hns::TagAdded));
  EXPECT_TRUE(checkNoCallback());

  // Add a link to new non-existing tag
  addPseudoTag("ns1/t1_1", "ns1/t1_3");
  EXPECT_EQ(countTag("ns1/t1_1"), 2);
  EXPECT_EQ(countTag("ns1/t1_3"), 2);

  // Add the tag
  addTag("ns1/t1_3");
  EXPECT_EQ(countTag("ns1/t1_1"), 3);
  EXPECT_EQ(countTag("ns1/t1_2"), 3);
  EXPECT_EQ(countTag("ns1/t1_3"), 3);
  EXPECT_TRUE(checkCallback("ns1/t1_1", "ns1/t1_3", hns::TagAdded));
  EXPECT_TRUE(checkNoCallback());

  // Remove the tag
  removeTag("ns1/t1_3");
  EXPECT_EQ(countTag("ns1/t1_1"), 2);
  EXPECT_EQ(countTag("ns1/t1_2"), 2);
  EXPECT_EQ(countTag("ns1/t1_3"), 2);
  EXPECT_TRUE(checkCallback("ns1/t1_1", "ns1/t1_3", hns::TagRemoved));
  EXPECT_TRUE(checkNoCallback());

}

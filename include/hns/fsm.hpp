#pragma once

#include <boost/static_assert.hpp>
#include <hns/id.hpp>
#include <llog/logger.hpp>

#define FSM_PREPARE_CLASS(c) \
friend class hns::fsm<c >;\
template<typename E> void post_event(const E&e) { fsm<c >::template post_event_(e); }

namespace hns
{

template<unsigned int State>
struct state
{
  const static unsigned int value = State;
};

template<typename Derived>
class fsm
{
public:

protected:
  //** Internale stuff
  unsigned int state_;

  fsm() :
    state_(0)
  {
  }

/*
  // States
  template<int State, typename Event>
  void handle(const Event& event)
  {
    llog::llog<llog::Severity::Warning>(
      "Unhandled Event",
      "State", llog::Argument<int>(State),
      "Event", llog::Argument<std::string>(""));
  }
*/

public:
  template<typename Event>
  void post_event(Event& event)
  {
    Derived* d_this = (Derived*)this;

    switch(state_)
    {
    case 0:
      d_this->handle(state<0>(), event);
      break;
    case 1:
      d_this->handle(state<1>(), event);
      break;
    case 2:
      d_this->handle(state<2>(), event);
      break;
    case 3:
      d_this->handle(state<3>(), event);
      break;
    case 4:
      d_this->handle(state<4>(), event);
      break;
    case 5:
      d_this->handle(state<5>(), event);
      break;
    default:
      assert(0);
      break;
    }
  }

  template<typename State>
  void trans(const State&)
  {
    llog::llog<llog::Severity::Debug>("trans");
    state_ = State::value;
    BOOST_STATIC_ASSERT((State::value <= 5));
  }

};

}

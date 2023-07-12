#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <future>

#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
  std::unique_lock<std::mutex> uLock(_mtx);
  _cond.wait(uLock,[this] {return !_queue.empty();});
  T last_element = std::move(_queue.back());
  _queue.pop_back();
  return last_element;
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
  std::lock_guard<std::mutex> lock(_mtx);
  _queue.emplace_back(std::move(msg));
  _cond.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _trafficLightQueue = std::make_shared<MessageQueue<TrafficLightPhase>>();
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
  while(true)
  {
     TrafficLightPhase check =  _trafficLightQueue->receive();
        
        // if receive a green TrafficLightPhase, then exit while loop
        if(check == TrafficLightPhase::green) break;
    
  }
    
    
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
  threads.emplace_back(std::thread(&TrafficLight::CycleThroughPhases,this));
}

// virtual function which is executed in a thread
void TrafficLight::CycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
  auto start= std::chrono::system_clock::now();
  int cycle_duration =  5;
   while(true)
    {    
      auto time_elapsed =  (std::chrono::system_clock::now()-start).count();
      if(time_elapsed >= cycle_duration)
      {
        auto start= std::chrono::system_clock::now();
                    std::lock_guard<std::mutex> lck(_mutex);
                if (_currentPhase ==TrafficLightPhase::green) 
        {
          _currentPhase = TrafficLightPhase::red;
        }
        else if (_currentPhase==TrafficLightPhase::red) {_currentPhase=TrafficLightPhase::green;}
      }
      TrafficLightPhase phase = _currentPhase;
      _trafficLightQueue->send(std::move(phase));     

    }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));

}


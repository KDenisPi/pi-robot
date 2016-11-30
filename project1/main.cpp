#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <memory>

#include "version.h"

/*
* Timer test
*/
#include "Timers.h"

using namespace std;

int main (int argc, char* argv[])
{
  cout <<  "Project1 started" << endl;

  //std::shared_ptr<smashine::Timer> timer(new smashine::Timer(5, 0, 100000));
  std::shared_ptr<smashine::Timer> timer(new smashine::Timer(3, 5));
  smashine::Timers* timers = new smashine::Timers();
  timers->start();
  sleep(1);
  timers->create_timer(timer);
  sleep(1);
  timers->reset_timer(timer->get_id());
  sleep(4);
  timers->cancel_timer(timer->get_id());
  //cout <<  "wait..." << endl;
  //sleep(5);
  timers->stop();

  cout <<  "Project1 finished" << endl;
  exit(0);
}


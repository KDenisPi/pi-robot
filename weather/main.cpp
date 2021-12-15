#include <unistd.h>
#include <stdlib.h>
#include <memory>

#include "version.h"
#include "defines.h"

#include "StateMachine.h"
#include "context.h"
#include "WeatherStFactory.h"
#include "WebSettingsWeather.h"
#include "pi-main.h"


using namespace std;

int main (int argc, char* argv[])
{
  std::shared_ptr<pimain::PiMain<weather::WeatherStFactory,weather::web::WebWeather>> pmain =
      std::make_shared<pimain::PiMain<weather::WeatherStFactory,weather::web::WebWeather>>("weather");

  pmain->load_configuration(argc, argv);
  pmain->run();

  exit(EXIT_SUCCESS);
}


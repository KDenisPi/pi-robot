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

using tm_type = pimain::PiMain<weather::WeatherStFactory,weather::Context, weather::web::WebWeather>;

int main (int argc, char* argv[])
{
  std::shared_ptr<tm_type> pmain = std::make_shared<tm_type>("weather");

  pmain->load_configuration(argc, argv);
  pmain->run();

  exit(EXIT_SUCCESS);
}


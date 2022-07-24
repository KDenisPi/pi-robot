#include <unistd.h>
#include <stdlib.h>
#include <memory>

#include "pi-main.h"
#include "defines.h"

#include "ststates.h"
#include "factory.h"


using namespace std;

int main (int argc, char* argv[])
{
  std::shared_ptr<pimain::PiMain<tmain::TMainStFactory,http::web::WebSettingsItf>> pmain =
      std::make_shared<pimain::PiMain<tmain::TMainStFactory,http::web::WebSettingsItf>>("tmain");

  pmain->load_configuration(argc, argv);
  pmain->run();

  exit(EXIT_SUCCESS);
}

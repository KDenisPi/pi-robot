#include <unistd.h>
#include <stdlib.h>
#include <memory>

#include "pi-main.h"
#include "defines.h"

#include "ststates.h"
#include "factory.h"


using namespace std;

using tm_type = pimain::PiMain<tmain::TMainStFactory,tmain::Context, http::web::WebSettingsItf>;

int main (int argc, char* argv[])
{
  std::shared_ptr<tm_type> pmain = std::make_shared<tm_type>("tmain");

  pmain->load_configuration(argc, argv);
  pmain->run();

  exit(EXIT_SUCCESS);
}

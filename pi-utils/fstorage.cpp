#include "fstorage.h"

namespace piutils {
namespace fstor {

thread_local std::list<std::string> FStorage::dfiles;

}
}
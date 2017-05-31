#include <stddef.h>
#include <stdint.h>
#include "appstore.hpp"
#include "apps.hpp"

void publish_all(){
	appstore_publish("free",&app_free);
	appstore_publish("hello",&app_hello);
}

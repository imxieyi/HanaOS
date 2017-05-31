#include <stddef.h>
#include <stdint.h>
#include "appstore.hpp"
#include "apps.hpp"

void publish_all(){
	appstore_publish("free",&app_free);
	appstore_publish("hello",&app_hello);
	appstore_publish("poweroff",&app_poweroff);
	appstore_publish("reboot",&app_reboot);
}

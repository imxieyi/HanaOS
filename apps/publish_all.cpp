#include <stddef.h>
#include <stdint.h>
#include "appstore.hpp"
#include "apps.hpp"

void publish_all(){
	appstore_publish("free",&app_free,false);
	appstore_publish("hello",&app_hello,false);
	appstore_publish("poweroff",&app_poweroff,false);
	appstore_publish("reboot",&app_reboot,false);
	appstore_publish("window",&app_window,true);
}

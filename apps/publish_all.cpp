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
	appstore_publish("tasklist",&app_tasklist,false);
	appstore_publish("nyancat",&app_nyancat,true);
	appstore_publish("crash1",&app_crash1,false);
	appstore_publish("crash2",&app_crash2,false);
	appstore_publish("crash3",&app_crash3,false);
	appstore_publish("time",&app_time,false);
	appstore_publish("help",&app_help,false);
    appstore_publish("binaryclock",&app_binaryclock,false);
}

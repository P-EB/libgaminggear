#include <libnotify/notify.h>
int main() {
	/* this wouldn't run, just needs to compile */
	notify_notification_new(NULL, NULL, NULL);
	return 0;
}

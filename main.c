#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

bool helpTriggered = false;

#ifdef NOTIFY

#include <libnotify/notify.h>

void notify(int percent){
	int pid = fork();

	if(pid == 0){
		// Execute as user who called, since binary is in root ownership
		uid_t uid = getuid();
		gid_t gid = getgid();
		seteuid(uid);
		setegid(gid);

		notify_init("backlight");
		char buf[10];
		sprintf(buf, "%d%%", percent);
		NotifyNotification* n = notify_notification_new("backlight",buf,NULL);
		notify_notification_set_timeout(n, 500);
		notify_notification_show(n, NULL);
		exit(0);
	}
}

#endif

int getMaxBrightness(){
	// This function is unsafe
	int fd = open("/sys/class/backlight/intel_backlight/max_brightness", O_RDONLY);
	char buf[10];
	ssize_t size = read(fd, buf, 10);
	return atoi(buf);
}

int getCurrentBrightness(){
	// This function is unsafe
	int fd = open("/sys/class/backlight/intel_backlight/brightness", O_RDONLY);
	char buf[10];
	ssize_t size = read(fd, buf, 10);
	return atoi(buf);
}

void setBrightness(int brightness){
	// This function is unsafe
	int fd = open("/sys/class/backlight/intel_backlight/brightness", O_WRONLY);
	char buf[10];
	snprintf(buf, 10, "%d", brightness);
	write(fd, buf, 10);
	return;
}

int transformToIntel(int percent){
	if(percent <= 0)
		percent = 1;
	if(percent > 100)
		percent = 100;
	return getMaxBrightness()*(percent/100.0f);
}

int transformToPercent(int intel){
	return ((float)intel/(float)getMaxBrightness())*100;
}

void changeBrightness(int percent){
	
	int brightnessPercent = transformToPercent( getCurrentBrightness() );
	
	brightnessPercent += percent;

	if(brightnessPercent <= 0)
		brightnessPercent = 1; // Never go under 1%
	if(brightnessPercent > 100)
		brightnessPercent = 100;

	int brightnessIntel = transformToIntel(brightnessPercent);
	setBrightness(brightnessIntel);
}

void printHelp(char* progName){
	helpTriggered = true;
	printf("Usage: %s [+-] or <value>\nWhere <value> is in percent\n", progName);
}

int main(int argc, char** argv){

	if(argc != 2){
		printHelp(argv[0]);
		return 0;
	}

	// Parse argument
	if(argv[1][0] == '+')
		changeBrightness(5);
	else if(argv[1][0] == '-')
		changeBrightness(-5);
	else if(argv[1][0] >= '0' && argv[1][0] <= '9')
		setBrightness(transformToIntel(atoi(argv[1])));
	else
		printHelp(argv[0]);

#ifdef NOTIFY
	if(!helpTriggered)
		notify( transformToPercent( getCurrentBrightness() ) );
#endif

	return 0;
}

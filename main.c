#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

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

	return 0;
}
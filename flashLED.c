#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define LED3_PATH "/sys/class/leds/beaglebone:green:usr3" 
void writeLED(char filename[], char value[]);
void removeTrigger();

int main(int argc, char* argv[]){
if(strcmp(argv[1],"on") == 0){
	printf("Turning the LED on");
	removeTrigger();
	writeLED("/brightness", "1");
}else if (strcmp(argv[1],"off") == 0){
	printf("Turning the LED off");
	removeTrigger();
	writeLED("/brightness", "0");
}else if (strcmp(argv[1],"flash") == 0){
	printf("Flashing");
	writeLED("/trigger", "timer");
	writeLED("/delay_on", "50");
	writeLED("/delay_off", "50");
}else if (strcmp(argv[1], "status") == 0){
	FILE* fp;
	char fullFileName[100];
	char line[80];
	sprintf(fullFileName, LED3_PATH, "/trigger");
	fp = fopen(fullFileName, "rt");
	while (fgets(line, 80, fp) != NULL){
		printf("%s", line);
	}
	fclose(fp);
}
else{
	printf("Invalid command!\n");
}
printf("Finished the makeLED Program\n");
return 0;
}	

void writeLED(char filename[], char value[]){
	FILE* fp;
	char fullFileName[100];
	sprintf(fullFileName, LED3_PATH "%s", filename);
	fp = fopen(fullFileName, "w+");
	fprintf(fp, "%s", value);
	fclose(fp);
}

void removeTrigger(){
	writeLED("/trigger", "none");
}
 

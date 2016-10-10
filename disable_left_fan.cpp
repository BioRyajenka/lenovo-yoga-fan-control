/*
================================================
        It autostarts from /etc/rc.local
================================================
*/

#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

#define FAN_SPEED_REG_LEFT 0xf3
#define FAN_SPEED_REG_RIGHT 0xf2

#define MIN_SPEED 0xff
#define MAX_SPEED 0x01

#define FAN_AUTO_MOD 0x0
#define READ_EC 0x80
#define WRITE_EC 0x81
#define EC_CMD 0x66
#define EC_DATA 0x62
#define CPU_TEMP_0 0x57
#define CRITICAL_TEMP 85
#define SLEEP_TIME 10000 // Wait until EC parse command in microseconds

unsigned char read_register(unsigned char address) {
    usleep(SLEEP_TIME);
    outb(READ_EC, EC_CMD);
    usleep(SLEEP_TIME);
    outb(address, EC_DATA);
    usleep(SLEEP_TIME);
    unsigned char data = inb(EC_DATA);
    usleep(SLEEP_TIME);

    return data;
}

unsigned char read_temp() {
    return read_register(CPU_TEMP_0);
}

void write_register(unsigned char value, unsigned char address) {
    usleep(SLEEP_TIME);
    outb(WRITE_EC, EC_CMD);
    usleep(SLEEP_TIME);
    outb(address, EC_DATA);
    usleep(SLEEP_TIME);
    outb(value, EC_DATA);
    usleep(SLEEP_TIME);
}

void default_mode() {
    printf("Setting default fan mode (with left fan disabled)\n");
    write_register(MIN_SPEED, FAN_SPEED_REG_LEFT);
    write_register(FAN_AUTO_MOD, FAN_SPEED_REG_RIGHT);
}

std::string time_to_string() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,80,"%d-%m-%Y %H:%M:%S",timeinfo);
    return std::string(buffer);
}

void say_hello() {
    std::ofstream myfile;
    myfile.open("/home/status.txt", std::ofstream::out | std::ofstream::app);
    myfile << "Started " << time_to_string() << "\n";
    myfile.close();
}

int main() {
    printf("Hello!\n");
    say_hello();

    if (ioperm(0x62, 5, 1)) {
        syslog(LOG_ERR, "IOperm error. Can't get access to ports 0x62 to 0x66");
        perror("IOperm error. Can't get access to ports 0x62 to 0x66");
        exit(EXIT_FAILURE);
    }
    default_mode();

    printf("Bye\n");
}

#include "rs232.h"

int cport_nr = 16;

int main() {
	int bdrate = 57600;
    
    char mode[] ={'8','N','1',0};
    
    if(RS232_OpenComport(cport_nr, bdrate, mode)) {
        printf("Can not open comport\n");
    }
    
    RS232_cputs(cport_nr, "A#1");
    RS232_cputs(cport_nr, "A#2");
    RS232_cputs(cport_nr, "A#3");
    RS232_cputs(cport_nr, "A#4");
    RS232_cputs(cport_nr, "A#5");
    RS232_cputs(cport_nr, "A#6");
    RS232_cputs(cport_nr, "A#7");
    RS232_cputs(cport_nr, "A#8");
    
    return 0;
}

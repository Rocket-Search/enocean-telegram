Lesen von EnOcean Telegrammen via "elemt14 EnOcean Pi 868" Modul


//Compile: clear; rm -frv enocean_telegrams; g++ -g -fPIC -Wall enocean_telegrams.cpp -o enocean_telegrams; date;


//Forked from: https://github.com/hfunke/EnOceanSpy/blob/master/EnOceanSpy.c


//Info: https://www.cmrr.umn.edu/~strupp/serial.html
// 		https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/


//manuel lesen:

//stty -F /dev/ttyAMA0 57600

//hexdump -C < /dev/ttyAMA0

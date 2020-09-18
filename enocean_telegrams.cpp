//Compile: clear; rm -frv enocean_telegrams; g++ -g -fPIC -Wall enocean_telegrams.cpp -o enocean_telegrams; date;
//Fork: https://github.com/hfunke/EnOceanSpy/blob/master/EnOceanSpy.c
//Info: https://www.cmrr.umn.edu/~strupp/serial.html
// 		https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/

//manuel lesen:
//stty -F /dev/ttyAMA0 57600
//hexdump -C < /dev/ttyAMA0

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>  //unistd.h — standard symbolic constants and types. 
#include <fcntl.h>   //fcntl.h — file control options . O_RDWR      Open for reading and writing.
#include <termios.h>  // The <termios.h> header shall contain the definitions used by the terminal I/O interfaces (see Chapter 11, General Terminal Interface for the structures and names defined).
//############################
#include <sstream>
#include <bits/stdc++.h> 
#include <iomanip>

using namespace std;

//Public File Descriptor für die Serielle Schnitstelle UART (/dev/ttyAMA0). Hardware elemt14 EnOcean PI 868 (auf einem Pi Zero WH v1.1 Revision 9000c1)
int serial_file_descriptor = -1;

//Signal Handler
void signalHandler( int signum)
{
        cout << "Interrupt signal (" << signum << ") received.\n";
        cout<<"PROBLEM EnOcean Telegram"<<endl;
		
		close(serial_file_descriptor);
		
        //exit (666);
        exit (- 1);
}

int main(int argc, char *argv[])
{

//####################################################################
//Error Signals

        signal(SIGSEGV, signalHandler);
        signal(SIGABRT, signalHandler);
        signal(SIGFPE, signalHandler);
        signal(SIGILL, signalHandler);
        signal(SIGTERM, signalHandler);
        signal(SIGINT, signalHandler);

//####################################################################

	//Serielle Schnittstelle öffnen
	serial_file_descriptor = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
	cout<<"serial_file_descriptor#"<<serial_file_descriptor<<endl;
	
	//The termios functions describe a general terminal interface that is provided to control asynchronous communications ports.
	//Dient zu Kontrolle der Serial Port Kommunikation
	struct termios SerialPortSettings;
	tcgetattr(serial_file_descriptor, &SerialPortSettings);

	//Serial Port Einstellungen (geeignet für elemt14 EnOcean PI 868)
	SerialPortSettings.c_cflag = B57600 | CS8 | CLOCAL | CREAD;			//In termios.h definiert. Baudrate von 57600 Bauds. CS8 8 bits per byte (most common) .  CLOCAL Ignore modem control lines.  CREAD  Enable receiver.
	SerialPortSettings.c_iflag = IGNPAR | ICRNL | PARENB;				//IGNPAR Ignore parity errors .  ICRNL  Translate carriage return to newline on input (unless IGNCR is set).    PARENB Enable parity generation on output and parity checking for input.
	SerialPortSettings.c_oflag = 0;										//tcflag_t c_oflag;      /* output modes */
	SerialPortSettings.c_lflag = 0;										//tcflag_t c_lflag;      /* local modes */

	tcflush(serial_file_descriptor, TCIFLUSH);							//TCFLSH	Flushes the input and/or output queue.
	tcsetattr(serial_file_descriptor, TCSANOW, &SerialPortSettings);	//Optionen sofort setzen 
	
	//Bei Problem beim Öffnen der Seriellen Schnittstelle, Program beenden.
	if (serial_file_descriptor == -1)
	{
		cout<<"Problem beim öffnen von /dev/ttyAMA0"<<endl;
		close(serial_file_descriptor);
		exit(-1);
	}
	
	//Empfangspuffer defnieren
	unsigned char empfangs_puffer[1024];
	//Größe des empfangenen Pakets definieren
	int empfangs_puffer_groesse  = -1;
	
	//Puffer als String definieren, besser zur späteren Verarbeitung
	string buffer_inhalt;
	string buffer_inhalt_temp ;
	//String Stream Converter von "unsigned char" nach "string"
	std::stringstream ConvertStream_1;

	//Endlosschleife zum herauslesen des Empfangspuffers
	while(1)
	{
	
		//Socket lesen ob Daten vorhanden sind und in die Variablen schreiben
		empfangs_puffer_groesse = read(serial_file_descriptor, empfangs_puffer, sizeof(empfangs_puffer));
		//cout<<"empfangs_puffer_groesse#"<<empfangs_puffer_groesse<<endl;
		
		if (empfangs_puffer_groesse < 0)
		{
			cout<<"Puffer leer"<<endl;
		}
		else if (empfangs_puffer_groesse == 0)
		{
			cout<<"Keine Daten"<<endl;
		}
		else
		{
			for ( int i = 0 ; i < empfangs_puffer_groesse; i++)
			{	
				//C-Style Ausgabe
				//printf("printf#, printf,%02X ", empfangs_puffer[i]);
							
				//C++ Aussgabe mit konvertierung nach HEX (0xAB)
				//std::cout <<"cout setfill#" << std::hex << std::setw(2)  << std::setfill('0') << (int)empfangs_puffer[i];
				
				//Einzelnes Character aus dem char Array auslesen und nach String kovertieren.
				ConvertStream_1<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)empfangs_puffer[i];
				ConvertStream_1>>buffer_inhalt_temp;
				buffer_inhalt.append(buffer_inhalt_temp);
				//sstream wieder leeren.
				ConvertStream_1.clear();
				
			}
		}
		//empanfspuffer variablen leeren.
		empfangs_puffer_groesse = -1 ;
		empfangs_puffer[1024] = '\0';

		//Telegram als HEX ausgeben
		cout<<"Telegram HEX#"<<buffer_inhalt<<"#"<<endl;
		buffer_inhalt.clear();
		
		//5 Sekunden schlafen bis zum nächsten durchlauf
		sleep(5); 
		
	}
    
	//Filedescriptor der Seriellen Schnittstelle schließen.
	close(serial_file_descriptor);
	
	return(0);
		
}



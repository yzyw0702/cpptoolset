#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)

#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

namespace toolchip {

#if defined(__linux__)

#elif defined(_WIN32)
// class SerialChip is modified from Serial.h and Serial.cpp originally released on website "playground of Arduino.cc"
	class SerialChip {
	private:
		HANDLE hSerial;
		COMSTAT comstat;

	public:
		bool isConnected;

	public:
		//Initialize Serial communication with the given COM port
		void connectTo(const string portName = "\\\\.\\COM5") {
			this->isConnected = false;
			this->hSerial = CreateFileA(portName.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

			if (this->hSerial == INVALID_HANDLE_VALUE) {
				if (GetLastError() == ERROR_FILE_NOT_FOUND) {
					printf("ERROR: Serial chip was not connected. Port %s is invalid.\n", portName.c_str());
				}
				else {
					printf("ERROR!!!");
				}
			}
			else {
				DCB dcbSerialParams = { 0 };
				if (!GetCommState(this->hSerial, &dcbSerialParams)) {
					printf("failed to retrieve serial chip parameters!");
				}
				else {
					dcbSerialParams.BaudRate = CBR_9600;
					dcbSerialParams.ByteSize = 8;
					dcbSerialParams.StopBits = ONESTOPBIT;
					dcbSerialParams.Parity = NOPARITY;
					dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

					if (!SetCommState(hSerial, &dcbSerialParams)) {
						printf("ALERT: Could not set Serial Port parameters");
					}
					else {
						this->isConnected = true;
						PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
						Sleep(2000);
					}
				}
			}
		}

		SerialChip() {

		}

		SerialChip(int idxCom) {
			char sIdx[64];
			sprintf(sIdx, "\\\\.\\COM%d", idxCom);
			connectTo(sIdx);
		}

		//Close the connection
		~SerialChip() {
			//Check if we are isConnected before trying to disconnect
			if (this->isConnected) {
				this->isConnected = false;
				CloseHandle(this->hSerial);
			}
		}

		//Read data in a buffer, if nbChar is greater than the
		//maximum number of bytes available, it will return only the
		//bytes available. The function return -1 when nothing could
		//be read, the number of bytes actually read.
		int receiveSignal(char *buffer, unsigned int nbChar) {
			DWORD bytesRead;
			unsigned int toRead;
			DWORD errors;
			ClearCommError(this->hSerial, &errors, &this->comstat);
			if (this->comstat.cbInQue>0) {
				if (this->comstat.cbInQue>nbChar) {
					toRead = nbChar;
				}
				else {
					toRead = this->comstat.cbInQue;
				}
				if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL)) {
					return bytesRead;
				}
			}
			return 0;
		}

		//Writes data from a buffer through the Serial connection
		//return true on success.
		bool sendSignal(const char *buffer, unsigned int nbChar) {
			DWORD bytesSend;
			DWORD errors;
			if (!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0)) {
				ClearCommError(this->hSerial, &errors, &this->comstat);
				return false;
			}
			else
				return true;
		}

		//simpler implementation of sendSignal
		bool sendSignal(const char cmd) {
			return sendSignal(&cmd, 1);
		}
	};

#endif

}

namespace debug_toolchip {
	using namespace toolchip;
	void test_chipwritedata(){
		//SerialChip ard("\\\\.\\COM5");
		SerialChip ard(5);
		if (!ard.isConnected) {
			std::cout << "arduino connection is invalid.\n";
		}
		else {
			cout << "arduino connection is ready.\n";
		}
		char buff = 13;
		if (!ard.sendSignal(buff)) {
			cout << "could not send data to arduino.\n";
		}

	}
}

#endif // SERIALCLASS_H_INCLUDED

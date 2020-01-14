#include "SerialClass.h"
#include <wchar.h>
#include <Windows.h>
#include <string>
#include <regex>
#pragma once
/* Abstraction class to control the 3D printer via UART
	Provides methods to display object scanning data
*/
#define OUTPUT_BUFFER_SIZE 1000
#define INPUT_BUFFER_SIZE 1000
using namespace std;
class Printer {
private:
	Serial* SP;
	char outputBuffer[OUTPUT_BUFFER_SIZE];
	char inputBuffer[INPUT_BUFFER_SIZE];
public:
	Printer(LPCWSTR printerPort);
	bool writeGcode(char* gcodeString);
	char* blockingRead(char* expectedResponse);
	void goToPosition(float x, float y, float z);
};

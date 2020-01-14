#include "Printer.h"

Printer::Printer(LPCWSTR printerPort)
{
	SP = new Serial(printerPort);
	if (SP->IsConnected())
	{
		wprintf(L"Connected to %ls\n", printerPort);		
		//Wait for Marlin to initilize
		Sleep(5000);
		//Junk the initial stuff Marlin sends 
		for (int i = 0; i < 2; i++)
		{
			SP->ReadData(inputBuffer, INPUT_BUFFER_SIZE);
			Sleep(500);
		}
		memset(inputBuffer,0,INPUT_BUFFER_SIZE);		
		printf("Homing Printer...\n");
		readGcodeResponse((char*)"G28\n", (char*)"endstops hit:",0);		
		printf("Homing Success.\n");
	}	
}

bool Printer::writeGcode(char* gcodeString)
{
	//Clear outputBuffer
	memset(outputBuffer, 0, OUTPUT_BUFFER_SIZE);
	//Copy gcodeString into buffer and send.
	strcpy_s(outputBuffer, OUTPUT_BUFFER_SIZE,gcodeString);	
	return SP->WriteData(outputBuffer, strlen(outputBuffer));
}

char* Printer::read(int timeout)
{
	int readCount = 0; 
	while(readCount < timeout)
	{
		//Clear the input buffer.
		memset(inputBuffer,0,INPUT_BUFFER_SIZE);
		SP->ReadData(inputBuffer,INPUT_BUFFER_SIZE);
		if(strlen(inputBuffer) > 0)//Data recieved; we are done.
		{
			return inputBuffer;
		}
		readCount++;
	}
	return (char*)-1;//Read timed out
}

char* Printer::readGcodeResponse(char* gcodeString, char* returnSubtring, int timeout)//if timeout is 0; gcode is only sent once.
{
	int readCount = 0;
	//Load the output buffer with the gcodeString
	memset(outputBuffer, 0, OUTPUT_BUFFER_SIZE);
	strcpy_s(outputBuffer, OUTPUT_BUFFER_SIZE, gcodeString);
	SP->WriteData(outputBuffer, strlen(outputBuffer));//Write the gcode data

	while (true)
	{	
		memset(inputBuffer, 0, INPUT_BUFFER_SIZE);
		SP->ReadData(inputBuffer, INPUT_BUFFER_SIZE);
		if (strlen(inputBuffer) > 0)
		{
			string responseData = string(inputBuffer);
			//Try to find the substring
			int substringIndex = responseData.find(returnSubtring);
			if (substringIndex >= 0)//found the substring
			{
				return &inputBuffer[substringIndex];
			}
		}
		if(readCount > timeout && timeout > 0)//Send it again and keep trying....
		{
			readCount = 0;
			memset(outputBuffer, 0, OUTPUT_BUFFER_SIZE);
			strcpy_s(outputBuffer, OUTPUT_BUFFER_SIZE, gcodeString);
			SP->WriteData(outputBuffer, strlen(outputBuffer));//Write the gcode data

		}
		readCount++;
		Sleep(1);
	}
}

void Printer::goToPosition(float x, float y, float z)
{
	while (true)
	{
		try
		{
			char output[50];
			memset(output, 0, 50);
			//Format the position gcode
			sprintf_s(output, "G0 X%f Y%f Z%f\n", x, y, z);
			readGcodeResponse(output, (char*)"ok",500);
			char* data = readGcodeResponse((char*)"M114\n", (char*)"X:",500);
			string response = string(data);
			std::regex r("[+-]?([0-9]*[.])?[0-9]+");
			smatch m;
			float X;
			float Y;
			float Z;
			std::regex_search(response, m, r);//Extract the current x,y,z positions from the printer.
			X = stof(m[0]);
			response = m.suffix();
			std::regex_search(response, m, r);
			Y = stof(m[0]);
			response = m.suffix();
			std::regex_search(response, m, r);
			printf("Printer at %s\n", output);
			Z = stof(m[0]);
			if (abs(X - x) < 0.001 && abs(Y - y) < 0.001 && abs(Z - z) < 0.001)//Verify position
			{
				return;
			}
		}
		catch (exception ex)
		{
			int i = 0; 
		}
	}
	
	
}

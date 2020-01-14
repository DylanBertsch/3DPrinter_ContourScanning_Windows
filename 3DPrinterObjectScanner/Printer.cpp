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
		writeGcode((char*)"G28\n");
		printf("Homing Printer...\n");
		blockingRead((char*)"endstops hit:");
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
	return -1;//Read timed out
}

char* Printer::blockingRead(char* expectedResponse)//Waits for a specific substring from the printer.
{	
	int readCount = 0;
	while (true)
	{
		//Clear buffer and read
		memset(outputBuffer, 0, OUTPUT_BUFFER_SIZE);
		SP->ReadData(inputBuffer, INPUT_BUFFER_SIZE);
		string input = string(inputBuffer);
		int substringPosition = input.find(expectedResponse);//Attempt to find the substring.
		if (substringPosition >= 0)
		{
			return inputBuffer;//Desired response recieved, stop blocking. 
		}
		if (readCount > 50)
		{
			SP->WriteData(outputBuffer, strlen(outputBuffer));
		}
		readCount++;
		Sleep(1);
	}
	return inputBuffer;
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
			writeGcode(output);
			blockingRead((char*)"ok");//Block until the printer has responded to the request.
			writeGcode((char*)"M114\n");//Verify the print head is in the desired position.
			string response = string(blockingRead((char*)"X:"));
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

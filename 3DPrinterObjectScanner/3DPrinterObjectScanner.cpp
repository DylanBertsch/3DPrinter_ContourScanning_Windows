// 3DPrinterObjectScanner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Printer.h"
#include <regex>
#include <vector>
#include <fstream>

using namespace std;

Printer printer = Printer(L"\\\\.\\COM3");
vector<float> outputDataX;
vector<float> outputDataY;
vector<float> outputDataZ;
void scanArea(int xpos, int ypos, int xLength, int yLength, float stepSize)
{
	outputDataX.clear();
	outputDataY.clear();
	outputDataZ.clear();
	for(float x = xpos; x <= xpos + xLength; x = x + stepSize)
	{
		for (float y = ypos; y <= ypos + yLength; y = y + stepSize)
		{			
			printer.goToPosition(x, y,5);//Go to point in mesh grid.
			char* data = printer.readGcodeResponse((char*)"G2001\n",(char*)"TOF:",500);
			string TOF_SUBSTRING = string(data);
			std::regex r("[+-]?([0-9]*[.])?[0-9]+");//Get a float value from the printer
			smatch m;			
			regex_search(TOF_SUBSTRING, m, r);
			float Z = stof(m.str(0));
			outputDataX.push_back(x);
			outputDataY.push_back(y);
			cout << Z << endl;
			if (Z < 51)//Objected Detected
			{
				outputDataZ.push_back(1);
				cout << 1 << endl;
			}
			else
			{
				outputDataZ.push_back(0);//No object detected
				cout << 0 << endl;
			}						
			
		}
	}
}

void writeCSV()
{
	ofstream csvFile("outputData.csv");	
	if (csvFile.is_open())
	{
		for (int index = 0; index < outputDataX.size(); index++)
		{
			float x = outputDataX[index];
			float y = outputDataY[index];
			float Z = outputDataZ[index];
			csvFile << x << "," << y << "," << Z << endl;
		}
		csvFile.close();
	}
}

int main()
{
	scanArea(125, 150, 35, 40, 0.5);
	writeCSV();
}

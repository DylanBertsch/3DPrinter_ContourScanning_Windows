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
			printer.writeGcode((char*)"G2001\n");//Get TOF Value
			Sleep(1);
			char* data = printer.blockingRead((char*)"TOF:");			
			std::regex r("[+-]?([0-9]*[.])?[0-9]+");//Get a float value from the printer
			smatch m;
			string str = string(data);
			regex_search(str, m, r);
			float Z = stof(m.str(0));
			outputDataX.push_back(x);
			outputDataY.push_back(y);
			if (Z < 50)//Objected Detected
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
	scanArea(125, 150, 60, 60, 0.5);
	writeCSV();
}

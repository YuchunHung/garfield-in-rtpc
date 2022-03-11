#include <iostream>
#include <fstream> 
#include <sstream>
#include <cstdlib>
#include <string>

#include "InfoRead.h"

using namespace std;

InfoRead :: InfoRead(const string inputFile) 
{
	
	fstream infoFile;

	// open the file 
	infoFile.open(inputFile, ios::in);
	if (!infoFile) {
		cout << "Can't open the information file! " << endl;
		exit(-1);
	}  // The check of opening the file. 

	// Load information
	LoadInformation(infoFile);

	//initialize the variable

} // InfoRead

void InfoRead :: LoadInformation(fstream& infoFile) 
{

	int countLine = 0;
	
	string str, tempStr;

	// read file
	  // Read the instruction information. 
	do {
		getline(infoFile, str);
		istringstream linestream(str);
		if (infoFile.eof()) {
			cout << "Please set the \"//info\" before the parameter setting." << endl;
			exit(-1); 
		}  
	} while(strcmp(str.c_str(), "//info"));

	countLine = 1;

	  // Read the parameter settings from *.inc
	do {
		getline(infoFile, str);
		istringstream linestream(str);
		string buffer;
		string buffer2;

		if (countLine != 4) {
			while (getline(linestream, buffer, ' ')) {
				if (strcmp(buffer.c_str(), "=") == 0) { 
					getline(linestream, buffer2, ' ');
				}
			}
		}

		int tempIdx = 0;
		// set the corrspendent varialbe. 
		switch (countLine) {
			case 1:
				Voltage_drift = buffer2;
				break;
			case 2:
				Voltage_trans = buffer2;
				break;
			case 3:
				B_field = buffer2;
				break;
			case 4:
				do {
					getline(linestream, buffer, ' ');
				} while (strcmp(buffer.c_str(), "=") != 0);
				while (getline(linestream, buffer, ':')) {
					istringstream linestream2(buffer);
					for (int i = tempIdx; i < (tempIdx + 2); i++) {
						getline(linestream2, buffer, '/');
						if (tempIdx == 0) {
							GasMix[i] = buffer;
						} 
						else if (tempIdx == 2) {
							buffer.pop_back();
							GasMix[i] = buffer;
						}
					} // i++
					tempIdx += 2;
				}
				break;
			case 5:
				Pressure = buffer2;
				break;
			case 6:
				Temperature = buffer2;
				break;
			case 7: 
				outputRootFile = buffer2;
				break;
			case 8: 
				SimRegion = buffer2;
				break;
			case 9: 
				tempStr = buffer2;
				break;
			default :
				break;

		}

		countLine++;
	} while (!infoFile.eof() || !strcmp(str.c_str(), "//end"));

	// assign the correct mesh files for either drift region or GEMs regions.
	  // Drift region ./RTPC 
	char lastChar;
	lastChar = tempStr[tempStr.length()-1];
	if (lastChar == '/')
		RTPCinputPath = tempStr;
	else 
		RTPCinputPath = tempStr + '/';
	  
	  // GEMs regions. ./bouns/trans*  
	if (lastChar == '/') {
		for (int i = 0; i< 3; i++) 
			GEMinputPath[i] = tempStr + "trans" + to_string(i + 1) + '/';
	}
	else {
		for (int i = 0; i < 3; i++) 
			GEMinputPath[i] = tempStr + "/trans" + to_string(i + 1) + '/';
	}
	

	infoFile.close();
	cout << "All the information are read." << endl;
} // LoadInformation

void InfoRead :: SetParameter(double& V_drift, double& V_trans, double& B, double& P, double& T, 
							  string GasComp[], double GasRatio[]) 
{
	string str;
	V_drift = stod(Voltage_drift);
	V_trans = stod(Voltage_trans);
	B = stod(B_field) + 0.0;
	P = stod(Pressure) + 0.0;
	T = stod(Temperature) + 273.15;
	GasMix[1].pop_back();
	GasComp[0] = "\"" + GasMix[0] + "\"";
	GasComp[1] = "\"" + GasMix[1] + "\"";
	GasRatio[0] = stod(GasMix[2]);
	GasRatio[1] = stod(GasMix[3]);

}  // SetParameter

void InfoRead :: SetInputFile(string& meshHeader, string& meshElement, string& meshNodes, 
							  string& dielectric, string& E_field, const int label)
{
	if (label == 0) {
		meshHeader = RTPCinputPath + "mesh.header";
		meshElement = RTPCinputPath + "mesh.elements";
		meshNodes = RTPCinputPath + "mesh.nodes";
		dielectric = RTPCinputPath + "dielectrics.dat";
		E_field = RTPCinputPath + "RTPC.result";

	} // RTPC Drift 
	else if (label == 1 || label == 2 || label == 3) {
		meshHeader = GEMinputPath[label - 1] + "mesh.header";
		meshElement = GEMinputPath[label - 1] + "mesh.elements";
		meshNodes = GEMinputPath[label - 1] + "mesh.nodes";
		dielectric = GEMinputPath[label - 1] + "dielectrics.dat";
		E_field = GEMinputPath[label - 1] + "trans" + to_string(label) + ".result";
		
	} // GEMs
	else {
		cout << "The label of the simulation regions was incorrect! Only 0(RTPC), 1(trans1), 2(trans2), 3(trans3) can be set as the parameter." << endl;
		exit(-1);
	}

/*	cout << "===============" << endl;
	cout << "header    : " << meshHeader << endl;
	cout << "element   : " << meshElement << endl;
	cout << "nodes     : " << meshNodes << endl;
	cout << "dlelectric: " << dielectric << endl;
	cout << ".result   : " << E_field << endl;
	cout << "===============" << endl;
*/
	// check the input file are assigned correctly.
	CheckElectricFieldFile(label, meshHeader, E_field);

}  // SetInputFile
		
void InfoRead :: CheckElectricFieldFile(const int label, string meshHeaderFile, string E_fieldFile)
{
	fstream headFile, electricFieldFile;
	string str;
	int count = 0;
	double voltageValue = 0.0, assignedVoltage = 0.0;

	if (label == 0) 
		assignedVoltage = stod(Voltage_drift);
	else  
		assignedVoltage = stod(Voltage_trans);
	
	cout << "CheckElectricFieldFile is working..." << endl;
	// open the file
	headFile.open(meshHeaderFile);
	if (!headFile) {
		cout << "Can't open the mesh.header in the folder." << endl;
		exit(-1);
	}  // Check the opening head file

	// Get the number of nodes from header file.
	getline(headFile, str);
	istringstream linestream(str);
	string buffer;
	getline(linestream, buffer, ' ');
	const int numOfNodes = stoi(buffer);
	headFile.close();

	// open the file
	electricFieldFile.open(E_fieldFile);
	if (!electricFieldFile) {
		cout << "Can't open the *.result in the folder." << endl;
		exit(-1);
	}  // Check the opening electric field file
	
	// Got to the line which record the electric potential values.
	while (count < (numOfNodes + 13)) {
		getline(electricFieldFile, str);
		count++;
	} // 13 lines for information of electric field file.
	getline(electricFieldFile, str);
	voltageValue = stod(str);

	// print out the error message if the voltage is not consistent with the inforamtion file. 
	cout << "Voltage in electric field file is : " << voltageValue << endl;
	cout << "Voltage setting in the info file : " << assignedVoltage << endl;
	if (abs(voltageValue - assignedVoltage) > 0.0001) {
		cout << "The *.result file may not consistent with the value in infomation file. Please double-check the files again!" << endl;
		exit(-1);
	}

	electricFieldFile.close();
}  // checkElectricFieldFile


void InfoRead :: PrintInformation() 
{
	// output the reading results.
	cout << "Drift voltage = " << Voltage_drift << endl;
	cout << "Transfer voltage = " << Voltage_trans << endl;
	cout << "Magnetic field = " << B_field << endl;
	cout << "Gas Mixture = " << GasMix[0] << "/" << GasMix[1] << ":" << GasMix[2] << "%/" << GasMix[3] << "%" << endl;
	cout << "Pressure = " << Pressure << endl;
	cout << "Temperature = " << Temperature << endl;
	cout << "output Root file = " << outputRootFile << endl;
	cout << "Simulation region = " << SimRegion << endl;
	if (SimRegion.compare("Drift") == 0) 
		cout << "RTPC input file path = " << RTPCinputPath << endl;
	else if (SimRegion.compare("GEMs") == 0) {
		for (int i = 0; i < 3; i++)  
			cout << "GEMs input file path" << (i + 1) << " = " << GEMinputPath[i] << endl;
	}
	else
		cout << "The tag of Simulation region is wrong. Either \"Drift\" or \"GEMs\" are valid." << endl;
} // PrintInformation


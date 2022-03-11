#ifndef INFOREAD_H
#define INFOREAD_H

#include <cstdlib>
#include <cstring>

// Read the information file and set up the parameter

using namespace std;

class InfoRead {
	public: 
		// Default Constructor
		InfoRead();
		// Constructor with an input file.
		InfoRead(const string inputFile);
		// Destructor
		//~InfoRead();

		// variable 
		string outputRootFile;

		// function
		void PrintInformation();
		void SetParameter(double& V_drift, double& V_trans, double& B_field, double& Pressure, double& Temperature, string GasComp[], double GasRatio[]);
		void SetInputFile(string& meshHeader, string& meshElement, string& meshNodes, string& dielectric, string& E_field, const int label);
		
		
	private:
		
		// variable
		string Voltage_drift;
		string Voltage_trans;
		string B_field;
		string GasMix[4];
		string Pressure;
		string Temperature;
		string SimRegion;
		string RTPCinputPath;
		string GEMinputPath[3];

		// function
		void LoadInformation(fstream& infoFile);
		void CheckElectricFieldFile(const int label, string meshHeaderFile, string E_fieldFile);

}; // infoRead class




#endif 

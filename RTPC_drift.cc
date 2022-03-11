//////////////////////////////////////////
//
// Simulation of the BONuS12 RTPC
//
// Nate Dzbenski
// ndzbe001@odu.edu
//
// Gabriel Charles
// gcharles@odu.edu
//
// 22 June 2020
//
// New construct code with latest Garfield++
// by Yu-Chun Hung
// Update date: Mar. 10. 2022 
//
///////////////////////////////////////////

// New RTPC in Garfield++
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
//#include <map>

#include "TApplication.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TDatime.h"

//#include "GeometryRoot.hh"
//#include "ViewGeometry.hh"
//#include "ComponentAnsys123.hh"
//#include "ViewField.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/AvalancheMC.hh"
#include "Garfield/Random.hh"
//#include "Plotting.hh"
//#include "FundamentalConstants.hh"
//#include "ComponentAnalyticField.hh"
//#include "ViewCell.hh"
//#include "SolidBox.hh"
//#include "SolidTube.hh"
//#include "GeometrySimple.hh"
//#include "TrackHeed.hh"
#include "Garfield/ComponentElmer.hh"
//#include "ViewFEMesh.hh"
//#include "ComponentVoxel.hh"

// User own library
#include "InfoRead.h"

using namespace std;
using namespace Garfield;

int main(int argc, char *argv[])
{
	TApplication app("app", &argc, argv);
    
    if ( !argv[1] ){
        cout << "You need to execute the program with the number of events to simulate, with: " << endl;
        cout << "./RTPC_drift [number of events]" << endl;
        cout << "Where [number of events] is the number of events you want to simulate." << endl;
        exit(EXIT_FAILURE);
    }
 
	// variables declaration
	clock_t startTime, stopTime; 
	TDatime executiveTime;
	const float B_max = 5.22;
	double B_field = 0.0;
	float B_scale = 0.0;
	
	double Voltage = 0.0;
	double V_GEMs = 0.0;
	double V_trans = 0.0;
	string gasComp[2]; 
	double gasRatio[2] = {0.0};
	string gasResult1[6];
	double gasResult2[6] = {0.0};
	double simTemperature = 0.0; // 293.15;
	double simPressure = 0.0; 	 // 753.;

	// Set the initial position [cm] and starting time [ns].
	double x0 = 0.0, y0 = 0.0, z0 = -19.0, t0 = 0.0;
	// Set the initial energy [eV].
	double e0 = 36.0;
	// Set the initial direction (x, y, z).
	// In case of a null vector, the direction is randomized.
	double dx0 = 0.0, dy0 = 0.0, dz0 = 0.0;
	// Calculate an electron avalanche
	int ne = 0, ni = 0;
	int ne_tot = 0;
	// Electron information after the avalanche
	Double_t x1 = 0.0, y1 = 0.0, z1 = 0.0, t1 = 0.0, e1 = 0.0;
  	Int_t status;

	double zeros[9] = {0.0};
    double the_rs[9] = {3.1, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 6.9};
    double the_zs[9] = {-19.0, -15.0, -10.0, -5.0, 0.0, 5.0, 10.0, 15.0, 19.0};
    
	string meshHeader;
	string meshElements;
	string meshNodes;
	string dielectrics;
	string elmerResult;

	
	startTime = clock();
	  // Load the parameter from input file.
	InfoRead info(app.Argv(2));
	  // Set the physics variables
	info.SetParameter(Voltage, V_trans, B_field, simPressure, simTemperature, gasComp, gasRatio);
	B_scale = B_field/B_max;
	  // Set the electric field input files obtained from Elmer
	info.SetInputFile(meshHeader, meshElements, meshNodes, dielectrics, elmerResult, 0);

	  // Set the output Root file path
	TString outputRootFile = info.outputRootFile;

	//cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;
	//info.PrintInformation();
	//cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;


	// Setup the gas.
	MediumMagboltz *gas = new MediumMagboltz();
	//  gas->LoadGasFile("gasFiles/He_100_DME_00.gas");
	string gasComp1 = "He";
	string gasComp2 = "CO2";
	gas->SetComposition(gasComp1, gasRatio[0], gasComp2, gasRatio[1]);
	gas->SetTemperature(simTemperature);
	gas->SetPressure(simPressure);
	gas->EnableDrift();                           // Allow for drifting in this medium
	//gas->PrintGas();
	

	ComponentElmer * elm = new ComponentElmer(meshHeader, meshElements, meshNodes, dielectrics, elmerResult, "cm");
	elm -> SetMedium(0, gas);
	// max B-field is 5.22 T
	//elm->LoadMagneticField("Fieldmaps/solenoid_map_may2019.dat", B_scale);
	// GOAL : Load the magneticField !!!!!!!!! //
	//
	//
	//
	//
	//
	//
	//
	//
	// GOAL : Load the magneticField !!!!!!!!! //


	// Assemble a Sensor object 
	Sensor *sensor = new Sensor(); 
	sensor -> SetArea(-7.0, -7.0, -20.0, 7.0, 7.0, 20.0);
	// Calculate the electric field using the Component object cmp
	sensor -> AddComponent(elm);
	//sensor->AddComponent(bfield);

	TCanvas *c_e = new TCanvas("Cell","Cell");
	ViewDrift *v_e = new ViewDrift();
	v_e->SetCanvas(c_e);
	v_e->SetArea(-7.0, -7.0, -20.0, 7.0, 7.0, 20.0);
  

	// Evaluate the number of electrons in the avalanche
	AvalancheMicroscopic *aval = new AvalancheMicroscopic(); // did not get it to work with AvalancheMC()
	//AvalancheMC *aval = new AvalancheMC();
	aval -> SetSensor(sensor);
	aval -> EnablePlotting(v_e);
	//aval -> EnableMagneticField();
    

    // begin the data creation
    //for(int z_i=0; z_i < 9; z_i++) {
    for(int z_i = 4; z_i < 5; z_i++) {
    	z0 = the_zs[z_i];
        cout << z0 << endl;;
        for(int r_i = 0; r_i < 9; r_i++) {
            // Set the initial position [cm] and starting time [ns].
            x0 = the_rs[r_i];
            y0 = 0.0;
            t0 = 0.0;
			x1 = y1 = z1 = t1 = 0.0;
            
            cout << "z = " << z0 << ", r = " << x0 << endl;
            
            //sprintf(tvec_name,"bTd_%i_%i",z_i,r_i);
            //sprintf(phivec_name,"bPhid_%i_%i",z_i,r_i);
            //sprintf(xvec_name,"bXd_%i_%i",z_i,r_i);
            //sprintf(yvec_name,"bYd_%i_%i",z_i,r_i);
            
            // create a branch for each vector
            //tTree->Branch(tvec_name, &drift_times[z_i][r_i]);
            //pTree->Branch(phivec_name, &drift_angles[z_i][r_i]);
            //xTree->Branch(xvec_name, &drift_x[z_i][r_i]);
            //yTree->Branch(yvec_name, &drift_y[z_i][r_i]);
            
            for(int eve = 0;eve < atoi(argv[1]); eve++) {
                ne_tot = 0;
                cout << "Event number: " << eve << " e0 = " << e0 << endl;
                //aval -> AvalancheElectron(x0, y0, z0, t0, e0, dx0, dy0, dz0);
                aval -> DriftElectron(x0, y0, z0, t0, e0, dx0, dy0, dz0);
                
                // Get the number of electrons and ions in the avalanche.
                aval -> GetAvalancheSize(ne, ni);
                ne_tot += ne;
                
                if(0 < aval->GetNumberOfElectronEndpoints()) {
                    for(int nava = 0;nava < aval->GetNumberOfElectronEndpoints(); nava++) {
                        aval -> GetElectronEndpoint(nava, x0, y0, z0, t0, e0, x1, y1, z1, t1, e1, status);
                  		cout << "At t = " << t1 << " x: " << x0 << "->" << x1 << " y: " << y0 << "->" << y1 << " e: " << e0 << "->" << e1 << endl;		
                        //drift_times[z_i][r_i].push_back(t1);
                        //drift_x[z_i][r_i].push_back(x1);
                        //drift_y[z_i][r_i].push_back(y1);
                        //if(x1 != 0)drift_angles[z_i][r_i].push_back(TMath::ATan2(y1,x1));
                    }  // nava++
				}
            }  // eve++
        }  // r_i++
    }  // z_i++
    //tTree->Fill();
    //pTree->Fill();
    //xTree->Fill();
    //yTree->Fill();
    
	
	
 	stopTime = clock();	
	executiveTime.Set();
  	gas -> GetComposition(gasResult1[0], gasResult2[0], gasResult1[1], gasResult2[1], 
						  gasResult1[2], gasResult2[2], gasResult1[3], gasResult2[3],
						  gasResult1[4], gasResult2[4], gasResult1[5], gasResult2[5]); // 2~5 are dummy
 	cout << endl;
	cout << "==================================" << endl;
	cout << "The information of parameter setting in Garfield simulation : " << endl;
	cout << "Voltage : " << Voltage << " V." << endl;
	//cout << "GEMs voltage : " << V_GEMs << " V. This term needs to be corrected. " << endl;
	cout << "Transfer voltage : " << V_trans << " V." << endl;
	cout << "Magnetic field magnitude : " << B_field << " T" << endl;
	cout << "Gas mixture : " << gasResult1[0] << "/" << gasResult1[1] << " = " 
							 << gasResult2[0]*100. << "/" << gasResult2[1]*100. << endl;
	cout << "Temperature : " << gas -> GetTemperature() << " K  (" << (gas -> GetTemperature() - 273.15) << "\u00b0C)" << endl;;
	cout << "Pressure : " << gas -> GetPressure() << " Torr" << endl;
	cout << "Parameter setting file : " << app.Argv(2) << endl;
	cout << "Output file : " << outputRootFile << endl;
	cout << "Running time : " << double(stopTime - startTime)/CLOCKS_PER_SEC << " sec" << endl;
	cout << "Simulation date : ";
	executiveTime.Print();
	cout << "==================================" << endl;
    
	cout << "Done! " << endl;
	return 0;

} // main

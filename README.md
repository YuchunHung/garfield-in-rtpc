# garfield-in-rtpc

> A temporary note about executing the garfield++ in Nate's repository https://github.com/ndzbenski/garfield  
> Follow the documetation in Nate's web page: https://userweb.jlab.org/~nathand/rgf/sim/garfieldpp.html

RTPC.geo  -> The setting file of the geometry of the RTPC detector. Also, it can help us to create the grids based on this seetting. 
If you modify the RTPC.geo, you will need to re-compile it to get the new grids information. 
<p> gmsh RTPC.geo -3 -order 2 <p>
The output file will be RTPC.msh

Next, you will need the information of the electric field on these nodes. 
First, 
<p> ElmerGrid 14 2 RTPC.msh -autoclean <p>
  
The output files are RTPC/mesh.boundary, RTPC/mesh.elements, RTPC/mesh.header, RTPC/mesh.names, and RTPC/mesh.nodes 
RTPC/mesh.boundary, RTPC/mesh.elements, RTPC/mesh.header, and RTPC/mesh.nodes are the input files in garfield.
Next, 

 <p> ElmerSolver RTPC.sif <p> 
to get the information of the electric field of the nodes. 
RTPC.sif  -> Define the physics to apply to the mesh. The boundary conditions (values of potentials) are implemented there.
The output files are RTPC/RTPC.result, and RTPC/RTPC.ep. RTPC.result is an input file in the garfield. 
  
RTPC.C -> Main file of the simulation. 

makefile -> A compile file to get garf_RTPC

Change the electric potential (boundary) -> change the setting in RTPC.sif
  ! Cathode
 Boundary Condition 1
 Target Boundaries = 1
 Potential = -4400
 End
 
Change the magnitude of magnetic field -> Change 
    elm -> LoadMagneticField("Fieldmaps/solenoid_map_may2019.dat", 0.778); 
  
  
> Run the simulation 
<p> ./garf_RTPC [number of events] <p> 
  where [number of events] is the number of events you want to simulate.
  
  The output file is rtpc_out.root
  
  > Analyze the rtpc_out.root
     Run analysis.c in ROOT. 
     
  

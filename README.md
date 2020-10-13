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
  
  

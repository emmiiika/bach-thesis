# bach-thesis

This directory contains programs to generate and analyze minimal uncolorable 
subgaphs of given graphs (cubic with girt $>= 5$, snarks with girth $>= 5$, 
and snarks with girth $>= 4$) All files are in the directory ```code```. 

For the generation algorithm and statistic files to be generated one only has 
to run ```main```. Before running the code, set what input sets the code will 
run on in ```main```. 

Compile the programs with ```-std=c++17 -fconcepts -I"../ba-graph/include" 
-I"../nauty2_8_8" -L ../nauty2_8_8/ -l:nauty.a```.

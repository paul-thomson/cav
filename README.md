CAV Assignment - MANMADILLO
===

#Workplan
1. (DONE) Read skeleton.out and draw bones (to check if correct)
2. Use matrix transformations to rotate bones (using keyframe)
3. Read attachment.out which contains weights for each mesh vertex:

Each line represents a vertex (by index). Each line contains the weights for all the bones (arranged by index).

4. Use the weights to "attach" them to the bones. Then also apply transformations to the mesh vertices
5. Done?

#Compile and run
./run.sh






===================================================
compiling the demo program:

g++ -o view view.cc matrix4f.cc -lglut -lGLU -lGL

running the program

./view arma2.obj   


about  skeleton.out  and attachment.out 
	
Each line in skeleton.out corresponds to a joint and is of the form:
idx x y z prev where prev is the index of the previous joint.

The skeletons are composed of 21 bones (22 joints).
The ID of the bone is same as the ID of the joint in skeleton2.out. 
In attachment2.out, the weights for each bone is listed.    

Each line in attachment.out corresponds to a mesh vertex and
consists of bone weights for each bone in the order of bone IDs.

**** amendments ****
matrix4f.cc, matrix4f.h include the classes / methods for computing
the rotation and translation matrix. They include the vector3f and
matrix4f classes.

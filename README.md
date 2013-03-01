CAV Assignment - MANMADILLO
===

#What has been done:
1) Read weights and skeleton files, storing the information in vectors.
2) Calculate transformation matrices using the bone translations and rotations.
3) Apply these transformations before drawing the mesh.
4) Allow keyframe animations to be hardcoded (see function createKeyframes). These keyframes also allow translating the entire mesh, not just rotating. These animations are stored in the global q vector, with the last space of each keyframe being reserved for the translating.
5) Linear interpolation is provided by the idle function myIdleLinearInterpolate

##Extra stuff
6) Bone calculation and drawing is provided as it was used primarily for debugging. This can be switched on/off using the boolean statement at the beginning of the myDisplay function.
7) Quadratic bezier and cubic bezier keyframe interpolation is also provided. These currently automatically generate the extra control points from the keyframes provided in the code. It generates these such that movements will be smoothed in some way (i.e. speed will not be constant between keyframes). It would be possible for the user to specify the control points themselves but I did not provide any examples of this.
To use different keyframe interpolation change which function is set as the idle function (near the end of main method).

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

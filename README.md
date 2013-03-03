CAV Assignment - MANMADILLO
===

#What has been done:
1) Read weights and skeleton files, storing the information in vectors.

2) Calculate transformation matrices using the bone translations and rotations.

3) Apply these transformations before drawing the mesh.

4) Allow keyframe animations to be hardcoded (see function createKeyframes). These keyframes also allow translating the entire mesh, not just rotating. These animations are stored in the global q vector, with the last space of each keyframe being reserved for the translating.

5) Linear interpolation is provided by the idle function myIdleLinearInterpolate

6) Short jumping animation has been added - not much time has been spent trying get the correct rotations so it looks a bit warped in some places.

##Extra stuff

7) Bone calculation and drawing is provided as it was used primarily for debugging. This can be switched on/off using the boolean statement at the beginning of the myDisplay function.

8) Quadratic bezier and cubic bezier keyframe interpolation is also provided. These currently automatically generate the extra control points from the keyframes provided in the code. It generates these such that movements will be smoothed in some way (i.e. speed will not be constant between keyframes). It would be possible for the user to specify the control points themselves but I did not provide any examples of this. Only linear interpolation can be used for translation, or the body will move in expected ways.
To use different keyframe interpolation change which function is set as the idle function (near the end of main method).

#How to run

##Eclipse
Development has been done in Eclipse and project settings have been used on multiple machines so importing into Eclipse should work, but other instructions are given in case this is not the case.

##Terminal
Compile:

g++ -o view view.cc matrix4f.cc -lglut -lGLU -lGL

Run:

./view arma2.obj

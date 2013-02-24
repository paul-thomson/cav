#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <map>
#include <set>
#include "view.h"




GLdouble bodyWidth = 1.0;

GLfloat angle = -150;   /* in degrees */
GLfloat xloc = 0, yloc = 0, zloc = 0;
int moving, begin;
int newModel = 1;


void printMatrix4f(Matrix4f m) {
	for (int i = 0; i < 4; i++) {
		cout << m(i,0) << ", " << m(i,1) << ", " << m(i,2) <<  ", " << m(i,3) << "\n";
	}
}

void printVector3f(Vector3f v) {
	cout << v[0] << ", " << v[1] << ", " << v[2] << "\n";
}


/* ARGSUSED3 */
void
mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		moving = 1;
		begin = x;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		moving = 0;
	}
}

/* ARGSUSED1 */
void motion(int x, int y)
{
	if (moving) {
		angle = angle + (x - begin);
		begin = x;
		newModel = 1;
		glutPostRedisplay();
	}
}

void
tablet(int x, int y)
{
	xloc = ((GLfloat) x) / 500 - 4;
	yloc = ((GLfloat) y) / 1000 - 2;
	newModel = 1;
	glutPostRedisplay();
}

int xt = 1, yt = 1, zt = 1, xr = 1;

void
translate(int x, int y, int z)
{
	GLfloat newz;

	if (xt)
		xloc += ((GLfloat) x) / 100;
	if (yt)
		yloc += ((GLfloat) y) / 100;
	if (zt) {
		newz = zloc - ((GLfloat) z) / 100;
		if (newz > -60.0 && newz < 13.0)
			zloc = newz;
	}
	newModel = 1;
	glutPostRedisplay();
}

/* ARGSUSED1 */
void
rotate(int x, int y, int z)
{
	if (xr) {
		angle += x / 2.0;
		newModel = 1;
		glutPostRedisplay();
	}
}

void
button(int button, int state)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case 1:
			xt = yt = zt = xr = 1;
			break;
		case 5:
			xt = 1;
			yt = zt = xr = 0;
			break;
		case 6:
			yt = 1;
			xt = zt = xr = 0;
			break;
		case 7:
			zt = 1;
			xt = yt = xr = 0;
			break;
		case 8:
			xr = 1;
			xt = yt = zt = 0;
			break;
		case 9:
			xloc = yloc = zloc = 0;
			newModel = 1;
			glutPostRedisplay();
			break;
		}
	}
}




int nRows = 480;
int nCols = 480; 

GLfloat light_ambient[] = {0.5, 0.5, 0.5, 1.0};  /* Red diffuse light. */
GLfloat light_diffuse[] = {0.8, 0.8, 0.8, 1.0};  /* Red diffuse light. */
GLfloat light_specular[] = {0.8, 0.8, 0.8, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {0.0, 0.0, 1.0, 0.0};  /* Infinite light location. */

static float modelAmb[4] = {0.2, 0.2, 0.2, 1.0};
static float matAmb[4] = {0.2, 0.2, 0.2, 1.0};
static float matDiff[4] = {0.8, 0.8, 0.8, 1.0};
static float matSpec[4] = {0.4, 0.4, 0.4, 1.0};
static float matEmission[4] = {0.0, 0.0, 0.0, 1.0};

static float modelAmb2[4] = {0.5, 0.5, 0.5, 1.0};
static float matAmb2[4] = {0.5, 0.5, 0.5, 1.0};
static float matDiff2[4] = {0.8, 0., 0., 1.0};
static float matSpec2[4] = {0.4, 0., 0., 1.0};
static float matEmission2[4] = {0.0, 0.0, 0.0, 1.0};



TriangleMesh trig;
vector< vector <float> > bones;
vector< Matrix4f > boneRotations;
vector< vector <float> > vertexWeights;
GLUquadricObj *qobj;



/*
bool contain(Edge & e, map < pair <int, int> , Edge > & list) 
{

	pair <int, int> key;

	key.first = e.v1;
	key.second = e.v2;

	if (list.find(key) == list.end()) return false;
	else return true;
}
 */

bool contain(Edge & e, vector < Edge > & list) 
{
	bool inlist = false;

	for (int i = 0; i < list.size(); i++) 
	{
		if ((list[i]._v1 == e._v1 && list[i]._v2 == e._v2) ||
				(list[i]._v2 == e._v1 && list[i]._v1 == e._v2))
		{
			return true;
		}	
	}

	return false;
}

int edgeID(Edge & e, vector < Edge > & list) 
{
	bool inlist = false;

	for (int i = 0; i < list.size(); i++) 
	{
		if ((list[i]._v1 == e._v1 && list[i]._v2 == e._v2) ||
				(list[i]._v2 == e._v1 && list[i]._v1 == e._v2))
		{
			return i;
		}	
	}

	return -1;
}




/*
int edgeID(Edge & e, map < pair <int, int> , Edge > & list) 
{
	pair <int, int> key;

	key.first = e.v1;
	key.second = e.v2;

	if (list.find(key) == list.end()) return -1;
	else return list[key].id();   
}
 */



int find(Edge & e, vector <Edge> list) 
{
	for (int i = 0; i < list.size(); i++) {
		if (list[i] == e) return i;
	}

	return -1;
}


void TriangleMesh::loadFile(char * filename)
{
	ifstream f(filename);


	if (f == NULL) {
		cerr << "failed reading polygon data file " << filename << endl;
		exit(1);
	}

	char buf[1024];
	char header[100];
	float x,y,z;
	float xmax,ymax,zmax,xmin,ymin,zmin;
	int v1, v2, v3, n1, n2, n3;

	xmax =-10000; ymax =-10000; zmax =-10000;
	xmin =10000; ymin =10000; zmin =10000;

	while (!f.eof()) {
		f.getline(buf, sizeof(buf));
		sscanf(buf, "%s", header);

		if (strcmp(header, "v") == 0) {
			sscanf(buf, "%s %f %f %f", header, &x, &y, &z);  
			_v.push_back(Vector3f(x,y,z));

			_vn.push_back(Vector3f(0.f,0.f,1.f));

			Node node;

			node._id = _v.size()-1; 

			_node.push_back(node);


			if (x > xmax) xmax = x;
			if (y > ymax) ymax = y;
			if (z > zmax) zmax = z;

			if (x < xmin) xmin = x;
			if (y < ymin) ymin = y;
			if (z < zmin) zmin = z;
		}
		else if (strcmp(header, "vn") == 0) {
			//	sscanf(buf, "%s %f %f %f", header, &x, &y, &z);
			//	_vn.push_back(Vector3f(x,y,z));
		}
		else if (strcmp(header, "f") == 0)
		{
			//	sscanf(buf, "%s %d//%d %d//%d %d//%d", header, &v1, &n1,
			//		&v2, &n2, &v3, &n3);


			sscanf(buf, "%s %d %d %d", header, &v1, &v2, &v3);


			Triangle trig(v1-1, v2-1, v3-1, v1-1, v2-1, v3-1);
			trig._id = _trig.size(); 
			_trig.push_back(trig);

			Edge e1(v1-1, v2-1);
			Edge e2(v2-1, v3-1);
			Edge e3(v3-1, v1-1);

			/*
			pair <int, int> id10(v1-1,v2-1),id11(v2-1,v1-1),
			     		id20(v2-1,v3-1),id21(v3-1,v2-1),
					id30(v3-1,v1-1),id31(v1-1,v3-1); 
			 */

			int id1,id2,id3;

			if ((id1 = edgeID(e1, _edge)) < 0) 
			{
				//				_edge[id10] = e1; _edge[id11] = e1;

				//				_edge[id10].setId(_edge.size()/2);
				//				_edge[id11].setId(_edge.size()/2);

				//				_edge[id10].add_triangle(&trig);
				//				_edge[id11].add_triangle(&trig);

				id1 = _edge.size();
				_edge.push_back(e1);
				_edge[_edge.size()-1] = e1;

				_node[v1-1].edges_to.push_back(v2-1);
				_node[v2-1].edges_to.push_back(v1-1);


				_node[v1-1].edges_cost.push_back(_v[v1-1].distance(_v[v2-1]));
				_node[v2-1].edges_cost.push_back(_v[v1-1].distance(_v[v2-1]));
			}

			if ((id2 = edgeID(e2, _edge)) < 0) 
			{
				/*
				_edge[id20] = e2; _edge[id21] = e2;

				_edge[id20].setId(_edge.size()/2);
				_edge[id21].setId(_edge.size()/2);

				_edge[id20].add_triangle(&trig);
				_edge[id21].add_triangle(&trig);
				 */

				id2 = _edge.size();
				e2.setId(id2);
				e2.add_triangle(trig._id);
				_edge.push_back(e2);
				_edge[_edge.size()-1] = e2;

				_node[v2-1].edges_to.push_back(v3-1);
				_node[v3-1].edges_to.push_back(v2-1);


				_node[v2-1].edges_cost.push_back(_v[v2-1].distance(_v[v3-1]));
				_node[v3-1].edges_cost.push_back(_v[v3-1].distance(_v[v2-1]));
			}

			if ((id3 = edgeID(e3, _edge)) < 0) 
			{
				/*
				_edge[id30] = e3; _edge[id31] = e3;

				_edge[id30].setId(_edge.size()/2);
				_edge[id31].setId(_edge.size()/2);

				_edge[id30].add_triangle(&trig);
				_edge[id31].add_triangle(&trig);
				 */

				id3 = _edge.size();
				e3.setId(id3);
				e3.add_triangle(trig._id);
				_edge.push_back(e3);

				_node[v3-1].edges_to.push_back(v1-1);
				_node[v1-1].edges_to.push_back(v3-1);


				_node[v3-1].edges_cost.push_back(_v[v3-1].distance(_v[v1-1]));
				_node[v1-1].edges_cost.push_back(_v[v1-1].distance(_v[v3-1]));
			}

			_edge[id1].add_triangle(trig._id);
			_edge[id2].add_triangle(trig._id);
			_edge[id3].add_triangle(trig._id);


			//			_trig[_trig.size()-1].setEdge(_edge[id10].id(), _edge[id20].id(), _edge[id30].id());
			_trig[_trig.size()-1].setEdge(id1,id2,id3); 
			//			_trig[_trig.size()-1].setEdge(&_edge[id1], &_edge[id2], &_edge[id3]);

			//cout << " set Edge "<< "ids " << id1 << ' '<< id2 << ' '<<id3<<'-' << _edge[id1].id() << ' ' <<  _edge[id2].id() << ' ' <<  _edge[id3].id() << endl;
			/*
			int tmpid = _trig.size()-1 ;
			cout << " trig " << _trig.size()-1 << ' '; 
			cout << _trig[tmpid].edge(0) << ' ' << _trig[tmpid].edge(1) << ' ' 
			     << _trig[tmpid].edge(2) << endl; 
			 */

		}
	}

	vector < vector < int > > facelist (_v.size());
	vector < Vector3f > facenorm (_trig.size());

	for (int i = 0; i < _edge.size(); i++) {
		//	cout << " edge " << i << " trig list " << _edge[i].getTrigList().size()<< endl;
	}

	for (int i = 0; i < _trig.size(); i++) 
	{
		/*
		cout << " trig " << i << ' '; 
			cout << _trig[i].edge(0) << ' ' << _trig[i].edge(1) << ' ' 
			     << _trig[i].edge(2) << endl; 
		 */


		Vector3f tmpv = (_v[_trig[i]._vertex[2]] - _v[_trig[i]._vertex[0]]) % 
				(_v[_trig[i]._vertex[1]] - _v[_trig[i]._vertex[0]]) ;

		tmpv.normalize();
		facenorm[i] = tmpv;

		facelist[_trig[i]._vertex[0]].push_back(i);
		facelist[_trig[i]._vertex[1]].push_back(i);
		facelist[_trig[i]._vertex[2]].push_back(i);
	}


	for (int i = 0; i < _v.size(); i++)  
	{
		Vector3f N(0.f,0.f,0.f); 

		float rate1, rate2;

		if (_v[i][1] > 0.5) 
		{
			rate1 = 1.f ; rate2 = 0.f;
		}
		else if (_v[i][1] < -0.5) 
		{
			rate1 = 0.f ; rate2 = 1.f;
		}
		else 
		{
			rate1 = _v[i][1] + 0.5f; rate2 = 1.f - rate1; 
		}

		//	cout << " v " << i << " 1:" << rate1 << " 2:" << rate2 << endl ;

		for (int j = 0; j < facelist[i].size(); j++) 
		{
			N += facenorm[facelist[i][j]]; 
			//		cout << " f " << facelist[i][j] << ' ' ;
		}
		//	cout << endl;

		N /= (float)facelist[i].size();

		_vn[i] = N;
	}


	_xmin = xmin; _ymin = ymin; _zmin = zmin;
	_xmax = xmax; _ymax = ymax; _zmax = zmax;

	f.close();

};

void loadBones(char * filename) {

	ifstream bonesfile;
	string line;
	bonesfile.open(filename);

	if (bonesfile == NULL) {
		cerr << "Failed reading bones file " << filename << endl;
		exit(1);
	}

	while(!bonesfile.eof()) {

		getline(bonesfile,line); // line is now the next line in file
		if (bonesfile.eof()) {
			break;
		}
		int i;
		vector<float> bone;
		float p1;
		float p2;
		float p3;
		int prev;
		char *fileLine = (char*)line.c_str(); //sscanf takes char array
		sscanf(fileLine,"%i %f %f %f %i",&i, &p1, &p2, &p3, &prev);
		//printf ("%i %f %f %f %i \n",i, p1, p2, p3, prev);
		bone.push_back(p1);
		bone.push_back(p2);
		bone.push_back(p3);
		bone.push_back(prev);
		bones.push_back(bone);
		Matrix4f id = Matrix4f();
		id.setIdentity();
		boneRotations.push_back(id);
	}

	/*Matrix4f id = Matrix4f();
	id.setIdentity();
	boneRotations.push_back(id);
	boneRotations.push_back(rotZ(1.5));
	//boneRotations.push_back(id);
	boneRotations.push_back(id);
	boneRotations.push_back(id);

	vector<float> b1(4);
	b1.insert(b1.begin(),1);
	b1.insert(b1.begin()+1,1);
	b1.insert(b1.begin()+2,0);
	b1.insert(b1.begin()+3,-1);
	bones.insert(bones.begin(),b1);

	vector<float> b2(4);
	b2.insert(b2.begin(),1);
	b2.insert(b2.begin()+1,-1);
	b2.insert(b2.begin()+2,0);
	b2.insert(b2.begin()+3,0);
	bones.insert(bones.begin() + 1,b2);

	vector<float> b3(4);
	b3.insert(b3.begin(),3);
	b3.insert(b3.begin()+1,-3);
	b3.insert(b3.begin()+2,0);
	b3.insert(b3.begin()+3,1);
	bones.insert(bones.begin() + 2,b3);

	vector<float> b4(4);
	b4.insert(b4.begin(),4);
	b4.insert(b4.begin()+1,0);
	b4.insert(b4.begin()+2,0);
	b4.insert(b4.begin()+3,2);
	bones.insert(bones.begin() + 3,b4);*/

	boneRotations[2] = rotX(1);
	bonesfile.close();
}

void loadWeights(char * filename) {
	ifstream weightsfile;
	weightsfile.open(filename);

	if (weightsfile == NULL) {
		cerr << "Failed reading weights file " << filename << endl;
		exit(1);
	}

	string line;

	while(!weightsfile.eof()) {

		vector <float> weights;
		for (int i = 0; i < 21; i++) {
			float w;
			weightsfile >> w;
			weights.push_back(w);
		}
		vertexWeights.push_back(weights);
	}
	weightsfile.close();
}

void recalcModelView(void)
{
	glPopMatrix();
	glPushMatrix();
	glTranslatef(xloc, yloc, zloc);
	glRotatef(angle, 0.0, 1.0, 0.0);
	glTranslatef(0, 0, .0);
	newModel = 0;
}

void myDisplay()
{
	bool DISPLAY_MESH = true;
	bool DISPLAY_BONES = false;

	if (newModel)
		recalcModelView();

	Matrix4f bone0 = translation(Vector3f(bones[0][0],bones[0][1],bones[0][2]));
	vector <Matrix4f> frameM (bones.size());
	vector <Matrix4f> frameMhatinv (bones.size());
	for (int i = 0; i < bones.size(); i++) {
		int currentBoneIndex = i;
		int joinBoneIndex = bones[i][3];
		Matrix4f m = Matrix4f();
		m.setIdentity();
		Matrix4f mhatinv = Matrix4f();
		mhatinv.setIdentity();
		while (joinBoneIndex != -1) {
			Vector3f currentBoneVector = Vector3f(bones[currentBoneIndex][0],bones[currentBoneIndex][1],bones[currentBoneIndex][2]);
			Vector3f currentJoinBone = Vector3f(bones[joinBoneIndex][0],bones[joinBoneIndex][1],bones[joinBoneIndex][2]);
			Matrix4f t = translation(currentBoneVector-currentJoinBone);
			m = (boneRotations[currentBoneIndex] * t) * m;
			mhatinv = !t * mhatinv;
			currentBoneIndex = joinBoneIndex;
			joinBoneIndex = bones[joinBoneIndex][3];
		}
		m = bone0 * m;
		mhatinv = !bone0 * mhatinv;
		frameM.insert(frameM.begin() + i,m);
		frameMhatinv.insert(frameMhatinv.begin() + i,mhatinv);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear OpenGL Window
	int trignum = trig.trigNum();
	Vector3f v1,v2,v3,n1,n2,n3,cv1,cv2,cv3;
	int v1Num, v2Num, v3Num;
	for (int i = 0 ; i < trignum; i++)
	{
		trig.getVertexIndices(i, v1Num, v2Num, v3Num);
		trig.getTriangleVertices(i,cv1,cv2,cv3); //copies which do not change
		float w;
		Vector3f v1 = Vector3f(0,0,0);
		Vector3f v2 = Vector3f(0,0,0);
		Vector3f v3 = Vector3f(0,0,0);
		for (int b = 0; b < bones.size(); b++) {

			w = vertexWeights[v1Num][b-1];
			v1 = v1 + (frameM[b] * (frameMhatinv[b] * cv1)) * w;

			w = vertexWeights[v2Num][b-1];
			v2 = v2 + (frameM[b] * (frameMhatinv[b] * cv2)) * w;

			w = vertexWeights[v3Num][b-1];
			v3 = v3 + (frameM[b] * (frameMhatinv[b] * cv3)) * w;

		}
		float m1,m2,m3,min,max;
		trig.getTriangleNormals(i,n1,n2,n3);
		trig.getMorseValue(i, m1, m2, m3);

		m1 = m2 = m3 = trig.color(i);

		GLfloat skinColor[] = {0.1, 1., 0.1, 1.0};

		if (max >= 0 && DISPLAY_MESH) {
			glBegin(GL_TRIANGLES);

			skinColor[1] = m1; skinColor[0] = 1-m1;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, skinColor);
			glNormal3f(-n1[0],-n1[1],-n1[2]);
			glVertex3f(v1[0],v1[1],v1[2]);

			skinColor[1] = m2; skinColor[0] = 1-m2;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, skinColor);
			glNormal3f(-n2[0],-n2[1],-n2[2]);
			glVertex3f(v2[0],v2[1],v2[2]);

			skinColor[1] = m3; skinColor[0] = 1-m3;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, skinColor);
			glNormal3f(-n3[0],-n3[1],-n3[2]);
			glVertex3f(v3[0],v3[1],v3[2]);

			skinColor[1] = m1; skinColor[0] = 1-m1;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, skinColor);
			glNormal3f(-n1[0],-n1[1],-n1[2]);
			glVertex3f(v1[0],v1[1],v1[2]);

			glEnd();
		}

	}
	for (int i = 0; i < bones.size(); i++) {
		int joinBoneIndex = bones[i][3];
		if (joinBoneIndex >= 0 && DISPLAY_BONES) {
			joinBoneIndex = bones[i][3];
			Vector3f joinBone = Vector3f(bones[joinBoneIndex][0],bones[joinBoneIndex][1],bones[joinBoneIndex][2]);
			Vector3f boneVector = Vector3f(bones[i][0],bones[i][1],bones[i][2]);
			Vector3f v1 = frameM[i] * (frameMhatinv[i]* boneVector);
			Vector3f v2 = frameM[joinBoneIndex] * (frameMhatinv[joinBoneIndex] * joinBone);
			if (i == 3) {
				printMatrix4f(frameM[i]);
				cout << "\n";
			}

			glLineWidth(10);
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_LINES);
			glVertex3f(v1[0],v1[1],v1[2]);
			glVertex3f(v2[0],v2[1],v2[2]);
			glEnd();
		}
	}


	glutSwapBuffers();
}


int main(int argc, char **argv)
{
	/* Matrix4f id = Matrix4f();
	id.setIdentity();
	Vector3f boneVector2 = Vector3f(-1,0,1);
	Vector3f boneVector = Vector3f(0,0,1);
	Vector3f joinBone = Vector3f(1,1,1);
	Matrix4f t1 = translation(boneVector-joinBone);
	Matrix4f m1 = rotZ(1.5) * t1;
	Matrix4f mhatinv1 =  !rotZ(1.5) * !t1;
	Matrix4f t2 = translation(boneVector2-boneVector);
	Matrix4f m2 = id * t2;
	Matrix4f mhatinv2 =  !id * !t2;
	Matrix4f m = m1 * m2;
	Matrix4f mhatinv = mhatinv1 * mhatinv2;

	Vector3f v = m * mhatinv * boneVector2;
	printVector3f(v);

	return 0; */

	if (argc >  1)  {
		trig.loadFile(argv[1]);
		loadBones("skeleton2.out");
		loadWeights("attachment2.out");
	}
	else {
		cerr << argv[0] << " <filename> " << endl;
		exit(1);
	}

	int width, height;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	glutInitWindowSize(nRows, nCols);
	glutCreateWindow("CAV Assignment 1");

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);


	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

	/* Setup the view of the cube. */
	glMatrixMode(GL_PROJECTION);
	gluPerspective( /* field of view in degree */ 40.0, 
			/* aspect ratio */ 1., /* Z near */ 1.0, /* Z far */ 1000.0);

	glMatrixMode(GL_MODELVIEW);

	gluLookAt(0.0, 0.0, 7.0,  /* eye is at (0,0,5) */
			0.0, 0.0, 0.0,      /* center is at (0,0,0) */
			0.0, 1.0, 0.0);      /* up is in positive Y direction */
	glPushMatrix();       /* dummy push so we can pop on model recalc */


	glutDisplayFunc(myDisplay);// Callback function

	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutTabletMotionFunc(tablet);
	glutSpaceballMotionFunc(translate);
	glutSpaceballRotateFunc(rotate);
	glutSpaceballButtonFunc(button);

	glutMainLoop();// Display everything and wait
}

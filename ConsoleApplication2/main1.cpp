#include <iostream>
#include <math.h>
#include <vector>
#include <time.h>
#include "glut.h"

using namespace std;
const int STEP_LG = 100;
const int STEP_MD = 10;
const int STEP_SM = 1;
const int STEP_DIV = 1000;
int step = STEP_LG;
bool forceLoadDirection = true;
int pointCount = 2;
int lineCount = 1;
int fulcrumCount = 1;
double torque;
double angularVel;
double angle;
bool physicsMode = false;
int width = 900;
int height = 900;
struct Line
{
	float velocityX, velocityY;
	double startX, startY;
	double endX, endY;
	unsigned char r, g, b, a;
};

//structure which stores relevant variables, radius, velocity, position and colour
struct Point
{
	double pointRadius = 1;
	double mass;
	float velocityX, velocityY;
	float transX, transY;
	double x, y,z;
	unsigned char r, g, b, a;
};

//a vector of Line structs called line which stores the information
vector<Line> line;
//a vector of Point structs called points which stores the information
vector<Point> point;
//a vector of Point structs called fulcrum which stores the information
vector<Point> fulcrum;

double getStep()
{
	double wut = (double)step / (double)STEP_DIV;
	cout << wut << "\n";
	return wut;
}

//does the distance calculation needed to detect collision(not used at all currently)
double distanceCalculate(double x1, double y1, double x2, double y2)
{
	double x = x1 - x2; //calculating number to square in next step
	double y = y1 - y2;
	double distance;

	distance = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);       //calculating Euclidean distance
	distance = sqrt(distance);

	cout << distance << endl;
	return distance;
}


void lineFunction(double startX, double startY, double endX, double endY)
{

	for (size_t i = 0; i < lineCount; i++)
	{
		Line ln;
		ln.startX = startX;
		ln.startY = startY;
		ln.endX = endX;
		ln.endY = endY;
		line.push_back(ln);
	}

	cout << "line x= " << startX << "line y = " << startY << endl;
}
void circleFulcrum(double x, double y)
{
	// generate random locations and random starting velocities for the particles
	for (size_t i = 0; i < fulcrumCount; i++)
	{
		Point fm;
		fm.x = x;
		fm.y = y;
		fulcrum.push_back(fm);
	}

	cout << "fulcrum x = " << x << " fulcrum y = " << y << endl;
}

void forceRotate()
{
	glTranslated(fulcrum[0].x, fulcrum[0].y, 0);
	glRotated(angle, 0, 0, 1);
	//cout << "torque = " << angle << endl;

	glTranslated(-fulcrum[0].x, -fulcrum[0].y, 0);


	//gluproject reference
	//http://stackoverflow.com/questions/11891481/gluproject-setup-in-opengl-vc-beginner
	GLdouble lx, ly, lz; // target 2d coords (z is 'unused')
	GLdouble fx, fy, fz;

	GLdouble model_view[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model_view);

	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluProject(fulcrum[0].x, fulcrum[0].y, fulcrum[0].z, model_view, projection, viewport, &fx, &fy, &fz);
	torque = 0;

	for (int i = 0; i < point.size(); i++)
	{
		gluProject(point[i].x, point[i].y, point[i].z, model_view, projection, viewport, &lx, &ly, &lz);
		double distance = lx - fx;


		//double distance = point[i].x - fulcrum[0].x;

		double force = distance*point[i].mass*.00001;
		torque -= force;
	}

	if (forceLoadDirection == true)
	{
		point[0].mass = point[0].mass;
	}
	else if (forceLoadDirection == false)
	{
		point[0].mass = -point[0].mass;
	}

		angularVel += torque;
		angle += angularVel;

		angularVel *= .999;

		//cout << "torque = " << torque << endl;
		//cout << "angular velocity = " << angularVel << endl;
		//cout << "angle = " << angle << endl;
 }
void circleLoad(double m, double x, double y,double z)
{
	// generate random locations and random starting velocities for the particles
		Point pt;
		pt.mass = m;
		pt.x = x;
		pt.y = y;
		pt.z = z;

		point.push_back(pt);


}
void drawLine()
{

	// draws the line/s stores in the vector
	for (size_t i = 0; i < lineCount; i++)
	{

		glLineWidth(5);
		glColor3f(1.0, 0.0, 0.0);

		glBegin(GL_LINES);
		glVertex3d(line[i].startX, line[i].startY, 0.0);
		glVertex3d(line[i].endX, line[i].endY, 0.0);

		glEnd();
	}

}
void drawforceDirection()
{
	//push current matrix onto the stack
	glPushMatrix();
	glutWireCone(5, 5, 50, 50);
	glTranslated(point[0].x, point[0].y, 0);
	glRotated(45, 0,1, 0);
	//pop the current matrix off the stack
	glPopMatrix();
}
void drawFulcrum()
{
	double dist1 = (point[0].transX - fulcrum[0].x);
	double dist2 = (point[1].transX - fulcrum[0].x);
	double staticEquilibrium = ((point[0].mass*dist1) - (point[1].mass*dist2));
	//cout <<"static equilibrium = "<< staticEquilibrium << endl;
	// draws the particles onto the screen particles are made of glutSolidSpheres
	for (size_t i = 0; i < fulcrumCount; i++)
	{
		//push current matrix onto the stack
		glPushMatrix();
		//moves (places) the particles at the random coordinates provided by the rand function(in main)
		glTranslated(fulcrum[i].x, fulcrum[i].y, -1);
		//generic colour provided elsewhere
		glColor3d(fulcrum[i].r, fulcrum[i].g, fulcrum[i].b);
		//particles of number particleCount radius = particleRadius

		if (staticEquilibrium>-0.01&&staticEquilibrium<0.01)
		{
			glColor3d(0, 1, 0);
		}
		else
		{
			glColor3d(1, 1, 1);
		}

		glutSolidSphere(fulcrum[i].pointRadius, 50, 50);
		//pop the current matrix off the stack
		glPopMatrix();
	}
}


void drawLoad()
{
	// draws the particles onto the screen particles are made of glutSolidSpheres
	for (size_t i = 0; i < point.size(); i++)
	{
		//push current matrix onto the stack
		glPushMatrix();
		//moves (places) the particles at the random coordinates provided by the rand function(in main)
		glTranslated(point[i].x, point[i].y, point[i].z);
		//generic colour provided elsewhere
		glColor3d(point[i].r, point[i].g, point[i].b);
		//particles of number particleCount radius = particleRadius

		//glColor3d(point[0].r = 1, point[0].g = 1, point[0].b = 0);
		glutSolidSphere(point[i].pointRadius, 50, 50);
		//pop the current matrix off the stack
		glPopMatrix();

	}

	//colour of force load
	glColor3d(point[0].r = 1, point[0].g = 1, point[0].b = 0);
	//colour of weight load
	glColor3d(point[1].r = 0, point[1].g = 0, point[1].b = 1);
}

void moveFulcrum(unsigned char key, int xL, int xR)
{

	xL = fulcrum[0].x;
	xR = fulcrum[0].y;
	xL = point[0].x;
	xR = point[0].y;
	xL = point[1].x;
	xR = point[1].y;

	if (key == 'a')
	{

		fulcrum[0].x -= getStep();
		point[2].x -= getStep();
		cout << "fulcrum position = " << fulcrum[0].x << endl;
	}
	if (key == 'd')
	{
		fulcrum[0].x += getStep();
		point[2].x += getStep();
		cout << "fulcrum position = " << fulcrum[0].x << endl;
	}
	if (key == 'q')
	{

		point[0].x -= getStep();
		cout << "force position = " << fulcrum[0].x << endl;
	}
	if (key == 'e')
	{
		point[0].x += getStep();
		cout << "force position = " << fulcrum[0].x << endl;
	}
	if (key == 'z')
	{

		point[1].x -= getStep();
		cout << "load position = " << fulcrum[0].x << endl;
	}
	if (key == 'c')
	{
		point[1].x += getStep();
		cout << "load position = " << fulcrum[0].x << endl;
	}
	if (key == 's'&&physicsMode==false)
	{
		cout << "physics mode true" << endl;
		physicsMode = true;

	}
	else if (key == 's'&&physicsMode == true)
	{
		cout << "physics mode false" << endl;
		physicsMode = false;

	}
	if (key == 'x')
	{
		cout << "step " << step;
		switch(step) {
			case STEP_LG:
				step = STEP_MD;
				break;
			case STEP_MD:
				step = STEP_SM;
				break;
			case STEP_SM:
				step = STEP_LG;
				break;
		}
		cout << " to " << step << "\n";
	}
	if (key == 'w'&&forceLoadDirection == true)
	{
		forceLoadDirection = false;
	}
	else if (key == 'w'&&forceLoadDirection == false)
	{
		forceLoadDirection = true;
	}

	if (fulcrum[0].x >= 50)
	{
		point[2].x = -50;
		fulcrum[0].x = -50;
	}

	if (fulcrum[0].x < -50)
	{
		point[2].x = 50;
		fulcrum[0].x = 50;
	}

	glutPostRedisplay();
}
//opengl stuff
void changeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45, ratio, 1, 100);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);

}
//opengl rendering stuff. updates 60 times a second(or something).
void renderScene(void)
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50, 50, -50, 50, -50, 50);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	if(physicsMode)
		forceRotate();

	drawLoad();
	drawLine();
	drawFulcrum();
	// drawforceDirection();

	//flushes the buffer
	glFlush();
	//swaps between the front buffer and the back buffer
	glutSwapBuffers();
	//updates the display
	glutPostRedisplay();
}

//main entry point *snigger*
int main(int argc, char **argv)
{
	//init randomizer
	srand(static_cast <unsigned> (time(0)));
	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	//window position
	glutInitWindowPosition(100, 100);
	//window size and name
	glutInitWindowSize(width, height);
	glutCreateWindow("Forces Acting On A Lever");
	//fulcrum
	circleFulcrum(0.0, 0.0);
	//force load>>>>>>>>>>>>>>
	circleLoad(10,20.0, 0.0,-1.0);
	//weight load>>>>>>>>>>>>>>>>>
	circleLoad(10.0,-20.0, 0.0,-1.0);
	//balancer for the fulcrum
	circleLoad(2.0, 0.0, 0.0,-1.0);
	cout <<"force mass = "<< point[0].mass << endl;
	cout << "load mass = " << point[1].mass << endl;
	cout << "fulcrum pos = " << fulcrum[0].x << endl;
	cout << "L = " << point[0].x - fulcrum[0].x << endl;
	cout << "X = " << fulcrum[0].x- point[1].x << endl;


	//lever
	lineFunction(20.0, 0.0, -20.0, 0.0);

	// register callback functions
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(moveFulcrum);

	// enter GLUT event processing loop
	glutMainLoop();

	return 1;
}
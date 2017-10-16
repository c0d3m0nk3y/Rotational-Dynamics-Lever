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

struct Point
{
	double pointRadius = 1;
	double mass;
	float velocityX, velocityY;
	double x, y,z;
	unsigned char r, g, b, a;
};

vector<Line> line;
vector<Point> point;
vector<Point> fulcrum;

double getStep()
{
	double wut = (double)step / (double)STEP_DIV;
	cout << wut << "\n";
	return wut;
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
	for (size_t i = 0; i < fulcrumCount; i++)
	{
		Point fm;
		fm.x = x;
		fm.y = y;
		fm.r = 1;
		fm.g = 1;
		fm.b = 0;
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

		double mass = (forceLoadDirection ? point[0].mass : -point[0].mass);
		double force = distance * mass * 0.00001;
		torque -= force;
	}

	angularVel += torque;
	angle += angularVel;
	angularVel *= .999;

	cout << "point[0].mass = " << point[0].mass << endl;
	//cout << "torque = " << torque << endl;
	//cout << "angular velocity = " << angularVel << endl;
	//cout << "angle = " << angle << endl;
 }

void circleLoad(double m, double x, double y, double z, double r, double b, double g)
{
		Point pt;
		pt.mass = m;
		pt.x = x;
		pt.y = y;
		pt.z = z;
		pt.r = r;
		pt.g = g;
		pt.b = b;

		point.push_back(pt);
}

void drawLine()
{
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
	glPushMatrix();
	glutWireCone(5, 5, 50, 50);
	glTranslated(point[0].x, point[0].y, 0);
	glRotated(45, 0,1, 0);
	glPopMatrix();
}

void drawFulcrum()
{
	for (size_t i = 0; i < fulcrumCount; i++)
	{
		glPushMatrix();
		glTranslated(fulcrum[i].x, fulcrum[i].y, -1);
		glColor3d(fulcrum[i].r, fulcrum[i].g, fulcrum[i].b);
		glutSolidSphere(fulcrum[i].pointRadius, 50, 50);
		glPopMatrix();
	}
}

void drawLoad()
{
	for (size_t i = 0; i < point.size(); i++)
	{
		glPushMatrix();
		glTranslated(point[i].x, point[i].y, point[i].z);
		glColor3d(point[i].r, point[i].g, point[i].b);
		glutSolidSphere(point[i].pointRadius, 50, 50);
		glPopMatrix();
	}
}

void keyboardFunc(unsigned char key, int xL, int xR)
{
	// fulcrum left/right
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

	// force left/right
	if (key == 'q')
	{

		point[0].x -= getStep();
		cout << "force position = " << point[0].x << endl;
	}
	if (key == 'e')
	{
		point[0].x += getStep();
		cout << "force position = " << point[0].x << endl;
	}

	// load left/right
	if (key == 'z')
	{

		point[1].x -= getStep();
		cout << "load position = " << point[1].x << endl;
	}
	if (key == 'c')
	{
		point[1].x += getStep();
		cout << "load position = " << point[1].x << endl;
	}

	if (key == 's')
	{
		physicsMode = !physicsMode;
		cout << "physics mode " << physicsMode << endl;
	}

	if ('w' == key)
		forceLoadDirection = !forceLoadDirection;

	if (key == 'x')
	{
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
		cout << "step " << step << "\n";
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

void changeSize(int w, int h)
{
	if (h == 0)
		h = 1;

	float ratio = w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45, ratio, 1, 100);
	glMatrixMode(GL_MODELVIEW);
}

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

int main(int argc, char **argv)
{
	//init randomizer
	srand(static_cast <unsigned> (time(0)));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Forces Acting On A Lever");
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboardFunc);

	circleFulcrum(0.0, 0.0);
	circleLoad(0,20.0, 0.0, -1.0, 1, 1, 0);
	circleLoad(10.0, -20.0, 0.0, -1.0, 0, 0, 1);
	circleLoad(2.0, 0.0, 0.0, -1.0, 0, 1, 0);

	cout <<"force mass = "<< point[0].mass << endl;
	cout << "load mass = " << point[1].mass << endl;
	cout << "fulcrum pos = " << fulcrum[0].x << endl;
	cout << "L = " << point[0].x - fulcrum[0].x << endl;
	cout << "X = " << fulcrum[0].x- point[1].x << endl;

	lineFunction(20.0, 0.0, -20.0, 0.0);

	glutMainLoop();

	return 1;
}
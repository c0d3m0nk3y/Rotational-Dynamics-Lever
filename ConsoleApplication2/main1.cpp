#include <iostream>
#include <math.h>
#include <vector>
#include <time.h>
#include "glut.h"
#include "GameObject.h"

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
vector<GameObject> gameObjects;
GameObject fulcrum;

struct Point
{
	double pointRadius = 1;
	double mass;
	float velocityX, velocityY;
	double x, y,z;
	unsigned char r, g, b, a;
};

vector<Point> point;

double getStep()
{
	return (double)step / (double)STEP_DIV;
}

void addLine(double startX, double startY, double endX, double endY)
{
	GameObject go = GameObject(startX, startY, 0, 1, 0, 0, GameObject::TYPE_LINE);
	go.lx = endX;
	go.ly = endY;
	go.lz = 0;
	gameObjects.push_back(go);
}

void addFulcrum(double x, double y)
{
	fulcrum = GameObject(x, y, 0, 1, 1, 0, GameObject::TYPE_FULCRUM);
	fulcrum.size = 1;
	gameObjects.push_back(fulcrum);
}

void addCircle(double m, double x, double y, double z, double r, double b, double g)
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

void forceRotate()
{
	glRotated(angle, 0, 0, 1);
	glTranslated(-fulcrum.x, -fulcrum.y, 0); // center cam on fulcrum

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

	gluProject(fulcrum.x, fulcrum.y, fulcrum.z, model_view, projection, viewport, &fx, &fy, &fz);
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
 }

void drawLine()
{
	for(size_t i = 0; i < gameObjects.size(); i++) {
		if(gameObjects[i].type == GameObject::TYPE_LINE) {
			GameObject go = gameObjects[i];

			glLineWidth(5);
			glColor3f(go.r, go.g, go.b);
			glBegin(GL_LINES);
			glVertex3d(go.x, go.y, go.z);
			glVertex3d(go.lx, go.ly, go.lz);
			glEnd();
		}
	}
}

void drawFulcrum()
{
	glPushMatrix();
	glTranslated(fulcrum.x, fulcrum.y, -1);
	glColor3d(fulcrum.r, fulcrum.g, fulcrum.b);
	glutSolidSphere(fulcrum.size, 50, 50);
	glPopMatrix();
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
		fulcrum.x -= getStep();
		cout << "fulcrum position = " << fulcrum.x << endl;
	}
	if (key == 'd')
	{
		fulcrum.x += getStep();
		cout << "fulcrum position = " << fulcrum.x << endl;
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
	{
		forceLoadDirection = !forceLoadDirection;
		cout << "load direction " << forceLoadDirection << endl;
	}

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

	if (fulcrum.x >= 50)
	{
		fulcrum.x = -50;
	}

	if (fulcrum.x < -50)
	{
		fulcrum.x = 50;
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

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Forces Acting On A Lever");
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboardFunc);

	addFulcrum(0.0, 0.0);
	addCircle(500.0, 20.0, 0.0, -1.0, 1, 1, 0);
	addCircle(10.0, -20.0, 0.0, -1.0, 0, 0, 1);
	addLine(20.0, 0.0, -20.0, 0.0);

	cout <<"force mass = "<< point[0].mass << endl;
	cout << "load mass = " << point[1].mass << endl;

	glutMainLoop();

	return 1;
}
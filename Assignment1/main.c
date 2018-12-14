/*******************************************************************
Multi-Part Model Construction and Manipulation
Code based off of Skeleton Code given.
********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "CubeMesh.h"
#include "QuadMesh.h"

const int meshSize = 16;    // Default Mesh Size
const int vWidth = 650;     // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

static int currentButton;
static unsigned char currentKey;

//Submarine properties
static GLfloat m_propellerSpin = 0; //Rotation value for propellers
static GLfloat m_submarineSpin = 0; //Holds the rotation value of Submarine
static GLfloat m_submarineHeight = 0; //Y Value of Submarine
static GLfloat m_propellerSpinSpeed = 5; //Rate that the propeller spins
static GLfloat m_submarineSpeed = 0.02; //Rate that the submarine moves forward/backwards
static GLfloat m_submarineRotationSpeed = 0.002; //Rate that the submarine rotates

//For dealing with the direction the submarine is facing
static GLfloat m_dirVX = 0; //The amount that the submarine will move in the X direction depending on what direction it's facing
static GLfloat m_dirVZ = 0; //The amount that the submarine will move in the Z direction depending on what direction it's facing
static GLfloat m_speedMultiplier = 0; //Speed multiplier for submarine when moving forwards or backwards, used with timer function
static GLfloat m_PI = 3.14159265359;

// Lighting/shading and material properties for submarine - upcoming lecture - just copy for now

// Light properties
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Material properties
static GLfloat submarine_mat_ambient[] = { 0.4F, 0.2F, 0.0F, 1.0F };
static GLfloat submarine_mat_specular[] = { 0.1F, 0.1F, 0.0F, 1.0F };
static GLfloat submarine_mat_diffuse[] = { 0.9F, 0.5F, 0.0F, 1.0F };
static GLfloat submarine_mat_shininess[] = { 0.0F };

// A quad mesh representing the ground / sea floor 
static QuadMesh groundMesh;

// Structure defining a bounding box, currently unused
//struct BoundingBox {
//    Vector3D min;
//    Vector3D max;
//} BBox;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
Vector3D ScreenToWorld(int x, int y);


int main(int argc, char **argv)
{
	printf("Press 'F1' for help.\n");
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("Assignment 1");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). */
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);   // This light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	// Set up ground/sea floor quad mesh
	Vector3D origin = NewVector3D(-8.0f, 0.0f, 8.0f);
	Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
	Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
	groundMesh = NewQuadMesh(meshSize);
	InitMeshQM(&groundMesh, meshSize, origin, 16.0, 16.0, dir1v, dir2v);

	Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
	Vector3D diffuse = NewVector3D(0.3f, 0.3f, 0.5f);
	Vector3D specular = NewVector3D(0.05f, 0.04f, 0.04f);
	SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.5);

	// Set up the bounding box of the scene
	// Currently unused. You could set up bounding boxes for your objects eventually.
	//Set(&BBox.min, -8.0f, 0.0, -8.0);
	//Set(&BBox.max, 8.0f, 6.0,  8.0);
}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Set submarine material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT, submarine_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, submarine_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, submarine_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, submarine_mat_shininess);



	// Apply transformations to construct and transform submarine to appropriate location
	glPushMatrix();
	glTranslatef(0 + m_dirVX, 3 + m_submarineHeight, 0 + m_dirVZ);
	glRotatef((m_submarineSpin * 180) / m_PI, 0, 1, 0);

	//Body of Submarine
	glPushMatrix();
	glScalef(1.0, 1.2, 8.0);
	glutSolidSphere(1.0, 50, 50);
	glPopMatrix();

	//Creating the main portion of the sail
	glPushMatrix();
	glTranslatef(0, 1.2, -2);
	glScalef(0.4, 0.8, 3);
	glutSolidCube(1);
	glPopMatrix();

	//Right Wing of Sail
	glPushMatrix();
	glTranslatef(0.3, 1.3, -2.7);
	glScalef(0.5, 0.1, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	//Left Wing of Sail
	glPushMatrix();
	glTranslatef(-0.3, 1.3, -2.7);
	glRotatef(180, 0, 1, 0);
	glScalef(0.5, 0.1, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	//Submarine Tail
	glPushMatrix();
	glTranslatef(0, 0, 7.7);
	glutSolidCone(0.26, 1, 25, 25);
	glPopMatrix();

	//Right Wing of Tail
	glPushMatrix();
	glTranslatef(0.4, 0, 7.2);
	glScalef(1.3, 0.05, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	//Left Wing of Tail
	glPushMatrix();
	glTranslatef(-0.4, 0, 7.2);
	glRotatef(180, 0, 1, 0);
	glScalef(1.3, 0.05, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	//Submarine Propellers
	for (int i = 0;i < 4;i++)
	{
		glPushMatrix();
		glTranslatef(0, 0, 8);
		glTranslatef(-0.02, 0, 0);
		glRotatef((90 * i) + m_propellerSpin, 0, 0, 1);
		glTranslatef(0.02, 0, 0);
		glScalef(1, 0.01, 0.4);
		glutSolidCube(1);
		glPopMatrix();
	}

	glPopMatrix();

	//Creates static square on ground
	glPushMatrix();
	glScalef(3, 1, 3);
	glutSolidCube(1);
	glPopMatrix();

	// Draw ground/sea floor
	DrawMeshQM(&groundMesh, meshSize);

	glutSwapBuffers();   // Double buffering, swap buffers
}


// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//Timer function
//When called the function goes through if statements to know whether or not the submarine
//is moving forwards or backwards. Depending on the direction it goes, it will move the submarine
//and make the propeller rotate in the appropriate direction. The m_speedMultiplier variable
//controls the different rates that the submarine moves at. After making the appropriate transformations,
//the submarine gets redrawn and the function gets called looped again by the glutTimerFunc function.
//Otherwise, if it detects that the speed of the submarine is at 0, it will stop the propeller from moving
//and exit out of the function without looping.
void timerSubmarine()
{
	if (m_speedMultiplier > 0)
	{
		m_propellerSpin -= m_propellerSpinSpeed * m_speedMultiplier;
		m_dirVZ -= (m_submarineSpeed * m_speedMultiplier)*cos(m_submarineSpin);
		m_dirVX -= (m_submarineSpeed * m_speedMultiplier)*sin(m_submarineSpin);
		glutPostRedisplay();
		glutTimerFunc(50, timerSubmarine, 1);
	}
	else if (m_speedMultiplier < 0)
	{
		m_propellerSpin -= m_propellerSpinSpeed * m_speedMultiplier;
		m_dirVZ += (m_submarineSpeed * -m_speedMultiplier)*cos(m_submarineSpin);
		m_dirVX += (m_submarineSpeed * -m_speedMultiplier)*sin(m_submarineSpin);
		glutPostRedisplay();
		glutTimerFunc(50, timerSubmarine, 1);
	}
	else
	{
		m_propellerSpin = 0;
	}

}


// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a': //Rotate submarine and propeller to the right
		m_propellerSpin += m_propellerSpinSpeed * 3;
		m_submarineSpin += m_submarineRotationSpeed;
		break;
	case 'd': //Rotate submarine and propeller to the left
		m_propellerSpin -= m_propellerSpinSpeed * 3;
		m_submarineSpin -= m_submarineRotationSpeed;
		break;
	case 'w': //Move submarine forwards while rotating propeller to the left
		m_propellerSpin -= m_propellerSpinSpeed * 2;
		m_dirVZ -= m_submarineSpeed*cos(m_submarineSpin);
		m_dirVX -= m_submarineSpeed*sin(m_submarineSpin);
		break;
	case 's': //Move submarine backwards while rotating propeller to the right
		m_propellerSpin += m_propellerSpinSpeed * 2;
		m_dirVZ += m_submarineSpeed*cos(m_submarineSpin);
		m_dirVX += m_submarineSpeed*sin(m_submarineSpin);
		break;
	case 'e': //Checks if submarine is already past the cap for moving forwards, if not,
		      //it either increases the rate that the submarine moves forward, or
		      //decreases the rate it moves backwards. Timer function than gets called
		      //to allow submarine to make the appropriate animation choice
		if (m_speedMultiplier < 0.25)
		{
			m_speedMultiplier += 0.05;
		}
		else
		{
			m_speedMultiplier = 0.25;
		}
		timerSubmarine();
		break;
	case 'q': //Checks if submarine is already past the cap for moving backwards, if not,
		      //it either increases the rate that the submarine moves backwards, or decreases
		      //the rate it moves forwards. Timer function than gets called to allow the submarine
		      //to make the appropriate animation choice
		if (m_speedMultiplier > -0.25)
		{
			m_speedMultiplier -= 0.05;
		}
		else
		{
			m_speedMultiplier = -0.25;
		}
		timerSubmarine();
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key, outputs information to the console, to let the user know what they can do with the
	//submarine
	if (key == GLUT_KEY_F1)
	{
		printf("To rotate the submarine to the right, press the 'd' key.\n");
		printf("To rotate the submarine to the left, press the 'a' key.\n");
		printf("To move the submarine forwards, press the 'w' key.\n");
		printf("To move the submarine backwards, press the 's' key.\n");
		printf("Use the up arrow key to raise the submarine.\n");
		printf("Use the down arrow key to lower the submarine (please note that there is a cap to prevent the submarine from hitting either the static object, or the floor.\n");
		printf("To make the submarine move forwards, slow down submarine\n when reversing, or make the submarine move forwards faster, press the 'e' key.\n");
		printf("To make the submarine move backwards, slow down when\n moving forwards, or make the submarine move backwards faster, press the 'q' key.\n");
	}
	else if (key == GLUT_KEY_DOWN) //Moves the submarine downwards until it hits the cap
	{
		if (m_submarineHeight > 0)
		{
			m_submarineHeight -= m_submarineSpeed;
			m_propellerSpin -= m_propellerSpinSpeed;
		}
	}
	else if (key == GLUT_KEY_UP) //Moves submarine upwards without restriction
	{
		m_submarineHeight += m_submarineSpeed;
		m_propellerSpin += m_propellerSpinSpeed;
	}
	// Do transformations with arrow keys
	//else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
	//{
	//}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


Vector3D ScreenToWorld(int x, int y)
{
	// you will need to finish this if you use the mouse
	return NewVector3D(0, 0, 0);
}

// (your name here)
// Chess animation starter kit.

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <ctime>
using namespace std;
#include "glut.h"
#include "graphics.h"


// Global Variables
// Some colors you can use, or make your own and add them
// here and in graphics.h
GLdouble redMaterial[] = {0.7, 0.1, 0.2, 1.0};
GLdouble greenMaterial[] = {0.1, 0.7, 0.4, 1.0};
GLdouble brightGreenMaterial[] = {0.1, 0.9, 0.1, 1.0};
GLdouble blueMaterial[] = {0.1, 0.2, 0.7, 1.0};
GLdouble whiteMaterial[] = {1.0, 1.0, 1.0, 1.0};

double screen_x = 600;
double screen_y = 500;

enum PIECES { PAWN=2, KNIGHT, BISHOP, ROOK, KING, QUEEN};


double GetTime()
{
	static clock_t start_time = clock();
	clock_t current_time = clock();
	double total_time = double(current_time - start_time) / CLOCKS_PER_SEC;
	return total_time;
}

// Outputs a string of text at the specified location.
void text_output(double x, double y, const char *string)
{
	void *font = GLUT_BITMAP_9_BY_15;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
	
	int len, i;
	glRasterPos2d(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) 
	{
		glutBitmapCharacter(font, string[i]);
	}

    glDisable(GL_BLEND);
}

// Given the three triangle points x[0],y[0],z[0],
//		x[1],y[1],z[1], and x[2],y[2],z[2],
//		Finds the normal vector n[0], n[1], n[2].
void FindTriangleNormal(double x[], double y[], double z[], double n[])
{
	// Convert the 3 input points to 2 vectors, v1 and v2.
	double v1[3], v2[3];
	v1[0] = x[1] - x[0];
	v1[1] = y[1] - y[0];
	v1[2] = z[1] - z[0];
	v2[0] = x[2] - x[0];
	v2[1] = y[2] - y[0];
	v2[2] = z[2] - z[0];
	
	// Take the cross product of v1 and v2, to find the vector perpendicular to both.
	n[0] = v1[1]*v2[2] - v1[2]*v2[1];
	n[1] = -(v1[0]*v2[2] - v1[2]*v2[0]);
	n[2] = v1[0]*v2[1] - v1[1]*v2[0];

	double size = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
	n[0] /= -size;
	n[1] /= -size;
	n[2] /= -size;
}

// Loads the given data file and draws it at its default position.
// Call glTranslate before calling this to get it in the right place.
void DrawPiece(const char filename[])
{
	// Try to open the given file.
	char buffer[200];
	ifstream in(filename);
	if(!in)
	{
		cerr << "Error. Could not open " << filename << endl;
		exit(1);
	}

	double x[100], y[100], z[100]; // stores a single polygon up to 100 vertices.
	int done = false;
	int verts = 0; // vertices in the current polygon
	int polygons = 0; // total polygons in this file.
	do
	{
		in.getline(buffer, 200); // get one line (point) from the file.
		int count = sscanf_s(buffer, "%lf, %lf, %lf", &(x[verts]), &(y[verts]), &(z[verts]));
		done = in.eof();
		if(!done)
		{
			if(count == 3) // if this line had an x,y,z point.
			{
				verts++;
			}
			else // the line was empty. Finish current polygon and start a new one.
			{
				if(verts>=3)
				{
					glBegin(GL_POLYGON);
					double n[3];
					FindTriangleNormal(x, y, z, n);
					glNormal3dv(n);
					for(int i=0; i<verts; i++)
					{
						glVertex3d(x[i], y[i], z[i]);
					}
					glEnd(); // end previous polygon
					polygons++;
					verts = 0;
				}
			}
		}
	}
	while(!done);

	if(verts>0)
	{
		cerr << "Error. Extra vertices in file " << filename << endl;
		exit(1);
	}

}

// NOTE: Y is the UP direction for the chess pieces.
double eye[3] = {4500, 8000, -4000}; // pick a nice vantage point.
double at[3]  = {4500, 0,     4000};
//
// GLUT callback functions
//

// As t goes from t0 to t1, set v between v0 and v1 accordingly.
void Interpolate(double t, double t0, double t1,
	double & v, double v0, double v1)
{
	double ratio = (t - t0) / (t1 - t0);
	if (ratio < 0)
		ratio = 0;
	if (ratio > 1)
		ratio = 1;
	v = v0 + (v1 - v0)*ratio;
}

void DrawBoard()
{
	GLfloat board1[] = { 0.9, 0.8, 0.9, 1.0 };
	GLfloat board2[] = { 0.2, 0.3, 0.1, 1.0 };
	glBegin(GL_QUADS);
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if ((i + j) % 2 == 0)
				glMaterialfv(GL_FRONT, GL_DIFFUSE, board1);
			else
				glMaterialfv(GL_FRONT, GL_DIFFUSE, board2);
			glNormal3d(0, 1, 0);
			double offset_x = 500;
			double offset_z = 500;
			glVertex3d(i * 1000 + offset_x, 0, j * 1000 + offset_z);
			glVertex3d(i * 1000 + offset_x, 0, (j + 1) * 1000 + offset_z);
			glVertex3d((i + 1) * 1000 + offset_x, 0, (j + 1) * 1000 + offset_z);
			glVertex3d((i + 1) * 1000 + offset_x, 0, j * 1000 + offset_z);
		}
	glEnd();
}

// This callback function gets called by the Glut
// system whenever it decides things need to be redrawn.
void display(void)
{
	double t = GetTime();
	double z, x, y;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],  at[0], at[1], at[2],  0,1,0); // Y is up!

	// Set the color for one side (white), and draw its 16 pieces.
	GLfloat mat_amb_diff1[] = {0.8f, 0.9f, 0.5f, 1.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff1);


	glPushMatrix();
	Interpolate(t, 1, 3, x, 1, 2);
	glTranslatef(1000, 0, 1000);
	glScaled(x, 1, x);
	//DrawPiece("ROOK.POL");
	glCallList(ROOK);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2000, 0, 1000);
	//DrawPiece("KNIGHT.POL");
	glCallList(KNIGHT);
	glPopMatrix();


	Interpolate(t, 10.6, 11.5, x, 6000, 5000);
	Interpolate(t, 10.6, 11.5, z, 4000, 3000);
	Interpolate(t, 8.6, 9.5, x, 7000, 6000);
	Interpolate(t, 8.6, 9.5, z, 5000, 4000);
	Interpolate(t, 5.0, 7.5, x, 3000, 7000);
	Interpolate(t, 5.0, 7.5, z, 1000, 5000);
	glPushMatrix();
	glTranslatef(x, 0, z);
	//DrawPiece("BISHOP.POL");
	glCallList(BISHOP);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(4000, 0, 1000);
	//DrawPiece("KING.POL");
	glCallList(KING);
	glPopMatrix();


	Interpolate(t, 8.5, 9.5, x, 5000, 3000);
	Interpolate(t, 8.5, 9.5, z, 1000, 3000);
	glPushMatrix();
	glTranslatef(x, 0, z);
	//DrawPiece("QUEEN.POL");
	glCallList(QUEEN);
	glPopMatrix();

	//Interpolate(t, 11.2, 11.5, y, 1000, 0); // not working 
	//Interpolate(t, 10.6, 11.5, x, 7000, 6000);
	//Interpolate(t, 10.6, 10.8, y, 0, 1000);
	//Interpolate(t, 10.6, 11.5, z, 1000, 3000);
	glPushMatrix();
	glTranslatef(7000, 0, 1000);
	//DrawPiece("KNIGHT.POL");
	glCallList(KNIGHT);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(8000, 0, 1000);
	//DrawPiece("ROOK.POL");
	glCallList(ROOK);
	glPopMatrix();

	glPushMatrix(); 
	glTranslatef(6000, 0, 1000);
	//DrawPiece("BISHOP.POL");
	glCallList(BISHOP);
	glPopMatrix();

	for (int x = 1000; x <= 7000; x += 1000)
	{
		if (x == 4000) {
			Interpolate(t, 2.0, 3.0, z, 2000, 3000);
			glPushMatrix();
			glTranslatef(x, 0, z);
			glCallList(PAWN);
			glPopMatrix();
			x += 1000;
		}
		/*if (x == 5000) {
			Interpolate(t, 5.0, 7.0, z, 2000, 4000);
			glPushMatrix();
			glTranslatef(x, 0, z);
			glCallList(PAWN);
			glPopMatrix();
			x += 1000;
		}*/
		glPushMatrix();
		glTranslatef(x, 0, 2000);
		//DrawPiece("PAWN.POL");
		glCallList(PAWN);
		glPopMatrix();
	}

	//Interpolate(t, 1.0, 3.0, z, 2000, 4000);
	//Interpolate(t, 5.5, 7, x, x, 4000);
	glPushMatrix();
	glTranslatef(8000, 0, 2000);
	glCallList(PAWN);
	glPopMatrix();

	// Set the color for one side (black), and draw its 16 pieces.
	GLfloat mat_amb_diff2[] = {0.1f, 0.5f, 0.8f, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff2);

	glPushMatrix();
	glTranslatef(1000, 0, 8000);
	//DrawPiece("ROOK.POL");
	glCallList(ROOK);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2000, 0, 8000);
	//DrawPiece("KNIGHT.POL");
	glCallList(KNIGHT);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3000, 0, 8000);
	//DrawPiece("BISHOP.POL");
	glCallList(BISHOP);
	glPopMatrix();

	glPushMatrix();
	Interpolate(t, 4, 6, y, 0, 400);
	glTranslatef(4000, y, 8000);
	double deg = 0;
	Interpolate(t, 4, 6, deg, 0, 90);
	glRotated(deg, 1, 0, 0);
	//DrawPiece("KING.POL");
	glCallList(KING);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5000, 0, 8000);
	//DrawPiece("QUEEN.POL");
	glCallList(QUEEN);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(6000, 0, 8000);
	//DrawPiece("BISHOP.POL");
	glCallList(BISHOP);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7000, 0, 8000);
	//DrawPiece("KNIGHT.POL");
	glCallList(KNIGHT);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(8000, 0, 8000);
	//DrawPiece("ROOK.POL");
	glCallList(ROOK);
	glPopMatrix();

	for(int x=1000; x<=8000; x+=1000)
	{
		if (x == 5000) {
			Interpolate(t, 9.6, 10.5, z, 7000, 5000);
			glPushMatrix();
			glTranslatef(x, 0, z);
			glCallList(PAWN);
			glPopMatrix();
			x += 1000;
		}
		if (x == 6000) {
			Interpolate(t, 7.6, 8.5, z, 7000, 6000);
			glPushMatrix();
			glTranslatef(x, 0, z);
			glCallList(PAWN);
			glPopMatrix();
			x += 1000;
		}
		if (x == 7000) {
			Interpolate(t, 3.0, 5.0, z, 7000, 5000);
			Interpolate(t, 7.2, 7.5, y, 0, -1500);
			glPushMatrix();
			glTranslatef(x, y, z);
			glCallList(PAWN);
			glPopMatrix();
			x += 1000;
		}
		glPushMatrix();
		glTranslatef(x, 0, 7000);
		//DrawPiece("PAWN.POL");
		glCallList(PAWN);
		glPopMatrix();
	}

	// Set the light position, and draw it yellow.
	double lightx = 6000;
	double lighty = 1500;
	double lightz = 5000;
	GLfloat yellow[] = { 1.0, 1.0, 0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, yellow);
	glPushMatrix();
	glTranslatef(lightx, lighty, lightz);
	glutSolidSphere(200, 50, 50);
	glPopMatrix();

	GLfloat light_position[] = {1,2,-.1f, 0}; // light comes FROM this vector direction.
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // position first light

	DrawBoard();

	glutSwapBuffers();
	glutPostRedisplay();
}


// This callback function gets called by the Glut
// system whenever a key is pressed.
void keyboard(unsigned char c, int x, int y)
{
	switch (c) 
	{
		case 27: // escape character means to quit the program
			exit(0);
			break;
		default:
			return; // if we don't care, return without glutPostRedisplay()
	}

	glutPostRedisplay();
}



void SetPerspectiveView(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double aspectRatio = (GLdouble) w/(GLdouble) h;
	gluPerspective( 
	/* field of view in degree */ 45.0,
	/* aspect ratio */ aspectRatio,
	/* Z near */ 100, /* Z far */ 30000.0);
	glMatrixMode(GL_MODELVIEW);
}

// This callback function gets called by the Glut
// system whenever the window is resized by the user.
void reshape(int w, int h)
{
	screen_x = w;
	screen_y = h;

	// Set the pixel resolution of the final picture (Screen coordinates).
	glViewport(0, 0, w, h);

	SetPerspectiveView(w,h);

}

// This callback function gets called by the Glut
// system whenever any mouse button goes up or down.
void mouse(int mouse_button, int state, int x, int y)
{
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
	}
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
	{
	}
	if (mouse_button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) 
	{
	}
	if (mouse_button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) 
	{
	}
	glutPostRedisplay();
}

// Your initialization code goes here.
void InitializeMyStuff()
{
	// set material's specular properties
	GLfloat mat_specular[] = {.9, .9, .9, 1.0};
	GLfloat mat_shininess[] = {90.0};
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	// set light properties
	//GLfloat light_position[] = {(float)eye[0], (float)eye[1], (float)eye[2],1};
	GLfloat white_light[] = {1,1,1,1};
	GLfloat low_light[] = {.3f,.3f,.3f,1};
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position); // position first light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light); // specify first light's color
	glLightfv(GL_LIGHT0, GL_SPECULAR, low_light);

	glEnable(GL_DEPTH_TEST); // turn on depth buffering
	glEnable(GL_LIGHTING);	// enable general lighting
	glEnable(GL_LIGHT0);	// enable the first light.

	glNewList(PAWN, GL_COMPILE);
	DrawPiece("PAWN.POL");
	glEndList();

	glNewList(ROOK, GL_COMPILE);
	DrawPiece("ROOK.POL");
	glEndList();

	glNewList(KING, GL_COMPILE);
	DrawPiece("KING.POL");
	glEndList();

	glNewList(QUEEN, GL_COMPILE);
	DrawPiece("QUEEN.POL");
	glEndList();

	glNewList(KNIGHT, GL_COMPILE);
	DrawPiece("KNIGHT.POL");
	glEndList();

	glNewList(BISHOP, GL_COMPILE);
	DrawPiece("BISHOP.POL");
	glEndList();
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screen_x, screen_y);
	glutInitWindowPosition(10, 10);

	int fullscreen = 0;
	if (fullscreen) 
	{
		glutGameModeString("800x600:32");
		glutEnterGameMode();
	} 
	else 
	{
		glutCreateWindow("Chess");
	}

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);

	glClearColor(1,1,1,1);	
	InitializeMyStuff();

	glutMainLoop();

	return 0;
}

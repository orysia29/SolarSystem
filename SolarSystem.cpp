//SolarSystem.cpp
//3D Texture Mapped Model of the Solar System
//Modeled to An Approximate Logarithmic Scale
//Particle System For The Sun Implemented

#include "glut.h"
#include <windows.h>

#include <math.h>
#include "RGBpixmap.h" 
#include "SolarSystemGlobals.h"
#include "ParticleSystem.h"

/********************/
/* Global Variables */
/********************/

// Viewer positioning angles.
GLfloat viewerAzimuth = INITIAL_VIEWER_AZIMUTH;
GLfloat viewerZenith  = INITIAL_VIEWER_ZENITH;

// ID numbers for all texture maps.
GLuint ParticleTextureName = -1;
GLuint EarthTextureName = 3;
GLuint MoonTextureName  = 4;
GLuint SunTextureName   = 0;
GLuint MercuryTextureName = 1;
GLuint VenusTextureName = 2;
GLuint MarsTextureName = 5;
GLuint JupiterTextureName = 6;
GLuint SaturnTextureName = 7;
GLuint UranusTextureName = 8;
GLuint NeptuneTextureName = 9;
GLuint PlutoTextureName = 10;
GLuint RingTextureName = 11;


// Variables to keep track of current day status.
GLfloat CurrentEarthRotation = 0.00;
GLfloat EarthDaysTranspired  = 0.00;
GLfloat EarthDayIncrement    = 0.01;

// The initial window and viewport sizes (in pixels), set to ensure that
// the aspect ration for the viewport, will be a constant. If the window
// is resized, the viewport will be adjusted to preserve the aspect ratio.
GLint currWindowSize[2]   = { 750, 750/ASPECT_RATIO };
GLint currViewportSize[2] = { 750, 750/ASPECT_RATIO };

GLfloat ViewerDistance = INITIAL_VIEWER_DISTANCE;

ParticleSystem particles;
bool particleFlag = true;
bool blendFlag = true;


/***********************/
/* Function prototypes */
/***********************/
void KeyboardPress(unsigned char pressedKey, int mouseXPosition, int mouseYPosition);
void NonASCIIKeyboardPress(int pressedKey, int mouseXPosition, int mouseYPosition);
void TimerFunction(int value);
void Display();

void MakeAllImages();
void MakeImage(const char bitmapFilename[], GLuint &textureName, bool hasAlpha);

void ResizeWindow(GLsizei w, GLsizei h);
void drag(int x, int y);

void drawEarthAndMoon();
void drawSun();
void drawSaturnRing();
void drawAllPlanets();
void drawPlanet(GLfloat inclination, GLfloat orbitDuration,	GLfloat orbitRadius, GLfloat rotationDuration, GLuint texturename, GLfloat radius);
void drawParticle(Particle currParticle);
void drawAllParticles();

void drawOrbit(double x, double y, double z, double r, int num_segments);
void drawMoon(GLfloat inclination, GLfloat orbitDuration, GLfloat planetOrbitDuration, GLfloat orbitRadius, GLfloat planetOrbitRadius, GLfloat rotationDuration, GLuint texturename, GLfloat radius);

/****************************/
/* Function implementations */
/****************************/

// The main function sets up the data and the   //
// environment to display the textured objects. //
int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	// Set up the display window.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH);
	glutInitWindowPosition(INIT_WINDOW_POSITION[0], INIT_WINDOW_POSITION[1]);
	glutInitWindowSize(currWindowSize[0], currWindowSize[1]);
	glutCreateWindow("Solar System");

	// Specify the resizing and refreshing routines.
	glutReshapeFunc(ResizeWindow);
	glutKeyboardFunc(KeyboardPress);
	glutSpecialFunc(NonASCIIKeyboardPress);
	glutDisplayFunc(Display);
	glutTimerFunc(20, TimerFunction, 1);
	glViewport(0, 0, currWindowSize[0], currWindowSize[1]);
	
	glutMotionFunc(drag);

	// Set up standard lighting, shading, and depth testing.
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_NORMALIZE);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Set up all texture maps and texture-mapped objects.
	MakeAllImages();
	glutMainLoop();
	return 0;
}




//++++++++++++++++++++++++++++++++++



// points structure made of two coordinates; x and y
struct Points
{
	float x, y;  // initializor
	Points() { x = 0.0; y = 0.0; } // constructor

	Points(float _x, float _y) : x(_x), y(_y) {}
};

// square made of 4 points
class Square
{
public:
	Points pts[4]; // square structure
	Square(); // initialize constructor

	void draw(Square* sqr); // draw square
	Points mouse(int x, int y); // get mouse coordintaes
	Square* drag(Square* sqr, Points* mouse); // change points of sqr
};

// square constructor
Square::Square()
{
	pts[0] = Points(0.2, 0.2);
	pts[1] = Points(0.4, 0.2);
	pts[2] = Points(0.4, 0.4);
	pts[3] = Points(0.2, 0.4);
};

// draw function
void Square::draw(Square* sqr)
{
	// draw square fill
	int i;
	glColor3f(0.2, 0.2, 0.2);
	glBegin(GL_QUADS);
	for (i = 0; i < 4; ++i)
	{
		glVertex3f(sqr->pts[i].x, 0, sqr->pts[i].y);
	}
	glEnd();
	// draw square points
	i = 0;

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POINTS);
	for (i = 0; i < 4; ++i)
	{
		glVertex3f(sqr->pts[i].x, 0, sqr->pts[i].y);
	}
	glEnd();
}

// mouse function
Points Square::mouse(int x, int y)
{
	int windowWidth = 400, windowHeight = 400;
	return Points(float(x) / windowWidth, 1.0 - float(y) / windowHeight);
}

// drag function
Square* Square::drag(Square* sqr, Points* mouse)
{
	sqr->pts[0].x = mouse->x - 0.1;
	sqr->pts[0].y = mouse->y - 0.1;
	sqr->pts[1].x = mouse->x + 0.1;
	sqr->pts[1].y = mouse->y - 0.1;

	sqr->pts[3].x = mouse->x - 0.1;
	sqr->pts[3].y = mouse->y + 0.1;

	sqr->pts[2].x = mouse->x + 0.1;
	sqr->pts[2].y = mouse->y + 0.1;

	return sqr;
}

// GLOBAL

// create square object
Square* sqr = new Square;

void drag(int x, int y)
{
	// int x and y of mouse converts to screen coordinates
	// returns the point as mousePt
	Points mousePt = sqr->mouse(x, y);
	//create pointer to window point coordinates
	Points* mouse = &mousePt;

	// if the mouse is within the square
	if (mouse->x > sqr->pts[0].x && mouse->y > sqr->pts[0].y)
	{
		if (mouse->x < sqr->pts[2].x && mouse->y < sqr->pts[2].y)
		{
			// then drag by chaning square coordinates relative to mouse
			sqr->drag(sqr, mouse);
			glutPostRedisplay();
		}
	}
}


//++++++++++++++++++++++++++++++++++




// Function to react to ASCII keyboard keys pressed by the user.   //
// +/- keys are used to accelerate/decelerate the animation, while //
// the z/Z keys are used to zoom in and out of the animation.      //
void KeyboardPress(unsigned char pressedKey, int mouseXPosition, int mouseYPosition)
{
	char pressedChar = char(pressedKey);
	switch (pressedKey)
	{
	case '=': {
		EarthDayIncrement *= 2.0;
		if (EarthDayIncrement > 10.0)
			EarthDayIncrement = 10.0;
		break;
	}
	case '-': {
		EarthDayIncrement *= 0.5;
		if (EarthDayIncrement < 0.01)
			EarthDayIncrement = 0.01;
		break;
	}
	case 'z': {
		ViewerDistance -= VIEWER_DISTANCE_INCREMENT;
		if (ViewerDistance < MINIMUM_VIEWER_DISTANCE)
			ViewerDistance = MINIMUM_VIEWER_DISTANCE;
		break;
	}
	case 'Z': {
		ViewerDistance += VIEWER_DISTANCE_INCREMENT;
		if (ViewerDistance > MAXIMUM_VIEWER_DISTANCE)
			ViewerDistance = MAXIMUM_VIEWER_DISTANCE;
		break;
	}
			//enable and disable the particle system
	case 'p': {
		particleFlag = !particleFlag;
		break;
	}
			//enable and disable the blending function
	case 'b': {
		blendFlag = !blendFlag;
		break;
	}
	}
}

// Function to react to non-ASCII keyboard keys pressed by the user. //
// Used to alter spherical coordinates of the viewer's position.     //
void NonASCIIKeyboardPress(int pressedKey, int mouseXPosition, int mouseYPosition)
{
	glutIgnoreKeyRepeat(false);

	if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
	{
		switch (pressedKey)
		{
		case GLUT_KEY_RIGHT:
			LOOK_AT_POSITION[0] += 0.1;
			break;
		case GLUT_KEY_LEFT:
			LOOK_AT_POSITION[0] -= 0.1;
			break;
		case GLUT_KEY_UP:
			LOOK_AT_POSITION[2] += 0.1;
			break;
		case GLUT_KEY_DOWN:
			LOOK_AT_POSITION[2] -= 0.1;
			break;
		}
	}
	else
	{
		switch (pressedKey)
		{
		case GLUT_KEY_RIGHT:
			viewerAzimuth += VIEWER_ANGLE_INCREMENT;
			if (viewerAzimuth > 2 * PI) viewerAzimuth -= 2 * PI;
			break;
		case GLUT_KEY_LEFT:
			viewerAzimuth -= VIEWER_ANGLE_INCREMENT;
			if (viewerAzimuth < 0.0) viewerAzimuth += 2 * PI;
			break;
		case GLUT_KEY_UP:
			viewerZenith -= VIEWER_ANGLE_INCREMENT;
			if (viewerZenith < VIEWER_ANGLE_INCREMENT) viewerZenith = VIEWER_ANGLE_INCREMENT;
			break;
		case GLUT_KEY_DOWN:
			viewerZenith += VIEWER_ANGLE_INCREMENT;
			if (viewerZenith > PI - VIEWER_ANGLE_INCREMENT) viewerZenith = PI - VIEWER_ANGLE_INCREMENT;
			break;
		}
	}
}

// The EarthDayIncrement represents the fraction of an      //
// Earth day being added to the scene in one screen update. //
void TimerFunction(int value)
{
	CurrentEarthRotation += EarthDayIncrement;
	EarthDaysTranspired += EarthDayIncrement;
	if (EarthDaysTranspired == EARTH_ORBIT_DUR)
		EarthDaysTranspired = 0;
	glutPostRedisplay();
	glutTimerFunc(20, TimerFunction, 1);
}

// Principal display routine: sets up material, lighting, //
// and camera properties, clears the frame buffer, and    //
// draws all texture-mapped objects within the window.    //
void Display()
{
	// Set up the properties of the viewing camera.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	sqr->draw(sqr);

	// Position and orient viewer.
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
			  LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith), 
			  LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
			  LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
			  0.0, 1.0, 0.020);

	// Render scene.
	drawSun();
	drawMoon(EARTH_INCLINATION, LUNAR_CYCLE, EARTH_ORBIT_DUR, MOON_ORBIT_RADIUS, EARTH_ORBIT_RADIUS, LUNAR_CYCLE, MoonTextureName, MOON_RADIUS);
	drawAllPlanets();
	drawSaturnRing();
	

	if(blendFlag){ //if the user has selected blending, enable it.
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_COLOR,GL_ONE);
	}
	if(particleFlag)
		drawAllParticles();

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glutSwapBuffers();
	glFlush();
}

// Create the textures associated with all texture-mapped objects being displayed. //
void MakeAllImages()
{
	MakeImage(EARTH_BMP_FILENAME, EarthTextureName, false);
	MakeImage(MOON_BMP_FILENAME,  MoonTextureName,  false);
	MakeImage(SUN_BMP_FILENAME,		SunTextureName,		false);
	MakeImage(MERCURY_BMP_FILENAME, MercuryTextureName, false);
	MakeImage(VENUS_BMP_FILENAME, VenusTextureName, false);
	MakeImage(PARTICLE_BMP_FILENAME, ParticleTextureName, false);
	MakeImage(MARS_BMP_FILENAME, MarsTextureName, false);
	MakeImage(JUPITER_BMP_FILENAME, JupiterTextureName, false);
	MakeImage(SATURN_BMP_FILENAME, SaturnTextureName, false);
	MakeImage(URANUS_BMP_FILENAME, UranusTextureName, false);
	MakeImage(NEPTUNE_BMP_FILENAME, NeptuneTextureName, false);
	MakeImage(PLUTO_BMP_FILENAME, PlutoTextureName, false);
	MakeImage(RING_BMP_FILENAME, RingTextureName, false);
}

//makes calls to the generic planet drawing function. took this out
//of the display function to enhance readability
void drawAllPlanets(){
	drawPlanet(MERCURY_INCLINATION, MERCURY_ORBIT_DUR, MERCURY_ORBIT_RADIUS, MERCURY_ROTATION_DUR, MercuryTextureName, MERCURY_RADIUS);
	drawPlanet(VENUS_INCLINATION, VENUS_ORBIT_DUR, VENUS_ORBIT_RADIUS, VENUS_ROTATION_DUR, VenusTextureName, VENUS_RADIUS);
	drawPlanet(EARTH_INCLINATION, EARTH_ORBIT_DUR, EARTH_ORBIT_RADIUS, EARTH_ROTATION, EarthTextureName, EARTH_RADIUS);
	drawPlanet(MARS_INCLINATION, MARS_ORBIT_DUR, MARS_ORBIT_RADIUS, MARS_ROTATION_DUR, MarsTextureName, MARS_RADIUS);
	drawPlanet(JUPITER_INCLINATION, JUPITER_ORBIT_DUR, JUPITER_ORBIT_RADIUS, JUPITER_ROTATION_DUR, JupiterTextureName, JUPITER_RADIUS);
	drawPlanet(SATURN_INCLINATION, SATURN_ORBIT_DUR, SATURN_ORBIT_RADIUS, SATURN_ROTATION_DUR, SaturnTextureName, SATURN_RADIUS);
	drawPlanet(URANUS_INCLINATION, URANUS_ORBIT_DUR, URANUS_ORBIT_RADIUS, URANUS_ROTATION_DUR, UranusTextureName, URANUS_RADIUS);
	drawPlanet(NEPTUNE_INCLINATION, NEPTUNE_ORBIT_DUR, NEPTUNE_ORBIT_RADIUS,	NEPTUNE_ROTATION_DUR, NeptuneTextureName, NEPTUNE_RADIUS);
	drawPlanet(PLUTO_INCLINATION, PLUTO_ORBIT_DUR, PLUTO_ORBIT_RADIUS, PLUTO_ROTATION_DUR, PlutoTextureName, PLUTO_RADIUS);
}

// Convert the bitmap with the parameterized name into an OpenGL texture. //
void MakeImage(const char bitmapFilename[], GLuint &textureName, bool hasAlpha)
{
	RGBpixmap pix;
	pix.readBMPFile(bitmapFilename, hasAlpha);
	pix.setTexture(textureName);
}

// Window-reshaping callback, adjusting the viewport to be as large  //
// as possible within the window, without changing its aspect ratio. //
void ResizeWindow(GLsizei w, GLsizei h)
{
	currWindowSize[0] = w;
	currWindowSize[1] = h;
	if (ASPECT_RATIO > w/h)
	{
		currViewportSize[0] = w;
		currViewportSize[1] = w / ASPECT_RATIO;
	}
	else
	{
		currViewportSize[1] = h;
		currViewportSize[0] = h * ASPECT_RATIO;
	}

	glViewport(0.5*(w-currViewportSize[0]), 0.5*(h-currViewportSize[1]), currViewportSize[0], currViewportSize[1]);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



//Function to draw and texture map the sun at the origin.  The sun acts as the light source
void drawSun()
{
	const GLfloat LIGHT_AMBIENT[] = { 0.2, 0.2, 0.2, 1.0 };
	const GLfloat LIGHT_DIFFUSE[] = { 1.0, 1.0, 1.0, 1.0 };
	const GLfloat LIGHT_SPECULAR[] = { 1.0, 1.0, 1.0, 1.0 };
	const GLfloat LIGHT_MODEL_AMBIENT[] = { 0.01, 0.01, 0.01, 1.0 };
	const GLfloat LIGHT_POSITION[] = { 0.0, 0.0, 0.0, 1.0 }; //Use XYZ coordinates to place light in 3D origin - inside the sun
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LIGHT_MODEL_AMBIENT);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LIGHT_AMBIENT);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LIGHT_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LIGHT_SPECULAR);
	glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POSITION);
	glEnable(GL_LIGHT0);

	GLUquadricObj* quadro = gluNewQuadric();
	gluQuadricNormals(quadro, GLU_SMOOTH);
	gluQuadricTexture(quadro, GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, SunTextureName);
	gluSphere(quadro, SUN_RADIUS, 48, 48);
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(quadro);
}


//Draws the texture-mapped earth and moon. //
void drawMoon(GLfloat inclination, GLfloat orbitDuration, GLfloat planetOrbitDuration, GLfloat orbitRadius, GLfloat planetOrbitRadius, GLfloat rotationDuration, GLuint texturename, GLfloat radius)
{
	GLUquadricObj* quadro = gluNewQuadric();							
	gluQuadricNormals(quadro, GLU_SMOOTH);		
	gluQuadricTexture(quadro, GL_TRUE);			
	glEnable(GL_TEXTURE_2D);
		glPushMatrix();
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glRotatef(inclination, 0.0, 0.0, 1.0);
			glRotatef( 360.0 * (EarthDaysTranspired/planetOrbitDuration), 0.0, 1.0, 0.0);
			glTranslatef(planetOrbitRadius, 0.0, 0.0 );
			glRotatef( 360.0 * EarthDaysTranspired / rotationDuration, 0.0, 1.0, 0.0 );
			glTranslatef(orbitRadius, 0.0, 0.0 );
			glBindTexture(GL_TEXTURE_2D, MoonTextureName);
			gluSphere(quadro, radius, 48, 48);
		glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(quadro);
}


//Draw Rings
void drawSaturnRing()
{
	GLUquadricObj* quadro = gluNewQuadric();							
	gluQuadricNormals(quadro, GLU_SMOOTH);		
	gluQuadricTexture(quadro, GL_TRUE);			
	glEnable(GL_TEXTURE_2D);
		glPushMatrix();
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glPushMatrix();
				glRotatef(SATURN_INCLINATION, 0.0, 0.0, 1.0);
				glRotatef( 360.0 * (EarthDaysTranspired/SATURN_ORBIT_DUR), 0.0, 1.0, 0.0);
				glTranslatef(SATURN_ORBIT_RADIUS, 0.0, 0.0 );
				glRotatef( -90.0, 1.0, 0.0, 0.0 );
				glBindTexture(GL_TEXTURE_2D, RingTextureName);
				glScalef(1,1,.02);
				gluSphere(quadro, SATURN_RADIUS*2, 48, 48);
			glPopMatrix();
		glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(quadro);
}

void drawOrbit(double x, double y, double z, double r, int num_segments)
{
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1.0f, 1.0f, 1.0f);


	GLUquadricObj* quadro = gluNewQuadric();
	gluQuadricNormals(quadro, GLU_SMOOTH);
	gluQuadricTexture(quadro, GL_TRUE);
	glPushMatrix();
		glTranslatef(0.0, 0.0, 0.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		gluDisk(quadro, r - 0.01, r + 0.01, num_segments, 1);
	glPopMatrix();
	gluDeleteQuadric(quadro);
	glDisable(GL_COLOR_MATERIAL);
}


//Given parameters about the planets dimension, orbit, radius etc, this function
//will draw a texture mapped plant.
//it is used to draw everything except the sun, earth/moon. and saturns rings, as
//they are special cases of this function
void drawPlanet(GLfloat inclination, GLfloat orbitDuration, GLfloat orbitRadius, GLfloat rotationDuration, GLuint texturename, GLfloat radius)
{
	GLUquadricObj* quadro = gluNewQuadric();							
	gluQuadricNormals(quadro, GLU_SMOOTH);		
	gluQuadricTexture(quadro, GL_TRUE);			
	glEnable(GL_TEXTURE_2D);
		glPushMatrix();
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glPushMatrix();
				glRotatef( inclination, 0.0, 0.0, 1.0);
				glRotatef( 360.0 * (EarthDaysTranspired/orbitDuration), 0.0, 1.0, 0.0);
				glTranslatef(orbitRadius, 0.0, 0.0 );
				glRotatef( 360.0 * (CurrentEarthRotation/rotationDuration), 0.0, 1.0, 0.0 );
				glRotatef( -90.0, 1.0, 0.0, 0.0 );
				glBindTexture(GL_TEXTURE_2D, texturename);
				gluSphere(quadro, radius, 48, 48);
			glPopMatrix();
		glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(quadro);

	drawOrbit(0.0, 0.0, 0.0, orbitRadius, 64);
}

//Cycles through each particle in the particle system and passes it to the
//draw function
void drawAllParticles(){
	particles.updateAll();
	for(int i = 0; i < particles.getNumberOfParticles(); i++)
		drawParticle(particles.getNextParticle());

}

//Draws one individual particle, given a particle struct from the particle
//system object
void drawParticle(Particle currParticle)
{	
	glEnable(GL_TEXTURE_2D);
		glPushMatrix();
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glRotatef(currParticle.azimuthRoation , 0, 1, 0);
			glRotatef( currParticle.zenithRotation ,0,0,1);
			glTranslatef(SUN_RADIUS + currParticle.surfaceTranslationFactor, 0 ,0);
			glRotatef(90, 0 , 1, 0);
			glScalef( .5, .5, 1.0 );
			glBindTexture(GL_TEXTURE_2D, ParticleTextureName);

			// Logo Facing Earth
			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2d(1,1); 
					glVertex3f(0.5f, 0.5f, 0.0f); // Top Right
				glTexCoord2d(0,1);
					glVertex3f(-0.5f, 0.5f, 0.0f); // Top Left
				glTexCoord2d(1,0); 
					glVertex3f(0.5f, -0.5f, 0.0f); // Bottom Right
				glTexCoord2d(0,0); 
					glVertex3f(-0.5f, -0.5f, 0.0f); // Bottom Left
			glEnd();

			// Logo Facing Away From Earth
			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2d(1,1); 
					glVertex3f(-0.5f, 0.5f, 0.0f); // Top Right
				glTexCoord2d(0,1);
					glVertex3f(0.5f, 0.5f, 0.0f); // Top Left
				glTexCoord2d(1,0); 
					glVertex3f(-0.5f, -0.5f, 0.0f); // Bottom Right
				glTexCoord2d(0,0); 
					glVertex3f(0.5f, -0.5f, 0.0f); // Bottom Left
			glEnd();
		glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}


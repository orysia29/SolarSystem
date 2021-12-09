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

void drawSun();
void drawSaturnRing();
void drawPlanet(GLfloat inclination, GLfloat orbitDuration,	GLfloat orbitRadius, GLfloat rotationDuration, GLuint texturename, GLfloat radius);
void drawParticle(Particle currParticle);
void drawAllParticles();
void drawOrbit(double x, double y, double z, double r, int num_segments);
void drawMoon(GLfloat inclination, GLfloat orbitDuration, GLfloat planetOrbitDuration, GLfloat orbitRadius, GLfloat planetOrbitRadius, GLfloat rotationDuration, GLuint texturename, GLfloat radius);

/****************************/
/* Function implementations */
/****************************/

// The main function from the github project sets up the GLUT/OpenGL environment as well as the window
int main(int argc, char** argv)
{
	//Initialize the glut API
	glutInit(&argc, argv);

	//glut API wraps OpenGL and provides an easier interface

	// Set up the display window.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH);
	glutInitWindowPosition(INIT_WINDOW_POSITION[0], INIT_WINDOW_POSITION[1]);
	glutInitWindowSize(currWindowSize[0], currWindowSize[1]);
	glutCreateWindow("Solar System");

	//GLUT provides function hooks and you give it your functions to use.

	//Provide functions to GLUT/OpenGL
	glutReshapeFunc(ResizeWindow);           //Update scene when window is resized
	glutKeyboardFunc(KeyboardPress);         //Process keyboard input for zooming and controlling animations
	glutSpecialFunc(NonASCIIKeyboardPress);  //Process keyboard input for moving the camera
	glutDisplayFunc(Display);                //Display function updates the scene
	glutTimerFunc(20, TimerFunction, 1);     //Set Animation timer
	glViewport(0, 0, currWindowSize[0], currWindowSize[1]);  //Where camera is viewing
	
	//GLUT uses a state machine to track all current settings to be rendered.  

	// Set up standard lighting, shading, and depth testing.
	glShadeModel(GL_SMOOTH);  //Sets shader type
	glEnable(GL_DEPTH_TEST);  //Begin tracking Depth edits
	glDepthFunc(GL_LEQUAL);	  //set depth buffer
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //smooths camera movement
	glEnable(GL_NORMALIZE);   //begin tracking normalization
	glCullFace(GL_BACK);      //cull the opposite face of objects
	glEnable(GL_CULL_FACE);   //tracking culling edits
	glClearColor(0.0, 0.0, 0.0, 0.0);        //reset material colors
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);   //sets pixel storage mode

	MakeAllImages();  //Get the texture images
	glutMainLoop();   //Main graphics render loop
	return 0;
}


// === UPDATE ADDED ===================================================================================

//++++++++++++++++++++++++++++++++++

//This is our attempt to add camera control based on mouse movement
//Currently does not work



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

// === End UPDATE ===================================================================================


//Function from github to process keyboard inputs
// +/- keys will speed up/slow down the animation
// the z/Z keys are used to zoom in and out 
void KeyboardPress(unsigned char pressedKey, int mouseXPosition, int mouseYPosition)
{
	char pressedChar = char(pressedKey); //Get the key that was pressed
	switch (pressedKey) //Check the key
	{
	case '=': { //speed up simulation
		EarthDayIncrement *= 2.0; //Increases simulation step increment
		if (EarthDayIncrement > 10.0)
			EarthDayIncrement = 10.0;
		break;
	}
	case '-': { //slow down simulation
		EarthDayIncrement *= 0.5; //decreases simulation step increment
		if (EarthDayIncrement < 0.01)
			EarthDayIncrement = 0.01;
		break;
	}
	case 'z': { //zoom in camera
		ViewerDistance -= VIEWER_DISTANCE_INCREMENT; //reduce camera distance
		if (ViewerDistance < MINIMUM_VIEWER_DISTANCE) //limit zoom in
			ViewerDistance = MINIMUM_VIEWER_DISTANCE;
		break;
	}
	case 'Z': { //zoom out camera
		ViewerDistance += VIEWER_DISTANCE_INCREMENT; //increase camera distance
		if (ViewerDistance > MAXIMUM_VIEWER_DISTANCE) //limit zoom out
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

//Function from github to process more keyboard inputs
// Used to modify the camera x,y,z position for translation and rotation movement.
void NonASCIIKeyboardPress(int pressedKey, int mouseXPosition, int mouseYPosition)
{
	glutIgnoreKeyRepeat(false);

// === UPDATED ADDED ===================================================================================
	if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
	{
		//Pan the camera - new function that was added
		switch (pressedKey)
		{
		case GLUT_KEY_RIGHT:              
			LOOK_AT_POSITION[0] += 0.1; //increment x-axis position
			break;
		case GLUT_KEY_LEFT:
			LOOK_AT_POSITION[0] -= 0.1; //decrement x-axis position
			break;
		case GLUT_KEY_UP:
			LOOK_AT_POSITION[2] += 0.1; //increment z-axis position
			break;
		case GLUT_KEY_DOWN:
			LOOK_AT_POSITION[2] -= 0.1; //decrement z-axis position
			break;
		}
	}
// === END UPDATE ===================================================================================
	else
	{
		//Rotate the camera
		switch (pressedKey)
		{
		case GLUT_KEY_RIGHT:                      
			viewerAzimuth += VIEWER_ANGLE_INCREMENT; //clockwise rotation
			if (viewerAzimuth > 2 * PI) viewerAzimuth -= 2 * PI;
			break;
		case GLUT_KEY_LEFT:
			viewerAzimuth -= VIEWER_ANGLE_INCREMENT; //counter clockwise rotation
			if (viewerAzimuth < 0.0) viewerAzimuth += 2 * PI;
			break;
		case GLUT_KEY_UP:
			viewerZenith -= VIEWER_ANGLE_INCREMENT; //rotate up
			if (viewerZenith < VIEWER_ANGLE_INCREMENT) viewerZenith = VIEWER_ANGLE_INCREMENT;
			break;
		case GLUT_KEY_DOWN:
			viewerZenith += VIEWER_ANGLE_INCREMENT; //rotate down
			if (viewerZenith > PI - VIEWER_ANGLE_INCREMENT) viewerZenith = PI - VIEWER_ANGLE_INCREMENT;
			break;
		}
	}
}

//Function for simulating time based on Earth and used to update positions of planets for the animations
//The GLUT render loop calls this function for incrementing the simulation
void TimerFunction(int value)
{
	CurrentEarthRotation += EarthDayIncrement;
	EarthDaysTranspired += EarthDayIncrement;
	if (EarthDaysTranspired == EARTH_ORBIT_DUR)
		EarthDaysTranspired = 0;
	glutPostRedisplay();
	glutTimerFunc(20, TimerFunction, 1);
}

// Important function
// This is main display function: it sets up materials for objects, lighting and camera position.
// Also sets up for drawing maps, textures and objects (sun, planets, moon, etc) within the window.
// Basically, draws everything
void Display()
{
	// Sets up the view matricies
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Calculate camera position
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
			  LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith), 
			  LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
			  LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
			  0.0, 1.0, 0.020);

	// Draw the scene - draw each object
	drawSun();
	drawPlanet(MERCURY_INCLINATION, MERCURY_ORBIT_DUR, MERCURY_ORBIT_RADIUS, MERCURY_ROTATION_DUR, MercuryTextureName, MERCURY_RADIUS);
	drawPlanet(VENUS_INCLINATION, VENUS_ORBIT_DUR, VENUS_ORBIT_RADIUS, VENUS_ROTATION_DUR, VenusTextureName, VENUS_RADIUS);
	drawPlanet(EARTH_INCLINATION, EARTH_ORBIT_DUR, EARTH_ORBIT_RADIUS, EARTH_ROTATION, EarthTextureName, EARTH_RADIUS);
	drawPlanet(MARS_INCLINATION, MARS_ORBIT_DUR, MARS_ORBIT_RADIUS, MARS_ROTATION_DUR, MarsTextureName, MARS_RADIUS);
	drawPlanet(JUPITER_INCLINATION, JUPITER_ORBIT_DUR, JUPITER_ORBIT_RADIUS, JUPITER_ROTATION_DUR, JupiterTextureName, JUPITER_RADIUS);
	drawPlanet(SATURN_INCLINATION, SATURN_ORBIT_DUR, SATURN_ORBIT_RADIUS, SATURN_ROTATION_DUR, SaturnTextureName, SATURN_RADIUS);
	drawPlanet(URANUS_INCLINATION, URANUS_ORBIT_DUR, URANUS_ORBIT_RADIUS, URANUS_ROTATION_DUR, UranusTextureName, URANUS_RADIUS);
	drawPlanet(NEPTUNE_INCLINATION, NEPTUNE_ORBIT_DUR, NEPTUNE_ORBIT_RADIUS, NEPTUNE_ROTATION_DUR, NeptuneTextureName, NEPTUNE_RADIUS);
	drawPlanet(PLUTO_INCLINATION, PLUTO_ORBIT_DUR, PLUTO_ORBIT_RADIUS, PLUTO_ROTATION_DUR, PlutoTextureName, PLUTO_RADIUS);
	drawMoon(EARTH_INCLINATION, LUNAR_CYCLE, EARTH_ORBIT_DUR, MOON_ORBIT_RADIUS, EARTH_ORBIT_RADIUS, LUNAR_CYCLE, MoonTextureName, MOON_RADIUS);
	drawSaturnRing();
	
	//Calculate particles animation
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

//Function from github to read in the bitmap image files to create the textures for each of the planets
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

// Function from github to convert the bitmap to a texture. Used to help the function above
void MakeImage(const char bitmapFilename[], GLuint &textureName, bool hasAlpha)
{
	RGBpixmap pix;
	pix.readBMPFile(bitmapFilename, hasAlpha);
	pix.setTexture(textureName);
}

//Function from github to re-draw the graphics when the window size is changed
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
	// === UPDATE ADDED ===================================================================================

	//Removed 

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

	// === END UPDATE ===================================================================================

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

// === UPDATE ADDED ===================================================================================

//Function draws the moon.  Updated so we can draw any moon around any planet
//Function repeats the planet drawing function but takes planet info to draw the moon around the planet instead of the sun
void drawMoon(GLfloat inclination, GLfloat orbitDuration, GLfloat planetOrbitDuration, GLfloat orbitRadius, GLfloat planetOrbitRadius, GLfloat rotationDuration, GLuint texturename, GLfloat radius)
{
	GLUquadricObj* quadro = gluNewQuadric();							
	gluQuadricNormals(quadro, GLU_SMOOTH);		//Provides normal map for the moon
	gluQuadricTexture(quadro, GL_TRUE);			//Provides texture for the moon
	glEnable(GL_TEXTURE_2D);                    //Set OpenGL to accept texture edits
		glPushMatrix();                         //Set put drawing info on OpenGL's data stack
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);   //Set Texture environment
			
			//Rotate and translate moon position based on time passed
			glRotatef(inclination, 0.0, 0.0, 1.0);                         
			glRotatef( 360.0 * (EarthDaysTranspired/planetOrbitDuration), 0.0, 1.0, 0.0);
			glTranslatef(planetOrbitRadius, 0.0, 0.0 );
			glRotatef( 360.0 * EarthDaysTranspired / rotationDuration, 0.0, 1.0, 0.0 );
			glTranslatef(orbitRadius, 0.0, 0.0 );

			glBindTexture(GL_TEXTURE_2D, MoonTextureName); //Add texture image
			gluSphere(quadro, radius, 48, 48);             //Draw the moon
		glPopMatrix();                        //End this drawing
	glDisable(GL_TEXTURE_2D);                 //End current OpenGL texture edit
	gluDeleteQuadric(quadro);
}

// === END UPDATE ===================================================================================


//Function from github to draw a ring on saturn.  The ring is actually a flattened sphere.
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

// === UPDATE ADDED ===================================================================================

//Function draws the lines showing planet orbit
//It actually draws a disk with a large inner radius and is affected by light
void drawOrbit(double x, double y, double z, double r, int num_segments)
{
	glEnable(GL_COLOR_MATERIAL); //tells OpenGL to begin tracking changes to object material
	glColor3f(1.0f, 1.0f, 1.0f); //Set the color to white, it will still be affected by lighting

	GLUquadricObj* quadro = gluNewQuadric();
	gluQuadricNormals(quadro, GLU_SMOOTH); //Provide a normal map for the disk
	glPushMatrix();
		glTranslatef(0.0, 0.0, 0.0); //Put the center of the disk in the center of the scene
		glRotatef(-90.0, 1.0, 0.0, 0.0); //Rotate the disk so it appears horizontal
		gluDisk(quadro, r - 0.01, r + 0.01, num_segments, 1); //Draw the disk based on planet orbit radius
	glPopMatrix();
	gluDeleteQuadric(quadro);
	glDisable(GL_COLOR_MATERIAL);
}

// === END UPDATE ===================================================================================

//Modified function from github to include drawing orbit ring
//Given parameters about the planets dimension, orbit, radius etc, this function will draw a textured planet.
//it is used to draw everything except the sun and saturns rings. 
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

//Functions from github to draw particles

//Cycles through each particle in the particle system and passes it to the draw function
void drawAllParticles(){
	particles.updateAll();
	for(int i = 0; i < particles.getNumberOfParticles(); i++)
		drawParticle(particles.getNextParticle());
}

//Draws one individual particle, given a particle struct from the particle system object
//Provides the particle animation effect
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


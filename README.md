# Solar System Project 👩‍🚀 🪐
## Authors

- Orysia Popowych
- Osida Richards
## Acknowledgements

Code from several similar projects on github were used.
- [acoffman/solar-system-opengl](https://github.com/acoffman/solar-system-opengl)
- [1kar/OpenGL-SolarSystem](https://github.com/1kar/OpenGL-SolarSystem)
- [23sent/SolarSystem-OpenGl](https://github.com/23sent/SolarSystem-OpenGl)
- [rrozema12/SolarSystem](https://github.com/rrozema12/SolarSystem)
## Project Files

* **SolarSystem.cpp** - the main code file - has all the important functions - the other files don't matter
* **SolarSystemGlobals.h** - has constant numbers used to represent the planets
* **RGBpixmap.cpp** - unmodified from github project - has functions for reading image file as a texture
* **RGBpixmap.h** - unmodified from github project - has functions for reading image file as a texture
* **ParticleSystem.h** - unmodified from github project - has functions for updating the particle effects
## About The Project

We started off looking at tutorials and other github projects to try and understand opengl and how to use it. We had a lot of trouble starting off so we ended up looking at an existing github project in particular and borrowing code and ideas from similar projects and made a few modifications. One modification was removing the lighting from the scene and turning the sun into a light source, the only light source. This can be seen in the drawSun function at line 450. This proved to be challenging because we didn't fully understand how OpenGL updates its materials for lighting. Making a lighting change with respect to the sun would also light up all the planets, removing all shading. We learned that OpenGl uses what is called a state machine to track its current rendering state so for example if you set a lighting parameter when the lighting state is enabled, it is going to set it for all objects drawn until you tell it to stop updating the lighting. In this case we made use of ambient, diffuse and specular lighting on all the planets. Given the light source, OpenGL performs all the calculations automatically to render the appropriate lighting as the position of the planet changes and the direction of the light source changes.

Drawing the graphics for the planets in opengl is very easy because GLUT provides a draw sphere function. We borrowed code from github to read in bitmap images for the textures which are fed into GLUT texture map function for each object. Each planet object is basically the same, just with different size, positions and texture images. We looked at how the planet is dawn and added some rings to show the path of each orbit in the drawOrbit function at line 542. Here we draw a disk with a very large inner radius to make is appear like a line. We had to change its material properties so it is not affected by lighting.

The rotation and orbit animation is not as complex as it seems. GLUT also provides a built-in render loop at the end of the main function line 115, and on each step the translations and rotations of each planet gets updated by a small increment that you can specify and then it gets re-drawn using the Translate and Rotate functions. The Display function at line 350 is very important in that is sets the camera position and draws all of the objects and runs the particle animation which is also taken care of by OpenGL.

We also modified the Camera movement by providing the ability to pan the camera as shown at line 290. Already existing movement rotates the view around the center with the arrow keys, but now when holding shift and using the arrows, the view is translated instead of rotated. There are also plus and minus controls for speeding up and slowing down the animation, and the z key zooms in and out.
## Functionalities

The project has several features that enable a user to manipulate aspects of the animation which are perspective, animation playback speed, and toggle the sun's particle effect on and off.

#### arrow keys
Using the arrow keys moves the users perspective in the direction of the respective arrow key used:
- The **up** arrow key tilts the users perspective up
- The **down** arrow key tilts the users perspective down
- The **left** arrow key shifts the users perspective to the left
- The **right** arrow key shifts the users perspective to the right

#### z & shift+z
- **z** : Zooms the users perspective closer to the sun
- **shift + z** : Zooms out the users perspective from the sun

#### + & -
The keys control the animations plaback speed.
- **-** : Holding or pressing the - key slows down the animation eventually stopping the animation
- **+** : Holding or pressing the + key speeds up the animation to a finite speed

#### p
Pressing this toggles the partical effect of the sun on or off
## Technologies

This project was written in **C++** using **Visual Studio** and uses **OpenGL graphics library with OpenGL Utility Toolkit (GLUT) API (Application Programmer Interface)**. 
OpenGL with GLUT was downloaded from [transmissionzero](https://www.transmissionzero.co.uk/software/freeglut-devel/)
//
//  OpenGL ProjectOpengl widget
//
#include "includes/projectOpengl.h"
#include <QtOpenGL>
#include <QMessageBox>
#include "includes/Fish.h"
#include "includes/Cube.h"
#include <math.h>
#include <QElapsedTimer>

#define MAX_FISH 100

//
//  Constructor
//
ProjectOpengl::ProjectOpengl(QWidget* parent)
    : QGLWidget(parent)
{
   init  = false;
   asp = 1;
   dim = 25;
   fov = 55;
   numFish = 10;     // default to 10
}

// 
// Change number of fish to be displayed
//
void ProjectOpengl::setNumberOfFish(int fishNum) {
   numFish = fishNum;
   
   // Request redisplay
   updateGL();
}

//
//  Initialize
//
void ProjectOpengl::initializeGL()
{
   startTimer.start();
   if (init) return;
   init = true;

   //  Load shaders
   // shader[0] = Background shader
   Shader(BACKGROUND_SHADER,"",":/shaders/underwater.frag");
   
   /*
   * Shader 1 - Fish texture
   */
   Shader(FISH_SHADER,"",":/shaders/fishTexture.frag");

	// Load Texture
   QPixmap rockTex(":/textures/rock.jpg");
   tex[0] = bindTexture(rockTex,GL_TEXTURE_2D);
   
   QPixmap noiseTex0(":/textures/noise0.png");
   tex[1] = bindTexture(noiseTex0,GL_TEXTURE_2D);
   
   QPixmap noiseTex1(":/textures/noise1.jpg");
   tex[2] = bindTexture(noiseTex1,GL_TEXTURE_2D);
   
   // Cube for cube
   background = new Cube();
   background->scale(200,200,1);
   background->translate(0,0,-50);

   //  Create fish array
   GLfloat x, y, z;
   for (int index = 0; index < MAX_FISH; index++) {
        Fish * fish = new Fish();
        fish->scale(1,1,1);
        
        // Pick random locations to place the fish
	    x = getRand(-50.0f, 50.0f);
	    z = getRand(-50.0f, 50.0f);
	    y = getRand(-50.0f, 50.0f);
	    
	    // Move fish to starting spot
	    fish->translate(x,y,z);
	    fishArray.push_back(fish);
    }
    
    //  Start 100 fps timer connected to upobdateGL
   timer.setInterval(10);
   connect(&timer,SIGNAL(timeout()),this,SLOT(updateGL()));
   timer.start();
   time.start();
}

//
//  Set projection when window is resized
//
void ProjectOpengl::resizeGL(int width, int height)
{
   //  Window aspect ration
   asp = height ? width / (float)height : 1;
   //  Viewport is whole screen
   glViewport(0,0,width,height);
   //  Set projection
   Projection();
}

//
//  Draw the window
//
void ProjectOpengl::paintGL()
{ 
      //  Clear screen and Z-buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);

   //  Set view
   glLoadIdentity();
   glTranslated(0,0,-2*dim);
   
   //  OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);
   //  Enable lighting
   glEnable(GL_LIGHTING);
   
   //  Apply background shader
   shader[BACKGROUND_SHADER].bind();
   
   //  Pass variables to BACKGROUND_SHADER
   QVector2D resolution(width(), height());
   shader[BACKGROUND_SHADER].setUniformValue("resolution",resolution);
   shader[BACKGROUND_SHADER].setUniformValue("RockTex", tex[0]);
   shader[BACKGROUND_SHADER].setUniformValue("NoiseTex0", tex[1]);
   shader[BACKGROUND_SHADER].setUniformValue("NoiseTex1", tex[2]);
   float t = 0.001*time.elapsed();
   shader[BACKGROUND_SHADER].setUniformValue("time", t);

   //  Draw background scene
   glPushMatrix();
   background->display();
   
   //  Release background shader
   //shader[BACKGROUND_SHADER].release();
   
   //  Apply fish scale shader
   shader[FISH_SHADER].bind();
   shader[FISH_SHADER].setUniformValue("resolution",resolution);
   // Draw fish
   for (int index = 0; index < numFish; index++) {
        fishArray[index]->display();
   }
   
   //  Release shader
   shader[BACKGROUND_SHADER].release();
   
   //  Release scale shader
   shader[FISH_SHADER].release();
   glPopMatrix();
   
   glDisable(GL_LIGHTING);
   glDisable(GL_DEPTH_TEST);
}

//
//  Throw a fatal error and die
//
void ProjectOpengl::Fatal(QString message)
{
   QMessageBox::critical(this,"ProjectOpengl",message);
   QApplication::quit();
}

//
//  Set OpenGL projection
//
void ProjectOpengl::Projection()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   float zmin = dim/4;
   float zmax = 4*dim;
   float ydim = zmin*tan(fov*M_PI/360);
   float xdim = ydim*asp;
   glFrustum(-xdim,+xdim,-ydim,+ydim,zmin,zmax);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


//
//  Load shader
//
void ProjectOpengl::Shader(int k,QString vert,QString frag)
{
   //  Vertex shader
   if (vert.length() && !shader[k].addShaderFromSourceFile(QGLShader::Vertex,vert))
      Fatal("Error compiling "+vert+"\n"+shader[k].log());
   //  Fragment shader
   if (frag.length() && !shader[k].addShaderFromSourceFile(QGLShader::Fragment,frag))
      Fatal("Error compiling "+frag+"\n"+shader[k].log());
   //  Link
   if (!shader[k].link())
      Fatal("Error linking shader\n"+shader[k].log());
}

GLfloat ProjectOpengl::getRand(GLfloat minimum, GLfloat range)
{
	return (((GLfloat)rand() / (GLfloat)RAND_MAX) * range) + minimum;
}

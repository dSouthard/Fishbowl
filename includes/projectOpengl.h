//
//  OpenGL projectOpengl Widget
//

#ifndef PROJECTOPENGL_H
#define PROJECTOPENGL_H

#include <QtOpenGL>
#include <QString>
#include <QVector>
#include "includes/Object.h"
#include "includes/Fish.h"
#include "includes/Cube.h"

#define NUMBER_OF_TEXTURES 3
#define NUMBER_OF_SHADERS 2
#define BACKGROUND_SHADER 0
#define FISH_SHADER 1

class ProjectOpengl : public QGLWidget
{
Q_OBJECT
private:
   bool    init;      //  Initialized
   int     numFish;   //  Number of fish to be displayed, default = 1
   QPoint  pos;       //  Mouse position
   int     fov;       //  Field of view
   double  dim;       //  Display size
   double  asp;       //  Sceen aspect ratio
   Cube* background;        //  Quad for background shader
   unsigned int tex[NUMBER_OF_TEXTURES];  //  Textures
   QElapsedTimer startTimer;   //  Time since program started
   QGLShaderProgram shader[NUMBER_OF_SHADERS]; //  Shaders
   QVector<Fish*> fishArray;   //  Fish Objects
   QTimer           timer;     //  Timer for animations
   QElapsedTimer    time;      //  Track elapsed time
public:
   ProjectOpengl(QWidget* parent=0);               //  Constructor
   QSize sizeHint() const {return QSize(1000,1000);} //  Default size of widget
public slots:
    void setNumberOfFish(int fishNum);     //  Slot to change number of fish
signals:
protected:
    void initializeGL();                   //  Initialize widget
    void resizeGL(int width, int height);  //  Resize widget
    void paintGL();                        //  Draw widget
private:
   void Fatal(QString message);            //  Error handler
   void Projection();                      //  Update projection
   void Shader(int k,QString vert,QString frag);  //  Create shader
   GLfloat getRand(GLfloat minimum, GLfloat range); // Return random number
};

#endif

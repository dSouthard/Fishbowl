/*
 * Fish class. Draws a fish.
 */

#include "includes/Fish.h"
#include <math.h>

#define MAX_AREA 70

using namespace std;

// setup the static variables
GLfloat Fish::material[4] = {1.f, 1.f, 1.f, 1.f};
GLfloat Fish::shininess = 120.f;

Fish::Fish() :
Object() {
    Sx = 0;
    Sy = 0;
    Sz = 0;
    Th = 0;
    // angles and cut offs for tail animation
    tailAngle = 0.0f;
    tailAngleCutOff = 20.0f;
    tailAngleInc = 1.0f;
}

//
//  Set rotation
//

void Fish::rotate(float th) {
    Th = th;
}

//
//  Set scaling
//

void Fish::scale(float dx, float dy, float dz) {
    Sx = dx;
    Sy = dy;
    Sz = dz;
}


/// Draws the full fish

void Fish::display(void) {
	
// work out how much to advance the fish by relative to its orientation
    GLfloat xInc;
    xInc = cos(x0 * (3.14156 ) / 180) ;

    // the floor is 70.0 x 70.0, but i want to keep the fish inside a
    // 65.0 x 65.0 area, so work out the circular boundaries if the fish goes
    // outside of this area
    if (x0 < -MAX_AREA) x0 += 150.f;

    // increment the fish position
    x0 -= (xInc);
    //z0 += (zInc);

    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x0, y0, z0);
    glRotated(Th, 0, 1, 0);
    glScaled(Sx, Sy, Sz);

    // set up the material properties (only front needs to be set)
    glMaterialfv(GL_FRONT, GL_AMBIENT, material);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    // enable texturing
	glEnable(GL_TEXTURE_2D);
        
    // set up texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // set up vertex arrays
    glVertexPointer(3, GL_FLOAT, 0, vertex);
    glNormalPointer(GL_FLOAT, 0, normal);
    glTexCoordPointer(2, GL_FLOAT, 0, texels);
    glColorPointer(3, GL_FLOAT, 0, colors);

    // enable vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // draw first side of the fish
    glFrontFace(GL_CCW);
    drawSide();

    // draw second side of the fish
    glScalef(1.0f, 1.0f, -1.0f);
    glFrontFace(GL_CW);
    drawSide();

    // work out new fish tail position
    GLfloat pt = sin(tailAngle * 3.14159 / 180);
    tailAngle += tailAngleInc;
    if (tailAngle < -tailAngleCutOff || tailAngle > tailAngleCutOff)
        tailAngleInc *= -1;

    // draw one side of flexible part of the tail
    vertex[143] = vertex[152] = vertex[149] = vertex[158] = vertex[167] = pt;
    glDrawArrays(GL_TRIANGLES, 6 + (4 * 6) + (3 * 5), 3 * 4);
    glScalef(1.0f, 1.0f, -1.0f);

    // draw second side of flexible part of the tail
    glFrontFace(GL_CCW);
    vertex[143] = vertex[152] = vertex[149] = vertex[158] = vertex[167] = -pt;
    glDrawArrays(GL_TRIANGLES, 6 + (4 * 6) + (3 * 5), 3 * 4);

    // disable all vertex arrays and texturing
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisable(GL_TEXTURE_2D);
   
    //  End
    glEnd();
    //  Undo transformations
    glPopMatrix();
}

/// Draws a side of the fish

void Fish::drawSide(void) {
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glDrawArrays(GL_QUADS, 6, 4 * 6);
    glDrawArrays(GL_TRIANGLES, 6 + 4 * 6, 3 * 5);
}


/*
 * ========================================
 * ====== VERTEX ARRAYS FOLLOW BELOW ======
 * ========================================
 *
 * Fish is divided into sections like this:
 *        _________
 *      /   |    | \
 *    /|    | 6  |   \     /|
 *  / 2| 4  |    | 8  \-- / |
 * -------------------- 9 | 10|
 *  \ 1| 3  |    | 7  /-- \ |
 *    \|    | 5  |  /      \|
 *      \---------/
 * 
 * 9:
 *      ___-------\
 *     /___d_------\
 *     |          . | 
 *    ` |   a   .   |
 *   `   |    .     |
 *  `     | .       |
 * `       |    b   | --> 10 joins here
 *  `     | .       |
 *   `   |    .     |
 *    ` |   c   .   |
 *     |_____     . |
 *         e -------/    e is on the bottom
 * 
 * 10:
 *                . // 
 *              - / .|
 *            /H/   /| 
 *          -|/    / |
 *         | |    /  |
 *         | | F /   |
 *    -->| |  /    |
 *         | | /     |
 *          -|/  G   |
 *            \      |
 *              \    |
 *  	            I\  |
 *                  \|  I is on the bottom
 * 
 */

GLfloat Fish::vertex[172] = {
    // 2				|					|					|
    0.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, 0.0f, 0.45f,
    // 1				|					|					|
    0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.45f, 0.5f, 0.5f, 0.0f,
    // 3				|					|					|
    0.5f, 0.0f, 0.45f, 0.5f, -0.5f, 0.0f, 1.5f, -1.0f, 0.0f, 1.5f, 0.0f, 0.45f,
    // 4				|					|					|
    0.5f, 0.5f, 0.0f, 0.5f, 0.0f, 0.45f, 1.5f, 0.0f, 0.45f, 1.5f, 1.0f, 0.0f,
    // 6				|					|					|
    1.5f, 0.0f, 0.45f, 1.5f, -1.0f, 0.0f, 2.5f, -1.0f, 0.0f, 2.5f, 0.0f, 0.45f,
    // 5				|					|					|
    1.5f, 1.0f, 0.0f, 1.5f, 0.0f, 0.45f, 2.5f, 0.0f, 0.45f, 2.5f, 1.0f, 0.0f,
    // 8				|					|					|
    2.5f, 0.0f, 0.45f, 2.5f, -1.0f, 0.0f, 3.5f, -0.5f, 0.0f, 3.5f, 0.0f, 0.2f,
    // 7				|					|					|
    2.5f, 1.0f, 0.0f, 2.5f, 0.0f, 0.45f, 3.5f, 0.0f, 0.2f, 3.5f, 0.5f, 0.0f,

    // 9
    // c				|					|					|
    3.5f, 0.5f, 0.0f, 3.5f, 0.0f, 0.2f, 3.75f, 0.4f, 0.1f,
    // b				|					|					|
    3.5f, 0.0f, 0.2f, 3.75f, -0.4f, 0.1f, 3.75f, 0.4f, 0.1f,
    // a				|					|					|
    3.5f, 0.0f, 0.2f, 3.5f, -0.5f, 0.0f, 3.75f, -0.4f, 0.1f,
    // e				|					|					|
    3.5f, 0.5f, 0.0f, 3.75f, 0.4f, 0.1f, 3.75f, 0.4f, -0.1f,
    // d				|					|					|
    3.5f, -0.5f, 0.0f, 3.75f, -0.4f, -0.1f, 3.75f, -0.4f, 0.1f,

    // 10
    // F				|					|					|
    3.75f, 0.4f, 0.1f, 3.75f, -0.4f, 0.1f, 4.3f, -0.8f, 0.0f,
    // G				|					|					|
    3.75f, 0.4f, 0.1f, 4.3f, -0.8f, 0.0f, 4.3f, 0.8f, 0.0f,
    // I				|					|					|
    3.75f, 0.4f, 0.1f, 4.3f, 0.8f, 0.0f, 3.75f, 0.4f, -0.1f,
    // H				|					|					|
    3.75f, -0.4f, -0.1f, 4.3f, -0.8f, 0.0f, 3.75f, -0.4f, 0.1f
};

GLfloat Fish::normal[172] = {
    // 2				|					|					|
    -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    // 1				|					|					|
    -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    // 4				|					|					|
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    // 3				|					|					|
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    // 6				|					|					|
    0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    // 5				|					|					|
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    // 8				|					|					|
    0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    // 7				|					|					|
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

    // 9
    // c				|					|					|
    0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.71f, 0.71f,
    // b				|					|					|
    0.0f, -0.71f, 0.71f, 0.0f, 0.0f, 1.0f, 0.0f, -0.71f, 0.71f,
    // a				|					|					|
    0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, -0.71f, 0.71f,
    // e				|					|					|
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    // d				|					|					|
    0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,

    // 10
    // F				|					|					|
    0.0f, -0.71f, 0.71f, 0.71f, -0.71f, 0.0f, 0.71f, -0.71f, 0.0f,
    // G				|					|					|
    0.0f, -0.71f, 0.71f, 0.71f, -0.71f, 0.0f, 0.71f, 0.71f, 0.0f,

    // I				|					|					|
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    // H				|					|					|
    0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f
};

GLfloat Fish::texels[172] = {
    // 2					|						|						|
    126.f / 128.f, 68.f / 128.0f, 119.f / 128.f, 47.f / 128.f, 112.f / 128.f, 68.f / 128.f,
    // 1					|						|						|
    126.f / 128.f, 68.f / 128.0f, 112.f / 128.f, 68.f / 128.f, 117.f / 128.f, 84.f / 128.f,
    // 4					|						|						|
    112.f / 128.f, 68.f / 128.f, 119.f / 128.f, 47.f / 128.f, 86.f / 128.f, 8.f / 128.f, 84.f / 128.f, 68.f / 128.f,
    // 3					|						|						|
    117.f / 128.f, 84.f / 128.f, 112.f / 128.f, 68.f / 128.f, 84.f / 128.f, 68.f / 128.f, 83.f / 128.f, 120.f / 128.f,
    // 6					|						|						|
    84.f / 128.f, 68.f / 128.f, 86.f / 128.f, 8.f / 128.f, 52.f / 128.f, 15.f / 128.f, 53.f / 128.f, 68.f / 128.f,
    // 5					|						|						|
    83.f / 128.f, 120.f / 128.f, 84.f / 128.f, 68.f / 128.f, 53.f / 128.f, 68.f / 128.f, 54.f / 128.f, 110.f / 128.f,
    // 8					|						|						|
    53.f / 128.f, 68.f / 128.f, 52.f / 128.f, 15.f / 128.f, 26.f / 128.f, 54.f / 128.f, 25.f / 128.f, 69.f / 128.f,
    // 7					|						|						|
    54.f / 128.f, 110.f / 128.f, 53.f / 128.f, 68.f / 128.f, 25.f / 128.f, 69.f / 128.f, 28.f / 128.f, 76.f / 128.f,

    // 9
    // c					|						|						|
    28.f / 128.f, 74.f / 128.f, 25.f / 128.f, 69.f / 128.f, 20.f / 128.f, 69.f / 128.f,
    // b					|						|						|
    25.f / 128.f, 69.f / 128.f, 20.f / 128.f, 69.f / 128.f, 20.f / 128.f, 69.f / 128.f,
    // a					|						|						|
    25.f / 128.f, 69.f / 128.f, 26.f / 128.f, 54.f / 128.f, 20.f / 128.f, 69.f / 128.f,
    // e					|						|						|
    26.f / 128.f, 69.f / 128.f, 20.f / 128.f, 69.f / 128.f, 20.f / 128.f, 69.f / 128.f,
    // d					|						|						|
    26.f / 128.f, 54.f / 128.f, 20.f / 128.f, 69.f / 128.f, 20.f / 128.f, 69.f / 128.f,

    // 10
    // F					|						|						|
    20.f / 128.f, 69.f / 128.f, 20.f / 128.f, 69.f / 128.f, 5.f / 128.f, 69.f / 128.f,
    // G					|						|						|
    20.f / 128.f, 69.f / 128.f, 5.f / 128.f, 69.f / 128.f, 5.f / 128.f, 69.f / 128.f,
    // I					|						|						|
    20.f / 128.f, 69.f / 128.f, 5.f / 128.f, 69.f / 128.f, 20.f / 128.f, 69.f / 128.f,
    // H					|						|						|
    20.f / 128.f, 69.f / 128.f, 5.f / 128.f, 69.f / 128.f, 20.f / 128.f, 69.f / 128.f
};

GLfloat Fish::colors[172] = {
    // 2				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f,
    // 1				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f,
    // 3				|					|					|
    0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f,
    // 4				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f, 0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f,
    // 6				|					|					|
    0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f,
    // 5				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f, 0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f,
    // 8				|					|					|
    0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f,
    // 7				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f, 0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f,

    // 9
    // c				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f,
    // b				|					|					|
    0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.2f,
    // a				|					|					|
    0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.2f,
    // e				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.2f,
    // d				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.2f,

    // 10
    // F				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f,
    // G				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f, 0.0f, 0.2f, 0.8f,
    // I				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f,
    // H				|					|					|
    0.0f, 0.0f, 0.2f, 0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 0.2f
};

#include "freeglut.h" // freeglut is in the parent directory
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>

// Camera parameters and camera auxiliary variables
typedef struct {
  double x, y, z;
} Point;

static Point centre; 
static Point obs; 
static Point up;
static double cosx,sinx,cosy,siny; 

static  double norm_vec_dir; 
static  double norm_vec_dirz;

#define PI     3.141592653589793
#define TWO_PI 6.283185307179586

static double angle_phi, angle_theta;
static double aux;
static double cos_inc, sin_inc; // cosine and sine of INC_ANG

// Predeclaration of functions to display the objects
#define NOBJS 3
static void cube_draw();
static void dod_draw();
static void ico_draw();

static unsigned object; // index of the object to display

// Array of pointers to the functions to display the objects
static void (* objects[NOBJS])() = {
   cube_draw,
   dod_draw,
   ico_draw
};

// Number of polygons in each object
#define NUM_POLYGONS_CUBE 6
#define NUM_POLYGONS_DOD  12
#define NUM_POLYGONS_ICO  20

static unsigned npolys[NOBJS] = { // polygons number for each object indexable by object index
   NUM_POLYGONS_CUBE,
   NUM_POLYGONS_DOD,
   NUM_POLYGONS_ICO
};

// Booleans to toggle

static int axes;       // show/not show axes
static int normals;    // show/not show normals
static int polygons;   // show/not show polygons
static int polygon;    // index of polygon to show contour, reset to 0 when polygons == 0

#define TIMER_TIME 5
static unsigned char KEY;
static double winx, winy;

// Mouse clicking and dragging variables
static int isDragging = 0;
static int lastX = 0;
static int lastY = 0;

// function to draw normal called from each obect display function
static void drawNormal(GLfloat* faceCenter, GLfloat* normal) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    //glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING); // Disable lighting for visualization
    glColor3f(1.0f, 1.0f, 0.0f); // yellow for visibility
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3fv(faceCenter);
    GLfloat endPoint[3];
    for (int i = 0; i < 3; i++) {
        endPoint[i] = faceCenter[i] + normal[i] * 0.1; // Scale normal for better visibility
    }
    glVertex3fv(endPoint);
    glEnd();
    glPopAttrib();
}

/***************  CUBE  ***************/

static GLfloat cube_vertices[][3] = {
    {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}, {1.0, 0.0, 1.0}, {1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}
};

static int cube_faces[NUM_POLYGONS_CUBE][4] = {
    {0, 1, 2, 3},  // Front face
    {4, 7, 6, 5},  // Back face
    {0, 4, 5, 1},  // Bottom face
    {3, 2, 6, 7},  // Top face
    {0, 3, 7, 4},  // Left face
    {1, 5, 6, 2}   // Right face
};

static GLfloat cube_normals[NUM_POLYGONS_CUBE][3] = {
    {0.0, 0.0, -1.0},  // Front
    {0.0, 0.0, 1.0},   // Back
    {0.0, -1.0, 0.0},  // Bottom
    {0.0, 1.0, 0.0},   // Top
    {-1.0, 0.0, 0.0},  // Left
    {1.0, 0.0, 0.0}    // Right
};

// for drawing normals at the center of the faces
static GLfloat cube_faceCenters[NUM_POLYGONS_CUBE][3] = {
    {0.5, 0.5, 0.0}, // Front face center
    {0.5, 0.5, 1.0}, // Back face center
    {0.5, 0.0, 0.5}, // Bottom face center
    {0.5, 1.0, 0.5}, // Top face center
    {0.0, 0.5, 0.5}, // Left face center
    {1.0, 0.5, 0.5}  // Right face center
};


static void cube_draw() {
    int *quad;
    glBegin(GL_QUADS);
    for (int i = 0; i < NUM_POLYGONS_CUBE; i++) {  // 6 faces of a cube
        glNormal3fv(cube_normals[i]);
        quad = cube_faces[i];
        glVertex3fv(cube_vertices[quad[0]]);
        glVertex3fv(cube_vertices[quad[1]]);
        glVertex3fv(cube_vertices[quad[2]]);
        glVertex3fv(cube_vertices[quad[3]]);
    }
    glEnd();
    glDisable(GL_LIGHTING);       // Disable lighting for visualization
    glColor3f(1.0f, 1.0f, 0.0f);  // yellow for visibility// yellow for visibility
    glLineWidth(2.0f);            
    if (polygons) {               // show current polygon countour
      glBegin(GL_LINES);
      quad = cube_faces[polygon];
      glVertex3fv(cube_vertices[quad[0]]);
      glVertex3fv(cube_vertices[quad[1]]);
      glVertex3fv(cube_vertices[quad[1]]);
      glVertex3fv(cube_vertices[quad[2]]);
      glVertex3fv(cube_vertices[quad[2]]);
      glVertex3fv(cube_vertices[quad[3]]);
      glVertex3fv(cube_vertices[quad[3]]);
      glVertex3fv(cube_vertices[quad[0]]);
      glEnd();
    }
    if (normals) {               // show normal
       for (int i = 0; i < NUM_POLYGONS_CUBE; i++) { // For each face
           drawNormal(cube_faceCenters[i], cube_normals[i]);
       }
    }
}

/***************  DODECAHEDRON  ***************/

static GLfloat dod_vertices[][3] = {
   { 0.190983, 0.190983, 0.190983 },
   { 0.190983, 0.190983, 0.809017 },
   { 0.190983, 0.809017, 0.190983 },
   { 0.190983, 0.809017, 0.809017 },
   { 0.809017, 0.190983, 0.190983 },
   { 0.809017, 0.190983, 0.809017 },
   { 0.809017, 0.809017, 0.190983 },
   { 0.809017, 0.809017, 0.809017 },
   { 0.5,      0.309017, 0.0      },
   { 0.5,      0.309017, 1.0      },
   { 0.5,      0.690983, 0.0      },
   { 0.5,      0.690983, 1.0      },
   { 0.309017, 0.0,      0.5      },
   { 0.309017, 1.0,      0.5      },
   { 0.690983, 0.0,      0.5      },
   { 0.690983, 1.0,      0.5      },
   { 0.0,      0.5,      0.309017 },
   { 1.0,      0.5,      0.309017 },
   { 0.0,      0.5,      0.690983 },
   { 1.0,      0.5,      0.690983 },
};

/* Each pentagon is shown as 3 triangles */

static int dod_faces[NUM_POLYGONS_DOD][3][3] = {
   { {  3, 11,  7 },   {  3,  7, 15 },   {  3, 15, 13 } },
   { {  7, 19, 17 },   {  7, 17,  6 },   {  7,  6, 15 } },
   { { 17,  4,  8 },   { 17,  8, 10 },   { 17, 10,  6 } },
   { {  8,  0, 16 },   {  8, 16,  2 },   {  8,  2, 10 } },
   { {  0, 12,  1 },   {  0,  1, 18 },   {  0, 18, 16 } },
   { {  6, 10,  2 },   {  6,  2, 13 },   {  6, 13, 15 } },
   { {  2, 16, 18 },   {  2, 18,  3 },   {  2,  3, 13 } },
   { { 18,  1,  9 },   { 18,  9, 11 },   { 18, 11,  3 } },
   { {  4, 14, 12 },   {  4, 12,  0 },   {  4,  0,  8 } },
   { { 11,  9,  5 },   { 11,  5, 19 },   { 11, 19,  7 } },
   { { 19,  5, 14 },   { 19, 14,  4 },   { 19,  4, 17 } },
   { {  1, 12, 14 },   {  1, 14,  5 },   {  1,  5,  9 } }
};

static GLfloat dod_normals[NUM_POLYGONS_DOD][3] = {
   { -0.000000, 0.850651, 0.525731 },
   { 0.850651, 0.525731, 0.000000 },
   { 0.525731, 0.000000, -0.850651 },
   { -0.525731, -0.000000, -0.850651 },
   { -0.850651, -0.525731, 0.000000 },
   { 0.000000, 0.850651, -0.525731 },
   { -0.850651, 0.525731, 0.000000 },
   { -0.525731, 0.000000, 0.850651 },
   { 0.000000, -0.850651, -0.525731 },
   { 0.525731, 0.000000, 0.850651 },
   { 0.850651, -0.525731, 0.000000 },
   { 0.000000, -0.850651, 0.525731 },
};


static GLfloat dod_face_centers[NUM_POLYGONS_DOD][3] = {
   { 0.500000, 0.861803, 0.723607 },
   { 0.861803, 0.723607, 0.500000 },
   { 0.723607, 0.500000, 0.138197 },
   { 0.276393, 0.500000, 0.138197 },
   { 0.138197, 0.276393, 0.500000 },
   { 0.500000, 0.861803, 0.276393 },
   { 0.138197, 0.723607, 0.500000 },
   { 0.276393, 0.500000, 0.861803 },
   { 0.500000, 0.138197, 0.276393 },
   { 0.723607, 0.500000, 0.861803 },
   { 0.861803, 0.276393, 0.500000 },
   { 0.500000, 0.138197, 0.723607 },
};

static void dod_draw() {
    int (*pentagon)[3] ;
    int *triangle;
    int vertexIndex;
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < NUM_POLYGONS_DOD; i++) {
        pentagon = dod_faces[i];
        glNormal3fv(dod_normals[i]);
        for (int j = 0; j < 3; j++) {
           triangle = pentagon[j];
           glVertex3fv(dod_vertices[triangle[0]]);
           glVertex3fv(dod_vertices[triangle[1]]);
           glVertex3fv(dod_vertices[triangle[2]]);
        }
    }
    glEnd();

    glDisable(GL_LIGHTING);      // Disable lighting for visualization
    glColor3f(1.0f, 1.0f, 0.0f); // yellow for visibility
    glLineWidth(2.0f);
    if (polygons) {              // show current polygon countour
      glBegin(GL_LINES);
      pentagon = dod_faces[polygon];
      triangle = pentagon[0];
      GLfloat *first = dod_vertices[triangle[0]] ;
      glVertex3fv(first);
      glVertex3fv(dod_vertices[triangle[1]]);
      glVertex3fv(dod_vertices[triangle[1]]);
      glVertex3fv(dod_vertices[triangle[2]]);
      glVertex3fv(dod_vertices[triangle[2]]);
      triangle = pentagon[1];
      glVertex3fv(dod_vertices[triangle[2]]);
      glVertex3fv(dod_vertices[triangle[2]]);
      triangle = pentagon[2];
      glVertex3fv(dod_vertices[triangle[2]]);
      glVertex3fv(dod_vertices[triangle[2]]);
      glVertex3fv(first);
      glEnd();
    }
    if (normals) {               // show normals
       for (int i = 0; i < NUM_POLYGONS_DOD; i++) {
          drawNormal(dod_face_centers[i], dod_normals[i]);
       }
    }
}

/***************  ICOSAHEDRON  ***************/

static GLfloat ico_vertices[][3] = {
   { 0.5,      1.0,      0.809017 },
   { 0.5,      1.0,      0.190983 },
   { 1.0,      0.809017, 0.5      },
   { 1.0,      0.190983, 0.5      },
   { 0.5,      0.0,      0.190983 },
   { 0.5,      0.0,      0.809017 },
   { 0.809017, 0.5,      1.0      },
   { 0.190983, 0.5,      1.0      },
   { 0.809017, 0.5,      0.0      },
   { 0.190983, 0.5,      0.0      },
   { 0.0     , 0.809017, 0.5      },
   { 0.0     , 0.190983, 0.5      }
};

static int ico_faces[NUM_POLYGONS_ICO][3] = {
   { 0,  6,  2},
   { 2,  6,  3},
   { 3,  6,  5},
   { 5,  6,  7},
   { 0,  7,  6},
   { 2,  3,  8},
   { 2,  8,  1},
   { 0,  2,  1},
   { 0,  1, 10},
   {10,  1,  9},
   { 1,  8,  9},
   { 3,  4,  8},
   { 4,  3,  5},
   { 4,  5, 11},
   { 7, 10, 11},
   { 7,  0, 10},
   { 4, 11,  9},
   { 8,  4,  9},
   { 5,  7, 11},
   {11, 10,  9}
};

static GLfloat ico_normals[NUM_POLYGONS_ICO][3] = {
   {  0.577350,  0.577350,  0.577350 },
   {  0.934172,  0.0,       0.356822 },
   {  0.577350, -0.577350,  0.577350 },
   {  0.0,      -0.356822,  0.934172 },
   {  0.0,       0.356822,  0.934172 },
   {  0.934172,  0.0,      -0.356822 },
   {  0.577350,  0.577350, -0.577350 },
   {  0.356822,  0.934172,  0.0      },
   { -0.356822,  0.934172,  0.0      },
   { -0.577350,  0.577350, -0.577350 },
   {  0.0,       0.356822, -0.934172 },
   {  0.577350, -0.577350, -0.577350 },
   {  0.356822, -0.934172,  0.0      },
   { -0.356822, -0.934172,  0.0      },
   { -0.934172,  0.0,       0.356822 },
   { -0.577350,  0.577350,  0.577350 },
   { -0.577350, -0.577350, -0.577350 },
   {  0.0,      -0.356822, -0.934172 },
   { -0.577350, -0.577350,  0.577350 },
   { -0.934172,  0.0,      -0.356822 }
};

static GLfloat ico_face_centers[NUM_POLYGONS_ICO][3] = {
   { 0.769672, 0.769672, 0.769672 },
   { 0.936339, 0.500000, 0.666667 },
   { 0.769672, 0.230328, 0.769672 },
   { 0.500000, 0.333333, 0.936339 },
   { 0.500000, 0.666667, 0.936339 },
   { 0.936339, 0.500000, 0.333333 },
   { 0.769672, 0.769672, 0.230328 },
   { 0.666667, 0.936339, 0.500000 },
   { 0.333333, 0.936339, 0.500000 },
   { 0.230328, 0.769672, 0.230328 },
   { 0.500000, 0.666667, 0.063661 },
   { 0.769672, 0.230328, 0.230328 },
   { 0.666667, 0.063661, 0.500000 },
   { 0.333333, 0.063661, 0.500000 },
   { 0.063661, 0.500000, 0.666667 },
   { 0.230328, 0.769672, 0.769672 },
   { 0.230328, 0.230328, 0.230328 },
   { 0.500000, 0.333333, 0.063661 },
   { 0.230328, 0.230328, 0.769672 },
   { 0.063661, 0.500000, 0.333333 }
};

static void ico_draw() {
    int *triangle;
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < NUM_POLYGONS_ICO; i++) {
        glNormal3fv(ico_normals[i]);
        triangle = ico_faces[i];
        glVertex3fv(ico_vertices[triangle[0]]);
        glVertex3fv(ico_vertices[triangle[1]]);
        glVertex3fv(ico_vertices[triangle[2]]);
    }
    glEnd();
    glDisable(GL_LIGHTING);      // Disable lighting for visualization
    glColor3f(1.0f, 1.0f, 0.0f); // yellow for visibility
    glLineWidth(2.0f);           
    if (polygons) {              // show current polygon countour
      glBegin(GL_LINES);
      triangle = ico_faces[polygon];
      GLfloat *first = ico_vertices[triangle[0]] ;
      glVertex3fv(first);
      glVertex3fv(ico_vertices[triangle[1]]);
      glVertex3fv(ico_vertices[triangle[1]]);
      glVertex3fv(ico_vertices[triangle[2]]);
      glVertex3fv(ico_vertices[triangle[2]]);
      glVertex3fv(first);
      glEnd();
    }

    if (normals) {               // show normals
       for (int i = 0; i < NUM_POLYGONS_ICO; i++) {
          drawNormal(ico_face_centers[i], ico_normals[i]);
       }
    }
}

// to check OpenGL errors
static void checkGLError() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL error: %d\n", err);
    }
}

// Generic display functions

static void drawAxes() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.05f, 0.f , -0.05f);
    glVertex3f(-0.05f, 0.2f ,-0.05f);
    glEnd();
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.05f, 0.f , -0.05f);
    glVertex3f( 0.15f, 0.f  ,-0.05f);
    glEnd();
    glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.05f, 0.f ,-0.05f);
    glVertex3f(-0.05f, 0.f , 0.15f);
    glEnd();
    glEnable(GL_LIGHTING);
    glPopAttrib();
}

static void display() {
    GLenum err;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(obs.x,obs.y,obs.z,centre.x,centre.y,centre.z,up.x,up.y,up.z);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20.0, winx/winy, 0.5, 10.0); // Example perspective settings
    glMatrixMode(GL_MODELVIEW);
  
    // Draw the cube
    // Light setup
    GLfloat light_position[] = { 4.5, 3.0, 5.5, 1.0 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light_diffuse[] =  { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // Adding attenuation for the first light
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.00);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.05);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);
    

    GLfloat light_position2[] = { -3.5, 3.0, -4.5, 1.0 };
    GLfloat light_ambient2[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light_diffuse2[] =  { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular2[] = { 1.0, 1.0, 1.0, 1.0 };
    
    glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    

    // Adding attenuation for the second light
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION,  1.00);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.05);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01);
    
    
    //Material setup
    GLfloat mat_ambient[] =  { 0.7, 0.0, 0.0, 1.0 };
    GLfloat mat_diffuse[] =  { 0.8, 0.0, 0.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 100.0 };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);

    // cube_draw();
    // ico_draw();
    // dod_draw();
    objects[object]();     // display object

    if (axes) drawAxes();  // display coordinate axes

    glutSwapBuffers();
    //checkGLError();
}


static void reshape(int width, int height) {
    winx = width;
    winy = height;
    glViewport(0, 0, width, height);
}

// Mouse and Keyboard functions

#define AKEY            'A'
#define BKEY            'B'
#define CKEY            'C'
#define FKEY            'F'
#define GKEY            'G'
#define NKEY            'N'
#define OKEY            'O'
#define PKEY            'P'
#define RKEY            'R'
#define SKEY            'S'
#define UPARROWKEY      101
#define DOWNARROWKEY    103
#define RIGHTARROWKEY   102
#define LEFTARROWKEY    100
#define ESCKEY          27

#define DISTANCE 6.5 // initial distance between the observer and the center of the object
#define UDD 0.05
#define INC_ANG (PI/80.)
#define HALF_INC ( 0.5 * INC_ANG )

// Move camera horizontally

static void mouseMoveX (int direction) {
   if ( direction > 0) {
       if ( angle_phi > (-INC_ANG - HALF_INC) ) {
          if ( (angle_phi < (-HALF_INC)) ||                  // phi will be 0
               (angle_phi > (TWO_PI - (INC_ANG + HALF_INC)))  // phi will be 360
             ) {
              angle_phi = 0.0;
              cosx = 1.0;
              sinx = 0.0;
              obs.x = norm_vec_dirz + centre.x;
              obs.z = centre.z;
              up.x = -siny;
              up.z = 0.0;
              glutPostRedisplay();
              return;
          }
       }
       angle_phi +=INC_ANG;
       aux  = cosx * cos_inc - sinx * sin_inc; // For cos(angle_phi + INC_ANG)
       sinx = sinx * cos_inc + cosx * sin_inc; // For sin(angle_phi + INC_ANG)
       cosx = aux;
       obs.x=norm_vec_dirz*cosx+centre.x;                          
       obs.z=norm_vec_dirz*sinx+centre.z;
       up.x = -siny * cosx;
       up.z = -siny * sinx;
       glutPostRedisplay();
       return;
   }
   // it was in the opposite direction
   if ( angle_phi < (INC_ANG + HALF_INC) ) {
      if ( (angle_phi > HALF_INC) ||                     // phi will be 0
           (angle_phi < (-TWO_PI + INC_ANG + HALF_INC))  // phi will be -360
         ) {
          angle_phi = 0.0;
          cosx = 1.0;
          sinx = 0.0;
          obs.x = norm_vec_dirz + centre.x;
          obs.z = centre.z;
          up.x = -siny;
          up.z = 0.0;
          glutPostRedisplay();
          return;
      }
   }
   angle_phi -=INC_ANG;
   aux  = cosx * cos_inc + sinx * sin_inc;  // For cos(angle_phi - INC_ANG)
   sinx = sinx * cos_inc - cosx * sin_inc;  // For sin(angle_phi - INC_ANG)
   cosx = aux;
   obs.x = norm_vec_dirz*cosx + centre.x;                          
   obs.z = norm_vec_dirz*sinx + centre.z;
   up.x = -siny * cosx;
   up.z = -siny * sinx;
   glutPostRedisplay();
}

// Move camera vertically

static void mouseMoveY (int direction) {
   if (direction > 0) {
      if ( angle_theta > (-INC_ANG - HALF_INC) ) {
        if ( (angle_theta < (-HALF_INC)) ||                  // theta will be 0
             (angle_theta > (TWO_PI - (INC_ANG + HALF_INC)))  // theta will be 360
           ) {
            angle_theta = 0.0;
            cosy = 1.0;
            siny = 0.0;
            norm_vec_dirz = norm_vec_dir;
            obs.x = norm_vec_dirz*cosx + centre.x;
            obs.y = centre.y;
            obs.z = norm_vec_dirz*sinx + centre.z;
            up.x = 0.0;
            up.y = 1.0;
            up.z = 0.0;
            glutPostRedisplay();
            return;
        }
     }
     angle_theta += INC_ANG;
     aux  = cosy * cos_inc - siny * sin_inc;  // For cos(angle_theta + INC_ANG)
     siny = siny * cos_inc + cosy * sin_inc;  // For sin(angle_theta + INC_ANG)
     cosy = aux;
     norm_vec_dirz = norm_vec_dir*cosy;
     obs.x = norm_vec_dirz*cosx + centre.x;                          
     obs.y = norm_vec_dir *siny + centre.y;
     obs.z = norm_vec_dirz*sinx + centre.z;
     up.x = -siny * cosx;
     up.y = cosy;
     up.z = -siny * sinx;
     glutPostRedisplay();
     return;
   }
   // it was the opposite direction
   if ( angle_theta < (INC_ANG + HALF_INC) ) {
      if ( (angle_theta > HALF_INC) ||                     // theta will be 0
           (angle_theta < (-TWO_PI + INC_ANG + HALF_INC))  // theta will be 360
         ) {
          angle_theta = 0.0;
          cosy = 1.0;
          siny = 0.0;
          norm_vec_dirz = norm_vec_dir;
          obs.x = norm_vec_dirz*cosx + centre.x;
          obs.y = centre.y;
          obs.z = norm_vec_dirz*sinx + centre.z;
          up.x = 0.0;
          up.y = 1.0;
          up.z = 0.0;
          glutPostRedisplay();
          return;
      }
   } 
   angle_theta -= INC_ANG;
   aux  = cosy * cos_inc + siny * sin_inc;  // For cos(angle_theta - INC_ANG)
   siny = siny * cos_inc - cosy * sin_inc;  // For cos(angle_theta - INC_ANG)
   cosy = aux;
   norm_vec_dirz = norm_vec_dir*cosy;
   obs.x = norm_vec_dirz*cosx + centre.x;                          
   obs.y = norm_vec_dir *siny + centre.y;
   obs.z = norm_vec_dirz*sinx + centre.z; 
   up.x = -siny * cosx;
   up.y = cosy;
   up.z = -siny * sinx;  
   glutPostRedisplay();
}

static void cameraInitialization() {
   centre.x = 0.5;
   centre.y = 0.5;
   centre.z = 0.5;
   
   up.x = 0.0; up.y = 1.0; up.z = 0.0;
   
   angle_phi= 0.0; angle_theta= 0.0;
   
   cosy = 1.; // cos(angle_theta)
   siny = 0.; // sin(angle_theta)
   cosx = 1.; // cos(angle_phi)
   sinx = 0.; // sin(angle_phi)
   
   norm_vec_dir = DISTANCE;
   
   norm_vec_dirz = norm_vec_dir;
   obs.x = norm_vec_dirz + centre.x;  
   obs.y = centre.y;
   obs.z = centre.z;
}

static void traveling(int direction) { /* move camera along viewing direction */
   norm_vec_dir = ( direction > 0 ) ? norm_vec_dir + UDD :  norm_vec_dir - UDD;
   norm_vec_dirz = norm_vec_dir*cosy;
   
   obs.x = norm_vec_dirz*cosx + centre.x;                          
   obs.y = norm_vec_dir*siny  + centre.y;
   obs.z = norm_vec_dirz*sinx + centre.z;
   glutPostRedisplay();
}

static void timer() { // timer function for displaying the object without releasing keyboard key
 
  switch (KEY) {

    case AKEY :          /* toggle bewteen showing axes and not showing axes  */

        axes ^= 1; 
        glutPostRedisplay();
        KEY = 0;
        return;        

    case BKEY :          /* move camera backward */
          
        traveling(1);
        glutTimerFunc(TIMER_TIME, timer, 0);
        return;

    case FKEY :          /* move camera forward */
            
        traveling(-1);
        glutTimerFunc(TIMER_TIME, timer, 0);
        return;

    case UPARROWKEY :    /* move camera up */

        mouseMoveY(1);
        glutTimerFunc(TIMER_TIME, timer, 0);
        return;
 
    case DOWNARROWKEY :  /* move camera up */

        mouseMoveY(-1);
        glutTimerFunc(TIMER_TIME, timer, 0);
        return;

    case RIGHTARROWKEY : /* move camera to the left (object to right) */

        mouseMoveX(1);
        glutTimerFunc(TIMER_TIME, timer, 0);
        return;

    case LEFTARROWKEY :  /* move camera to the right (object to left) */

        mouseMoveX(-1);
        glutTimerFunc(TIMER_TIME, timer, 0);
        return;

    case NKEY :          /* toggle to show / not show normals  */
        
        normals ^= 1;
        glutPostRedisplay();
        KEY = 0;
        return;
        
    case OKEY :          /* print observer/camera information */

        printf("gluLookAt (%f, %f, %f, %f, %f, %f, %f, %f, %f)\n",obs.x,obs.y,obs.z,centre.x,centre.y,centre.z,up.x,up.y,up.z);
        KEY = 0;
        
        return;
         
    case PKEY :          /* show polygon countour and print its index */

        if (polygons == 0) {
          polygon = -1;
          polygons = 1;
        }
        polygon++;             // show next polygon
        if (polygon >= npolys[object]) polygon = 0;
        printf("\rpolygon %d                   ", polygon); /* print polygon index */
        glutPostRedisplay();
        KEY = 0;
        return;     
   
    case RKEY :          

        KEY = 0;
        return;

    case SKEY :          /* toggle shape  */

        if (polygons == 1) {
           polygons = 0;          // we stop showing polygons contours when changing objects
           printf("\r                             "); //clean line in case report was requested
        }
        object++;
        if (object >= NOBJS) object = 0;
        glutPostRedisplay();

        KEY = 0;
        return;

    case GKEY :

        KEY = 0;

        return;
        

    case CKEY :          /* reset camera to initial position - print previous observer position  */

        printf("%f %f %f\n",obs.x,obs.y,obs.z);

        cameraInitialization();
        KEY = 0 ;
        glutPostRedisplay();
        return;

    default:

        KEY = 0;
        return;
    }
}

// Function called when keyboard keys are pressed

static void keyboard(unsigned char c, int x, int y) {
    static int background = 0;
    if (c== ESCKEY) {
      if (polygons == 1) {
        polygons = 0;         // we stop showing polygons contours
        printf("\r                             "); //clean line
        glutPostRedisplay();
        return;
      }
      background ^= 1;
      if (background) glClearColor(0.3098f, 0.50588f, 0.741176f, 1.0f);
      else glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glutPostRedisplay();
      return;
    }
    if ((c >= 'a') && (c <= 'z')) c -= ('a' - 'A');
    if ( c == 'X' ) exit(0);
    if ((c >= 'A') && (c <= 'Z')) {
       KEY = c;
       glutTimerFunc(TIMER_TIME, timer, 0);
    }
}

// Function called when keyboard keys are released

static void keyboardUp(unsigned char key, int x, int y) {
    // Handle key release events here
    KEY = 0;
    
}

// Function called when arrow keys are pressed

static void special(int key, int x, int y) {
    if ((key >= LEFTARROWKEY) && (key <= DOWNARROWKEY)) {
       KEY = key;
       glutTimerFunc(TIMER_TIME, timer, 0);
    }
}

// Function called when arrow keys are released

static void specialUp(int key, int x, int y) {
   if ((key >= LEFTARROWKEY) && (key <= DOWNARROWKEY)) KEY = 0;
}

// Function called when mouse wheel is moved

static void mouseWheelEvent(int wheel, int direction, int x, int y) {
   traveling(direction);
}

// Function called when mouse is clicked

static void mouseEvent(int button, int state, int x, int y) {
   if (button == GLUT_LEFT_BUTTON) {
       if (state == GLUT_DOWN) {
           isDragging = 1;
           lastX = x;
           lastY = y;
       } else {
           isDragging = 0;
       }
   }
}

// Function called when mouse is moved

void mouseMove(int x, int y) {
    int deltaX = x - lastX;
    int deltaY = y - lastY;

    if (isDragging && (deltaX != 0) ) {
       mouseMoveX(deltaX);
       lastX = x;
    }

    if ( isDragging && (deltaY != 0) ) {
       mouseMoveY(deltaY);
       lastY = y;
    }
}

/********************  MAIN FUNCTION  ********************/

int main(/*int argc, char** argv*/) {
    int argc = 1;
    char* argv[]= {"nothing"};
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    winx = winy = 700.;
    glutInitWindowSize(700, 700);
    glutCreateWindow("TOGGLE - Tool to visualize/debug OpenGL defined objects");
    
    glEnable(GL_DEPTH_TEST);

    KEY = 0;
    object = 0;   // show cube by default
    axes = 1;     // show axes by default
    normals = 0;  // do not show normals by default
    polygons = 0; // do not show polygons contours by default

    // for incremental update of sine and cossines
    cos_inc = cos(INC_ANG);
    sin_inc = sin(INC_ANG);

    /* initialization of the point to be observed and observer position */

    cameraInitialization();

    //checkGLError();
    glutDisplayFunc(display);

    // to intercept keyboard strokes
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseEvent);
    glutMotionFunc(mouseMove);
    glutMouseWheelFunc(mouseWheelEvent);
    glutIgnoreKeyRepeat(1);

    glutMainLoop();
    return 0;
}

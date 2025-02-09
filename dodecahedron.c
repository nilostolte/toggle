#include <stdio.h>
#include <math.h>
/***********************************************************************************************/
/**                                                                                           **/
/**      Program to generate a dedecahedron in the format required by Toggle                  **/
/**                                                                                           **/
/**      The dodecahedron is scaled and translated to be inside a 1 x 1 x 1 volume. This      **/
/**      program can be used to setup other objects.                                          **/
/**                                                                                           **/
/***********************************************************************************************/
#define SQRT5 (2.236067977499789696409173668731)
#define DT (( 1. + SQRT5 ) / 2.)
#define DR (2. / ( 1. + SQRT5 ))

static double dod_vertices[][3] = {
   { -1., -1., -1. },
   { -1., -1.,  1. },
   { -1.,  1., -1. }, 
   { -1.,  1.,  1. },
   {  1., -1., -1. }, 
   {  1., -1.,  1. },
   {  1.,  1., -1. }, 
   {  1.,  1.,  1. },
   {  0., -DR, -DT },
   {  0., -DR,  DT },
   {  0.,  DR, -DT },
   {  0.,  DR,  DT },
   { -DR, -DT,  0. },
   { -DR,  DT,  0. },
   {  DR, -DT,  0. },
   {  DR,  DT,  0. },
   { -DT,  0., -DR },
   {  DT,  0., -DR },
   { -DT,  0.,  DR },
   {  DT,  0.,  DR }
};

static int dod_faces[12][3][3] = {
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

void calculateFaceNormals() {
    double faceNormals[3];
    for (int i = 0; i < 12; i++) {
        // Fetch vertices for the current face
        double *v0 = dod_vertices[dod_faces[i][0][0]];
        double *v1 = dod_vertices[dod_faces[i][0][1]];
        double *v2 = dod_vertices[dod_faces[i][0][2]];

        // Calculate two vectors from these vertices
        double u[3] = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2]};
        double v[3] = {v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2]};

        // Cross product to get the normal
        faceNormals[0] = u[1] * v[2] - u[2] * v[1];
        faceNormals[1] = u[2] * v[0] - u[0] * v[2];
        faceNormals[2] = u[0] * v[1] - u[1] * v[0];

        // Normalize the normal vector
        float length = sqrt(faceNormals[0] * faceNormals[0] + 
                            faceNormals[1] * faceNormals[1] + 
                            faceNormals[2] * faceNormals[2]);
        faceNormals[0] /= length;
        faceNormals[1] /= length;
        faceNormals[2] /= length;
        printf("   { %f, %f, %f },\n", faceNormals[0], faceNormals[1], faceNormals[2]);
    }
}
void calculateMidPoints() {
    int (*pentagon)[3] ;
    int *triangle;
    double *v;
    double sum[3];
    for (int i = 0; i < 12; i++) {
        pentagon = dod_faces[i];
        triangle = pentagon[0];
        sum[0] = sum[1] = sum[2] = 0.0; 
        for (int j = 0; j < 3; j++) {
           v = dod_vertices[triangle[j]];
           sum[0] += v[0];
           sum[1] += v[1];
           sum[2] += v[2];
        }
        triangle = pentagon[1];
        v = dod_vertices[triangle[2]];
        sum[0] += v[0];
        sum[1] += v[1];
        sum[2] += v[2];
        triangle = pentagon[2];
        v = dod_vertices[triangle[2]];
        sum[0] = (sum[0] + v[0])*0.2; // divide by 5
        sum[1] = (sum[1] + v[1])*0.2; // divide by 5
        sum[2] = (sum[2] + v[2])*0.2; // divide by 5
        printf("   { %f, %f, %f },\n", sum[0], sum[1], sum[2]);
    }
}
#define DSC (1. / ( 1. + SQRT5 ))
int main() { 
    int i;
    double *f, x, y, z;
    printf("static double dod_vertices[][3] = {\n");
    //scales into [-0.5, 0.5] interval and translates it to [0, 1] interval
    for (i = 0; i < 20; i++) {
       f = dod_vertices[i];
       x = 0.5+f[0]*DSC;
       y = 0.5+f[1]*DSC;
       z = 0.5+f[2]*DSC;
       printf("   { %f, %f, %f },\n", x, y, z);
       f[0] = x;   // overwrite with new coordinates
       f[1] = y;   // overwrite with new coordinates
       f[2] = z;   // overwrite with new coordinates
    }
    printf("};\n");

    printf("\nstatic GLfloat dod_normals[][3] = {\n");
    calculateFaceNormals();
    printf("};\n");
    printf("\nstatic GLfloat dod_face_centers[][3] = {\n");
    calculateMidPoints();
    printf("};\n");
    
}
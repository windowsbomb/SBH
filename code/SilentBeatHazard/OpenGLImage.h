#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <ximage.h>

#include <iostream>
#include <string>
using namespace std;

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif


//GLubyte *file_data = NULL;

class OpenGLImage {
public:
    OpenGLImage ();
    ~OpenGLImage ();
    OpenGLImage(char *filename, GLuint id,GLint edgeMode_);
    void loadIdentity();
    int nextPower2(int n);
    void loadFile(char *filename);
    void draw();
    void translate(double dx, double dy);
    void updateXform();
    void inputXform();
    inline int tx() { return tx_; }
    inline int ty() { return ty_; }
    inline int width() { return width_; }
    inline int height() { return width_; }
private:
    int width_, height_;
    int texWidth_, texHeight_;
    GLubyte *data_;
    double theta_, sx_, sy_, sxy_, tx_, ty_;
    double *xform_, *invXform_;
	GLint edgeMode;
    GLuint textureID_;
    double *glMatrix_;
};


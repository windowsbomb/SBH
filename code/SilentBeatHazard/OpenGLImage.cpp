#include "OpenGLImage.h"

GLubyte *file_data = NULL;

int FindType(const string& ext)
{
	int type = 0;
	if (ext == "bmp")					type = CXIMAGE_FORMAT_BMP;
	#if CXIMAGE_SUPPORT_JPG
		else if (ext=="jpg"||ext=="jpeg")	type = CXIMAGE_FORMAT_JPG;
	#endif
	#if CXIMAGE_SUPPORT_GIF
		else if (ext == "gif")				type = CXIMAGE_FORMAT_GIF;
	#endif
	#if CXIMAGE_SUPPORT_PNG
		else if (ext == "png")				type = CXIMAGE_FORMAT_PNG;
	#endif
	#if CXIMAGE_SUPPORT_MNG
		else if (ext=="mng"||ext=="jng")	type = CXIMAGE_FORMAT_MNG;
	#endif
	#if CXIMAGE_SUPPORT_ICO
		else if (ext == "ico")				type = CXIMAGE_FORMAT_ICO;
	#endif
	#if CXIMAGE_SUPPORT_TIF
		else if (ext=="tiff"||ext=="tif")	type = CXIMAGE_FORMAT_TIF;
	#endif
	#if CXIMAGE_SUPPORT_TGA
		else if (ext=="tga")				type = CXIMAGE_FORMAT_TGA;
	#endif
	#if CXIMAGE_SUPPORT_PCX
		else if (ext=="pcx")				type = CXIMAGE_FORMAT_PCX;
	#endif
	#if CXIMAGE_SUPPORT_WBMP
		else if (ext=="wbmp")				type = CXIMAGE_FORMAT_WBMP;
	#endif
	#if CXIMAGE_SUPPORT_WMF
		else if (ext=="wmf"||ext=="emf")	type = CXIMAGE_FORMAT_WMF;
	#endif
	#if CXIMAGE_SUPPORT_J2K
		else if (ext=="j2k"||ext=="jp2")	type = CXIMAGE_FORMAT_J2K;
	#endif
	#if CXIMAGE_SUPPORT_JBG
		else if (ext=="jbg")				type = CXIMAGE_FORMAT_JBG;
	#endif
	#if CXIMAGE_SUPPORT_JP2
		else if (ext=="jp2"||ext=="j2k")	type = CXIMAGE_FORMAT_JP2;
	#endif
	#if CXIMAGE_SUPPORT_JPC
		else if (ext=="jpc"||ext=="j2c")	type = CXIMAGE_FORMAT_JPC;
	#endif
	#if CXIMAGE_SUPPORT_PGX
		else if (ext=="pgx")				type = CXIMAGE_FORMAT_PGX;
	#endif
	#if CXIMAGE_SUPPORT_RAS
		else if (ext=="ras")				type = CXIMAGE_FORMAT_RAS;
	#endif
	#if CXIMAGE_SUPPORT_PNM
		else if (ext=="pnm"||ext=="pgm"||ext=="ppm") type = CXIMAGE_FORMAT_PNM;
	#endif
		else type = CXIMAGE_FORMAT_UNKNOWN;

	return type;
}
string FindExtension(const string& name)
{
	int len = int(name.length());
	int i;
	for (i = len-1; i >= 0; i--){
		if (name[i] == '.')
		{
			return name.substr(i+1, len);
		}
	}
	return string("");
}
int readFileCxImage(const char *filename1, int &width, int &height) 
{	
	string ext = FindExtension(string(filename1));

	if (ext == "") return FALSE;
	int type = FindType(ext);

	CxImage * image = new CxImage();
	
	image->Load(filename1, type);
      //Initialize the image info structure and read an image.
      
      // Set the data size to accomodate this new image. 
	width = image->GetWidth();
	height = image->GetHeight();


      // This is the method for reading pixels that compiles and works, 
      // as opposed to GetImagePixels or GetOnePixel, which wouldn't compile. 
	
    if ( file_data )
        delete [] file_data;
    file_data = new GLubyte[width*height*4];
    
	bool alp=image->AlphaIsValid();
	

	//flip rgb and topb
	BYTE r, b, g;
	for (unsigned int j=0; j<height; j++) {
       for (unsigned int i=0; i < width; i++) {
           //  data ranges 0 to 256
           // Swap data vertically, to match NM convention.
		   r = image->GetPixelColor(i, height - j).rgbRed;
		   g = image->GetPixelColor(i, height - j).rgbGreen;
		   b = image->GetPixelColor(i, height - j).rgbBlue;
		   file_data[j*width * 3 + i * 3 + 0] = r;
		   file_data[j*width * 3 + i * 3 + 1] = g;
		   file_data[j*width * 3 + i * 3 + 2] = b;
		   //if (r == 255 && g == 255 && b == 255)
			  // file_data[j*width * 3 + i * 3 + 4] = 0;
		   //else
			   //file_data[j*width * 3 + i * 3 + 4] = 255;
		   //file_data[j*width * 3 + i * 3 + 0] = image->GetPixelColor(i, height - j).rgbRed;
		   //file_data[j*width * 3 + i * 3 + 1] = image->GetPixelColor(i, height - j).rgbGreen;
		   //file_data[j*width * 3 + i * 3 + 2] = image->GetPixelColor(i, height - j).rgbBlue;
		   //file_data[j*width * 3 + i * 3 + 3] = image->AlphaGet(i, height - j);
        }
    }
	return 0;
}



OpenGLImage::OpenGLImage() {
    tx_ = 0;
    ty_ = 0;
    theta_ = 0;
    sx_ = sy_ = 1;
    sxy_ = 0;
    width_ = 0;
    height_ = 0;
    texWidth_ = 0;
    texHeight_ = 0;
    textureID_ = 0;
    data_ = NULL;
    xform_ = new double[9];
    invXform_ = new double[9];
    loadIdentity();
    glMatrix_ = new double[16];
}

OpenGLImage::~OpenGLImage() {
    if ( data_ )
        delete [] data_;
    delete [] xform_;
    delete [] invXform_;
    delete [] glMatrix_;
}

void OpenGLImage::loadIdentity() {
    for (int j = 0; j < 3; j++)
        for (int i = 0; i < 3; i++) {
            xform_[3*j+i] = invXform_[3*j+i] = (i ==j) ? 1.0 : 0.0;
        }
}

OpenGLImage::OpenGLImage(char *filename, GLuint id,GLint edgeMode_) {
    tx_ = 0;
    ty_ = 0;
    theta_ = 0;
    sx_ = sy_ = 1;
    sxy_ = 0;
    textureID_ = id;
	edgeMode = edgeMode_;
    loadFile(filename);
    xform_ = new double[9];
    invXform_ = new double[9];
    loadIdentity();
    glMatrix_ = new double[16];
}

int OpenGLImage::nextPower2(int n) {
    int i = 1;
    while (i < n)
        i *= 2;
    return i;
}

//
// Load an image file into an opengl texture.
// Uses image magic library, can open .tif, .jpg, .ppm, .png, .bmp
//
void OpenGLImage::loadFile(char *filename) {
    //use cximage instead
	//readFileMagick(filename, width_, height_);
	readFileCxImage(filename, width_, height_);
    
	
	texWidth_ = nextPower2(width_);
    texHeight_ = nextPower2(height_);

    data_ = new GLubyte[texWidth_*texHeight_*4];
    for (int y = 0; y < height_; y++) {
        int line = texWidth_*(height_ - 1 - y);
        for (int x = 0; x < width_; x++) {

			int aa[4];
			aa[0]= (GLubyte)file_data[y * width_ * 3 + x * 3 + 0];
			aa[1] = (GLubyte)file_data[y * width_ * 3 + x * 3 + 1];
			aa[2] = (GLubyte)file_data[y * width_ * 3 + x * 3 + 2];
			aa[3] = (GLubyte)file_data[y * width_ * 3 + x * 3 + 3];
			data_[(line + x) * 4] = (GLubyte)file_data[y * width_ * 3 + x * 3 + 0];
			data_[(line + x) * 4 + 1] = (GLubyte)file_data[y * width_ * 3 + x * 3 + 1];
			data_[(line + x) * 4 + 2] = (GLubyte)file_data[y * width_ * 3 + x * 3 + 2];
			data_[(line + x) * 4 + 3] = (GLubyte)file_data[y * width_ * 3 + x * 3 + 3];
			//data_[(line + x) * 4 + 3] = (GLubyte)(255 * y / height_);
			//data_[(line + x) * 4 + 3] = (GLubyte)(255-file_data[y * width_ * 3 + x * 3 + 3]);

			//data_[(line + x) * 4 + 3] = (GLubyte)128;

        }
    }
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glBindTexture(GL_TEXTURE_2D, textureID_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edgeMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edgeMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, texWidth_, texHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_);
    glFlush();
    
}
//
// Draw the image as a texture on a GL_QUAD
//
void OpenGLImage::draw() {
    // Clear the transformation matrix and load the new one:
    glLoadIdentity();
    updateXform();
    glLoadMatrixd(glMatrix_);

    // Bind the texture associated with this QUAD
    glBindTexture(GL_TEXTURE_2D, textureID_);

    // Draw the QUAD: specify texture coordinates for each vertex
    glBegin(GL_QUADS); {
        glTexCoord2d(0, 0);
        glVertex2d(-1,-1);

        glTexCoord2d(((double)width_-1)/texWidth_, 0);
		glVertex2d(1, -1);

        glTexCoord2d(((double)width_-1)/texWidth_, ((double)height_-1)/texHeight_);
		glVertex2d(1, 1);

        glTexCoord2d(0, ((double)height_-1)/texHeight_);
        glVertex2d(-1, 1);
    }
    glEnd();
}

void OpenGLImage::translate(double dx, double dy) {
    tx_ = dx;
    ty_ = dy;
}

void OpenGLImage::inputXform() {
    cout <<"Enter translation in x and y : ";
    cin >> tx_ >> ty_;

    cout <<"Enter scale in x and y : ";
    cin >> sx_ >> sy_;

    cout <<"Enter rotation : ";
    cin >> theta_;

    cout <<"Enter shear : ";
    cin >> sxy_;
}

void OpenGLImage::updateXform() {
    double cost = cosf(theta_*M_PI/180);
    double sint = sinf(theta_*M_PI/180);
    xform_[0] = sx_*cost + sxy_*sint;
    xform_[1] = sxy_*cost - sx_*sint;
    xform_[2] = tx_;
    xform_[3] = sy_*sint;
    xform_[4] = sy_*cost;
    xform_[5] = ty_;
    xform_[6] = 0;
    xform_[7] = 0;
    xform_[8] = 1;
    
    invXform_[0] = xform_[4]*xform_[8] - xform_[5]*xform_[7];
    invXform_[1] = xform_[2]*xform_[7] - xform_[1]*xform_[8];
    invXform_[2] = xform_[1]*xform_[5] - xform_[2]*xform_[4];
    invXform_[3] = xform_[5]*xform_[6] - xform_[3]*xform_[8];
    invXform_[4] = xform_[0]*xform_[8] - xform_[2]*xform_[6];
    invXform_[5] = xform_[2]*xform_[3] - xform_[0]*xform_[5];
    invXform_[6] = xform_[3]*xform_[7] - xform_[4]*xform_[6];
    invXform_[7] = xform_[1]*xform_[6] - xform_[0]*xform_[7];
    invXform_[8] = xform_[0]*xform_[4] - xform_[1]*xform_[3];
    
    glMatrix_[0] = xform_[0];
    glMatrix_[1] = xform_[3];
    glMatrix_[2] = 0;
    glMatrix_[3] = xform_[6];
    
    glMatrix_[4] = xform_[1];
    glMatrix_[5] = xform_[4];
    glMatrix_[6] = 0;
    glMatrix_[7] = xform_[7];
    
    glMatrix_[8] = xform_[2];
    glMatrix_[9] = 0;
    glMatrix_[10] = 0;
    glMatrix_[11] = 1;
    
    glMatrix_[12] = xform_[2];
    glMatrix_[13] = xform_[5];
    glMatrix_[14] = 0;
    glMatrix_[15] = xform_[8];
}

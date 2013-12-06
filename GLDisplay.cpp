#include "GLDisplay.h"
#include "Core3_2_context.h"
#include "glheader.h"
#include "ViewerMainWindow.h"
#include "readObj.cpp"
#include "readBMP.cpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>

//pi
const double pi = 3.141592653589793238462;

static const GLfloat dice_buffer[] = {
    .25f,0.0f, 0.5f,0.0f, 0.5f,0.5f, .25f,0.5f,
    .25f,0.5f, 0.5f,0.5f, 0.5f,1.0f, .25f,1.0f,
    .498,0.0f, .75f,0.0f, .75f,0.5f, .498,0.5f,
    0.0f,0.5f, .25f,0.5f, .25f,1.0f, 0.0f,1.0f,
    .498,0.5f, .75f,0.5f, .75f,1.0f, .498,1.0f,
    0.0f,0.0f, .25f,0.0f, .25f,0.5f, 0.0f,0.5f,
};

//list of form1i(glGetUniformLocation(mShaderProgramID, "is_dice"), files[num].compare("cube.obj")==0);
//obj files we are using
const QString files[] = {
    "ground",
    "sword",
    "sword",
    "sword",
};//files
const int numFiles = sizeof(files)/sizeof(QString);

Mesh Shapes[numFiles];

const GLfloat translate_buffer[] = {
    0.0, -2.0f, 0.0f,
    0.0, 0.0f, 0.0f,
    0.0, 0.0f, 0.0f,
    0.0, 0.0f, 0.0f,
};//translate_buffer
const GLfloat scale_buffer[] = {
    100.0f,
    1.0f,
    1.0f,
    1.0f
};//rotate_buffer
GLfloat rotate_buffer[] = {
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 180.0f,
    0.0f, 0.0f, 45.0f,
    0.0f, 0.0f, -45.0f,
};//rotate_buffer

// Constructor: Set everything to zero and ensure we have an OpenGL 3.2 context (on Mac)
GLDisplay::GLDisplay(QWidget *parent)
  : QGLWidget(new Core3_2_context(QGLFormat::defaultFormat()), parent),
    sensitivity(135), step(1.0/60),
    freeMouse(true),
    mShaderProgramID(0),
    mTranslateX(0.0), mTranslateY(0.0), mTranslateZ(-2.0),
    mNear(0.5f), mFar(50.0f), mFOV(45.0f),
    mR(0.0f), mTheta(0.0f), mPhi(0.0f),
    mRotateX(0.0), mRotateY(0.0), mRotateZ(0.0),
    red(0.5f), green(0.5f), blue(0.5f),
    lightX(0.0f), lightY(0.0f), lightZ(2.0),
    intensity(.9*ViewerMainWindow::intensityMax),
    shine(.5f*ViewerMainWindow::shineMax)
{
    //display tracks mouse movement on mouseover,
    //without having to click it
    setMouseTracking(true);
    //Accept focus
    setFocusPolicy(Qt::StrongFocus);

    moving[forward] = false;
    moving[back] = false;
    moving[left] = false;
    moving[right] = false;

    //start timer at 60fps
    startTimer(1000/60);
}//constructor

// Initialization: Needs to be done once
void GLDisplay::initializeGL()
{
    // Generate VAO to store information on graphics board
    for(int i=0; i<numFiles; i++){
        glGenVertexArrays(1, &Shapes[i].mVertexArrayObjectID);
        glBindVertexArray(Shapes[i].mVertexArrayObjectID);

        // Create four buffers, one for vertices,
        // one for colors, one for normals, one for indices
        glGenBuffers(4, Shapes[i].mObjectBufferID);
    }//for

    // Black background
    glClearColor(0.117f, 0.565f, 1.0f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glSetup();

    setMinimumSize(0, 0);

    for(int i=0; i<numFiles; i++){
        load(i);
    }//for

    // Load and compile our shaders
    smoothShaderID = LoadShaders("smoothVertex.glsl", "smoothFragment.glsl");
    phongShaderID = LoadShaders("phongVertex.glsl", "phongFragment.glsl");
    mShaderProgramID = smoothShaderID;

}//initializeGL

// Draw window contents
void GLDisplay::paintGL()
{
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Generate MVP matrix
    //projection
    float fov = mFOV;
    float aspect = (double)width() / height();
    float near = mNear+.1, far = mFar;
    projection = glm::perspective(fov, aspect, near, far);

    //view
    if(mR < 0){
        mTheta += 180;
        mPhi *= -1;
        mR *= -1;
    }//if

    glm::mat4 rotateTheta = glm::rotate(-mTheta, glm::vec3(0,1,0));
    glm::mat4 rotatePhi = glm::rotate(mPhi, glm::vec3(1,0,0));
    glm::mat4 translateR = glm::mat4(1.0f);
    translateR[3][2] = -mR;

    view = translateR * rotatePhi * rotateTheta;

    for(int i=0; i<numFiles; i++){
        draw(i);
    }//for

}//paintGL

void GLDisplay::load(int num){

    Mesh *shape = &Shapes[num];
    load_obj((files[num] + ".obj").toStdString().c_str(), shape);

    shape->v_buffer_size = shape->vertices.size() * sizeof(shape->vertices[0]);
    shape->u_buffer_size = shape->uvs.size() * sizeof(shape->uvs[0]);
    shape->n_buffer_size = shape->normals.size() * sizeof(shape->normals[0]);
    shape->e_buffer_size = shape->elements.size() * sizeof(shape->elements[0]);

    cout << shape->uvs.size() << endl;

    GLfloat minX = 0.0f, maxX = 0.0f;
    GLfloat minY = 0.0f, maxY = 0.0f;
    GLfloat minZ = 0.0f, maxZ = 0.0f;
    GLfloat x, y, z;

    // vertex buffer
    for(size_t i=0; i < shape->vertices.size(); i++){
        x = shape->vertices[i].x;
            maxX = max(maxX, x);
            minX = min(minX, x);

        y = shape->vertices[i].y;
            maxY = max(maxY, y);
            minY = min(minY, y);

        z = shape->vertices[i].z;
            maxZ = max(maxZ, z);
            minZ = min(minZ, z);
    }//for

    //set scale and translate matrices
    shape->moveToOrigin[3][0] = (minX + maxX)/-2;
    shape->moveToOrigin[3][1] = (minY + maxY)/-2;
    shape->moveToOrigin[3][2] = (minZ + maxZ)/-2;

    GLfloat bounds = 0.0f;
    if(bounds < abs(minX+shape->moveToOrigin[3][0]))
        bounds = abs(minX+shape->moveToOrigin[3][0]);
    if(bounds < maxX+shape->moveToOrigin[3][0])
        bounds = maxX+shape->moveToOrigin[3][0];

    if(bounds < abs(minY+shape->moveToOrigin[3][1]))
        bounds = abs(minY+shape->moveToOrigin[3][1]);
    if(bounds < maxY+shape->moveToOrigin[3][1])
        bounds = maxY+shape->moveToOrigin[3][1];

    if(bounds < abs(minZ+shape->moveToOrigin[3][2]))
        bounds = abs(minZ+shape->moveToOrigin[3][2]);
    if(bounds < maxZ+shape->moveToOrigin[3][2])
        bounds = maxZ+shape->moveToOrigin[3][2];
    bounds = scale_buffer[num] * 0.75/bounds;
    shape->scaleToCube = glm::scale(bounds, bounds, bounds);

    // Load vertex data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, shape->mObjectBufferID[0]);
    glBufferData(GL_ARRAY_BUFFER, shape->v_buffer_size, &shape->vertices[0], GL_STATIC_DRAW);

    // Load normal data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, shape->mObjectBufferID[1]);
    glBufferData(GL_ARRAY_BUFFER, shape->n_buffer_size, &shape->normals[0], GL_STATIC_DRAW);

    // Load UV data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, shape->mObjectBufferID[2]);
    glBufferData(GL_ARRAY_BUFFER, shape->u_buffer_size, &shape->uvs[0], GL_STATIC_DRAW);

    // Load index data into buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape->mObjectBufferID[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape->e_buffer_size, &shape->elements[0], GL_STATIC_DRAW);

    Image texImage;
    ImageLoad((files[num] + ".bmp").toStdString().c_str(), &texImage);

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImage.sizeX, texImage.sizeY,
                 0, GL_BGR, GL_UNSIGNED_BYTE, texImage.data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D);

}//load

void GLDisplay::draw(int num){
    Mesh *shape = &Shapes[num];

    glm::mat4 model = glm::mat4(1.0f);
    model[3][0] += translate_buffer[num*3] + mTranslateX;
    model[3][1] += translate_buffer[num*3 + 1] + mTranslateY;
    model[3][2] += translate_buffer[num*3 + 2] + mTranslateZ;

    //generate rotation matrices
    glm::mat4 rotateX = glm::rotate(rotate_buffer[num*3], 		glm::vec3(1, 0, 0));
    glm::mat4 rotateY = glm::rotate(rotate_buffer[num*3 + 1], glm::vec3(0, 1, 0));
    glm::mat4 rotateZ = glm::rotate(rotate_buffer[num*3 + 2], glm::vec3(0, 0, 1));

    model = model * rotateZ * rotateY * rotateX;

    glm::mat4 normalMatrix = glm::transpose(glm::inverse(view*model));
    glm::mat4 lightMatrix = glm::transpose(glm::inverse(view));
    glm::vec4 light = lightMatrix * glm::vec4(lightX, lightY, lightZ, 1);

    glm::mat4 MVP = projection * view * model * shape->scaleToCube * shape->moveToOrigin;

    // Enable vertex and fragment processing
    glUseProgram(smoothShaderID);

    // Pass model view matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(mShaderProgramID, "MVP"), 1, false, &MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(mShaderProgramID, "normalMatrix"),
                                                1, false, &normalMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(mShaderProgramID, "lightMatrix"),
                                                1, false, &lightMatrix[0][0]);
    // Pass lighting properties to shaders
    glUniform4f(glGetUniformLocation(mShaderProgramID, "light"), light.x, light.y, light.z, light.w);
    glUniform1f(glGetUniformLocation(mShaderProgramID, "intensity"), intensity);
    glUniform1f(glGetUniformLocation(mShaderProgramID, "shine"), shine*shine);

    //Pass color to shaders
    glUniform1f(glGetUniformLocation(mShaderProgramID, "red"), red);
    glUniform1f(glGetUniformLocation(mShaderProgramID, "blue"), blue);
    glUniform1f(glGetUniformLocation(mShaderProgramID, "green"), green);

    // Associate positions and colors with corresponding attributes in shader
    GLuint positionAttribute = glGetAttribLocation(mShaderProgramID, "vertexPos_modelspace");
    glEnableVertexAttribArray(positionAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, shape->mObjectBufferID[0]);
    glVertexAttribPointer(positionAttribute, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint normalAttribute = glGetAttribLocation(mShaderProgramID, "vertexNormal");
    glEnableVertexAttribArray(normalAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, shape->mObjectBufferID[1]);
    glVertexAttribPointer(normalAttribute, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);

    GLuint UVAttribute = glGetAttribLocation(mShaderProgramID, "vertexUV");
    glEnableVertexAttribArray(UVAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, shape->mObjectBufferID[2]);
    glVertexAttribPointer(UVAttribute, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glUniform1i(glGetAttribLocation(mShaderProgramID, "myTextureSampler"), 0);

    // Bind index buffer and draw as triangles
    glDrawElements(GL_TRIANGLES, Shapes[num].elements.size(), GL_UNSIGNED_INT, (void*)0);

    // It is good practive to disable these once we are done
    glDisableVertexAttribArray(positionAttribute);
    glDisableVertexAttribArray(normalAttribute);
    glDisableVertexAttribArray(UVAttribute);
}//draw

// What to do when the window is resized
void GLDisplay::resizeGL(int w, int h)
{
    // Set up OpenGL viewport
    glViewport(0, 0, (GLint)w, (GLint)h);
}//resizeGL

void GLDisplay::setLightPos(double lx, double ly, double lz)
{
    // Set translation of object
    lightX = lx;
    lightY = ly;
    lightZ = lz;
    // Trigger redraw in OpenGL
    updateGL();
}//setTranslate

void GLDisplay::setView(double n, double m, double f){
    mNear = n;
    mFar = m;
    mFOV = f;

    updateGL();
}//setView

void GLDisplay::setCamera(double r, double t, double p){
    mR = r;
    mTheta = t;
    mPhi = p;

    updateGL();
}//setCamera

void GLDisplay::setRotate(double rx, double ry, double rz){
    mRotateX = rx;
    mRotateY = ry;
    mRotateZ = rz;

    updateGL();
}//setRotate

void GLDisplay::setLight(double i, double s){
    intensity = i;
    shine = s;

    updateGL();
}//setLight

void GLDisplay::setColor(double red, double green, double blue){
    this->red = red;
    this->green = green;
    this->blue = blue;

    updateGL();
}//setColor

void GLDisplay::setShading(int option){
    switch(option){
        case smoothOptionNumber:
            mShaderProgramID = smoothShaderID;
            break;
        case phongOptionNumber:
            mShaderProgramID = phongShaderID;
            break;
    }//switch

    updateGL();
}//setShading

void GLDisplay::timerEvent(QTimerEvent *e){
    if(e == 0){}
		// Rotating a single object
		objectSpinning(); 

    if(!freeMouse){			
			// Moving the camera
			if(moving[forward])
					move(glm::vec4(0,0,-step,0));
			if(moving[back])
					move(glm::vec4(0,0,step,0));
			if(moving[left])
					move(glm::vec4(-step,0,0,0));
			if(moving[right])
					move(glm::vec4(step,0,0,0));
		}//if
	updateGL();
}//timerEvent

//respond to mouse events
void GLDisplay::mouseMoveEvent(QMouseEvent *e){
    //if mouse is free do nothing
    if(freeMouse){
        return;
    }//if

    GLfloat dx = (e->x() - width()/2);
    mTheta += -dx*sensitivity/width();
    mTheta = (int)mTheta % 360;

    GLfloat dy = (e->y() - height()/2);
    mPhi += dy*sensitivity/height();
    mPhi = (int)mPhi % 360;

    QPoint global = mapToGlobal(QPoint(width()/2,height()/2));
    QCursor::setPos(global);

    updateGL();
}//mouseMoveEvent

void GLDisplay::mousePressEvent(QMouseEvent * e){
    if(e->button()==Qt::LeftButton){
        freeMouse = !(freeMouse);
        //show or hide cursor
        setCursor( QCursor( Qt::CursorShape(
            Qt::ArrowCursor +
            Qt::BlankCursor -
            cursor().shape()
        )));
    }//if
}//mousePressEvent

//respond to keyboard presses
void GLDisplay::keyPressEvent(QKeyEvent *e){
    if(e->isAutoRepeat())
        QWidget::keyPressEvent(e);

    switch(e->key())
    {
    case Qt::Key_W:
    case Qt::Key_Up:
        moving[forward] = true;
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        moving[back] = true;
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        moving[left] = true;
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        moving[right] = true;
        break;
    default:
        QWidget::keyPressEvent(e);
        break;
    }//switch

}//keyPressEvent

void GLDisplay::keyReleaseEvent(QKeyEvent *e){
    if(e->isAutoRepeat()){
        QWidget::keyReleaseEvent(e);
        return;
    }//if

    switch(e->key())
    {
    case Qt::Key_W:
    case Qt::Key_Up:
        moving[forward]=false;
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        moving[back] = false;
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        moving[left] = false;
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        moving[right] = false;
        break;
    case Qt::Key_Escape:
        freeMouse = true;
        //show cursor
        setCursor(QCursor(Qt::ArrowCursor));
        break;
    default:
        QWidget::keyReleaseEvent(e);
        break;
    }//switch

}//keyReleaseEvent

void GLDisplay::objectSpinning(){
	int obj_id = 2;

	mRotateX ++;
	mRotateY ++;
	mRotateZ ++;

	if(mRotateX >= 360)
		mRotateX -= 360;

	if(mRotateY >= 360)
		mRotateY -= 360;

	if(mRotateZ >= 360)
		mRotateZ -= 360;
	
	rotate_buffer[3 * obj_id] 		= mRotateX;
	rotate_buffer[3 * obj_id + 1] = mRotateY;
	rotate_buffer[3 * obj_id + 2]	= mRotateZ;
}

void GLDisplay::move(glm::vec4 direction){
    //Rotate up/down. Determine which axis to rotate around.
    if(direction.x != 0)
        direction = glm::rotate(mPhi, glm::vec3(0, 0, 1)) * direction;
    else
        direction = glm::rotate(mPhi, glm::vec3(1, 0, 0)) * direction;
    direction = glm::rotate(mTheta, glm::vec3(0, 1, 0)) * direction;
    mTranslateX -= direction.x;
    mTranslateY += direction.y;
    mTranslateZ -= direction.z;

    updateGL();
}//move

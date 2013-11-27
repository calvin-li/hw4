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

//list of obj files we are using
const QString files[] = {
    "cube.obj",
};//files
const int numFiles = sizeof(files)/sizeof(QString);

const GLfloat translate_buffer[] = {
    0.0f, 0.0f, 0.0f,
};//translate_buffer
const GLfloat rotate_buffer[] = {
    0.0f, 0.0f, 0.0f,
};//rotate_buffer

// Constructor: Set everything to zero and ensure we have an OpenGL 3.2 context (on Mac)
GLDisplay::GLDisplay(QWidget *parent)
  : QGLWidget(new Core3_2_context(QGLFormat::defaultFormat()), parent),
    sensitivity(135), step(1.0/60),
    freeMouse(true),
    mVertexArrayObjectID(0), mShaderProgramID(0),
    mTranslateX(0.0), mTranslateY(0.0), mTranslateZ(0.0),
    mNear(0.5f), mFar(5.0f), mFOV(45.0f),
    mR(0.0f), mTheta(0.0f), mPhi(0.0f),
    mRotateX(0.0), mRotateY(0.0), mRotateZ(0.0),
    red(0.5f), green(0.5f), blue(0.5f),
    moveToOrigin(glm::mat4(1.0f)), scaleToCube(glm::mat4(1.0f)),
    lightX(0.0f), lightY(0.0f), lightZ(2.0),
    intensity(.2*ViewerMainWindow::intensityMax),
        shine(.5f*ViewerMainWindow::shineMax)
{
    mObjectBufferID[0] = 0;
    mObjectBufferID[1] = 0;
    mObjectBufferID[2] = 0;
    mObjectBufferID[3] = 0;

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
    glGenVertexArrays(1, &mVertexArrayObjectID);
    glBindVertexArray(mVertexArrayObjectID);

    // Create four buffers, one for vertices,
    //    one for colors, one for normals, one for indices
    glGenBuffers(5, mObjectBufferID);

    // Black background
    glClearColor(0.0, 0.0, 0.0, 0.0);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glSetup();

    setMinimumSize(0, 0);
    load(0);

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

    Mesh *shape = new Mesh();
    load_obj(files[num].toStdString().c_str(), shape);

    index_buffer_size = shape->elements.size();
    vertex_buffer_size = shape->vertices.size();
    int num_normal = shape->normals.size();
    GLfloat minX = 0.0f, maxX = 0.0f;
    GLfloat minY = 0.0f, maxY = 0.0f;
    GLfloat minZ = 0.0f, maxZ = 0.0f;
    GLfloat x, y, z;

    GLfloat vertex_buffer[vertex_buffer_size*4];
    GLfloat color_buffer[vertex_buffer_size*3];
    GLfloat normal_buffer[num_normal*4];
    GLuint index_buffer[index_buffer_size];

    for(int i=0; i < vertex_buffer_size; i++){
        x = shape->vertices[i].x;
        vertex_buffer[i*4] = x;
            maxX = max(maxX, x);
            minX = min(minX, x);

        y = shape->vertices[i].y;
        vertex_buffer[i*4+1] = y;
            maxY = max(maxY, y);
            minY = min(minY, y);

        z = shape->vertices[i].z;
        vertex_buffer[i*4+2] = z;
            maxZ = max(maxZ, z);
            minZ = min(minZ, z);

        vertex_buffer[i*4+3] = 1;
    }//for

    for(int i=0; i < vertex_buffer_size; i++){
        color_buffer[i*3] = red;
        color_buffer[i*3+1] = green;
        color_buffer[i*3+2] = blue;
    }//for

    for(int i=0; i < num_normal; i++){
        normal_buffer[i*4] = shape->normals[i].x;
        normal_buffer[i*4+1] = shape->normals[i].y;
        normal_buffer[i*4+2] = shape->normals[i].z;
        normal_buffer[i*4+3] = 0;
    }//for

    for(int i=0; i < index_buffer_size; i++){
        index_buffer[i] = shape->elements[i];
    }//for

    GLuint sizeofvertex = sizeof(vertex_buffer);
    GLuint sizeofcolor = sizeof(color_buffer);
    GLuint sizeofnormal = sizeof(normal_buffer);
    GLuint sizeofUV = sizeof(dice_buffer);
    GLuint sizeofindex = sizeof(index_buffer);

    //set scale and translate matrices
    moveToOrigin[3][0] = (minX + maxX)/-2;
    moveToOrigin[3][1] = (minY + maxY)/-2;
    moveToOrigin[3][2] = (minZ + maxZ)/-2;

    GLfloat bounds = 0.0f;
    if(bounds < abs(minX+moveToOrigin[3][0]))
        bounds = abs(minX+moveToOrigin[3][0]);
    if(bounds < maxX+moveToOrigin[3][0])
        bounds = maxX+moveToOrigin[3][0];

    if(bounds < abs(minY+moveToOrigin[3][1]))
        bounds = abs(minY+moveToOrigin[3][1]);
    if(bounds < maxY+moveToOrigin[3][1])
        bounds = maxY+moveToOrigin[3][1];

    if(bounds < abs(minZ+moveToOrigin[3][2]))
        bounds = abs(minZ+moveToOrigin[3][2]);
    if(bounds < maxZ+moveToOrigin[3][2])
        bounds = maxZ+moveToOrigin[3][2];
    bounds = .75/bounds;
    scaleToCube = glm::scale(bounds, bounds, bounds);

    // Black background
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // Load vertex data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, mObjectBufferID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeofvertex, vertex_buffer, GL_STATIC_DRAW);

    // Load color data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, mObjectBufferID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeofcolor, color_buffer, GL_STATIC_DRAW);

    // Load normal data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, mObjectBufferID[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeofnormal, normal_buffer, GL_STATIC_DRAW);

    // Load UV data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, mObjectBufferID[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeofUV, dice_buffer, GL_STATIC_DRAW);

    // Load index data into buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mObjectBufferID[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeofindex, index_buffer, GL_STATIC_DRAW);

    Image diceImage;
    ImageLoad("dice_texture2.bmp", &diceImage);

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, diceImage.sizeX, diceImage.sizeY,
                 0, GL_RGB, GL_UNSIGNED_BYTE, diceImage.data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

}//load

void GLDisplay::draw(int num){

    glm::mat4 model = glm::mat4(1.0f);
    model[3][0] += translate_buffer[num*3] + mTranslateX;
    model[3][1] += translate_buffer[num*3 + 1] + mTranslateY;
    model[3][2] += translate_buffer[num*3 + 2] + mTranslateZ;

    //generate rotation matrices
    glm::mat4 rotateX = glm::rotate(rotate_buffer[num*3], glm::vec3(1, 0, 0));
    glm::mat4 rotateY = glm::rotate(rotate_buffer[num*3 + 1], glm::vec3(0, 1, 0));
    glm::mat4 rotateZ = glm::rotate(rotate_buffer[num*3 + 2], glm::vec3(0, 0, 1));

    model = model * rotateZ * rotateY * rotateX;

    glm::mat4 normalMatrix = glm::transpose(glm::inverse(view*model));
    glm::mat4 lightMatrix = glm::transpose(glm::inverse(view));
    glm::vec4 light = lightMatrix * glm::vec4(lightX, lightY, lightZ, 1);

    glm::mat4 MVP = projection * view * model * scaleToCube * moveToOrigin;

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
    glUniform1i(glGetUniformLocation(mShaderProgramID, "is_dice"), files[num].compare("cube.obj")==0);


    // Associate positions and colors with corresponding attributes in shader
    GLuint positionAttribute = glGetAttribLocation(mShaderProgramID, "vertexPos_modelspace");
    glEnableVertexAttribArray(positionAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, mObjectBufferID[0]);
    glVertexAttribPointer(positionAttribute, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint colorAttribute = glGetAttribLocation(mShaderProgramID, "vertexColor");
    glEnableVertexAttribArray(colorAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, mObjectBufferID[1]);
    glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint normalAttribute = glGetAttribLocation(mShaderProgramID, "vertexNormal");
    glEnableVertexAttribArray(normalAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, mObjectBufferID[2]);
    glVertexAttribPointer(normalAttribute, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);

    GLuint UVAttribute = glGetAttribLocation(mShaderProgramID, "vertexUV");
    glEnableVertexAttribArray(UVAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, mObjectBufferID[3]);
    glVertexAttribPointer(UVAttribute, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glUniform1i(glGetAttribLocation(mShaderProgramID, "myTextureSampler"), 0);

    // Bind index buffer and draw as triangles
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mObjectBufferID[4]);
    glDrawElements(GL_TRIANGLES, index_buffer_size, GL_UNSIGNED_INT, (void*)0);

    // It is good practive to disable these once we are done
    glDisableVertexAttribArray(positionAttribute);
    glDisableVertexAttribArray(colorAttribute);
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

    GLfloat color_buffer[vertex_buffer_size*3];
    for(int i=0; i<vertex_buffer_size; i++){
        color_buffer[i*3] = red;
        color_buffer[i*3+1] = green;
        color_buffer[i*3+2] = blue;
    }//for

    GLuint sizeofcolor = sizeof(color_buffer);
    // Load color data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, mObjectBufferID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeofcolor, color_buffer, GL_STATIC_DRAW);

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
    if(freeMouse)
        return;
    if(moving[forward])
        move(glm::vec4(0,0,-step,0));
    if(moving[back])
        move(glm::vec4(0,0,step,0));
    if(moving[left])
        move(glm::vec4(-step,0,0,0));
    if(moving[right])
        move(glm::vec4(step,0,0,0));
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
	if(mPhi > 90) mPhi = 90;
	else if (mPhi < -90) mPhi = -90;

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

void GLDisplay::move(glm::vec4 direction){
    direction = glm::rotate(mTheta, glm::vec3(0, 1, 0)) * direction;
    mTranslateX -= direction.x;
    mTranslateZ -= direction.z;

    updateGL();
}//move

#ifndef GLDISPLAY_H
#define GLDISPLAY_H

#include <glm/glm.hpp>
#include <QGLWidget>

class GLDisplay : public QGLWidget
{
    Q_OBJECT

    public:
        GLDisplay(QWidget *parent = 0);

        // Set the displayed objects translation
        void setLightPos(double lx, double ly, double lz);
        void setView(double n, double m, double f);
        void setCamera(double r, double t, double p);
        void setRotate(double rx, double ry, double rz);
        void setLight(double aI, double dR);
        void setColor(double red, double green, double blue);
        void setFile(QString s);
        void setShading(int option);
        void load();

        static const int smoothOptionNumber = 0;
        static const int phongOptionNumber = 1;

    protected:
        virtual void initializeGL();
        virtual void resizeGL(int w, int h);
        virtual void paintGL();

        virtual void mouseMoveEvent(QMouseEvent * e);
        GLfloat lastX, lastY;
        //previous mouse coordinates (used for movement)

    private:
        GLuint mVertexArrayObjectID;
        GLuint mObjectBufferID[5];
        GLuint mTextureID;
        GLuint mShaderProgramID;

        GLuint smoothShaderID;
        GLuint phongShaderID;

        float mTranslateX;
        float mTranslateY;
        float mTranslateZ;

        float mNear;
        float mFar;
        float mFOV;

        float mR;
        float mTheta;
        float mPhi;

        float mRotateX;
        float mRotateY;
        float mRotateZ;

        float red;
        float green;
        float blue;

        QString file;

        int vertex_buffer_size;
        int index_buffer_size;

        glm::mat4 moveToOrigin;
        glm::mat4 scaleToCube;

        float lightX;
        float lightY;
        float lightZ;

        float intensity;
        float shine;

};//GLDisplay
#endif

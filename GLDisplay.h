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
        void setShading(int option);

        static const int smoothOptionNumber = 0;
        static const int phongOptionNumber = 1;

    protected:
        virtual void initializeGL();
        virtual void resizeGL(int w, int h);
        virtual void paintGL();

        virtual void mouseMoveEvent(QMouseEvent * e);
        virtual void mousePressEvent(QMouseEvent * e);
        virtual void keyPressEvent(QKeyEvent * e);
        virtual void keyReleaseEvent(QKeyEvent * e);
        virtual void timerEvent(QTimerEvent * e);
        const GLfloat sensitivity;
        const double step;
        bool freeMouse;
        //previous mouse coordinates (used for movement)

    private:
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

        float lightX;
        float lightY;
        float lightZ;

        float intensity;
        float shine;

        //movement keys
        bool moving[4];
        static const unsigned int forward = 0;
        static const unsigned int back = 1;
        static const unsigned int left = 2;
        static const unsigned int right = 3;
        void move(glm::vec4);
				void objectSpinning();

        glm::mat4 projection;
        glm::mat4 view;
        void load(int num);
        void draw(int num);

};//GLDisplay
#endif

#include <QtGui>

class GLDisplay;
class QLineEdit;

class ViewerMainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        ViewerMainWindow();
        static const float intensityMax = 2;
        static const float shineMax = 20;

    private:
        static const float dialWidth = 60;
        static const float lineWidth = 50;

        GLDisplay *mGLDisplay;
        QLineEdit *mXLightLineEdit;
        QLineEdit *mYLightLineEdit;
        QLineEdit *mZLightLineEdit;

        QDial *mFOV;
        static const float fovMax = 180;
        QLineEdit *mNear;
        QLineEdit *mFar;

        QLineEdit *mR;
        QDial *cameraDials[2];
        static const float cameraMax = 360;

        QDial *axesDials[3];
        static const float axesMax = 360;

        QDial *lightDials[2];
        static const float lightNotch = 100;

        QDial *colorDials[3];
        static const float colorMax = 256;

        QComboBox *shadingOptions;
        QString smooth;
        QString phong;

    public slots:
        void lightPosChanged();
        void viewChanged();
        void cameraChanged();
        void rotateChanged();
        void lightChanged();
        void colorChanged();
        void shadingChanged(QString);
        void refresh();
				void reset();
};


#include <QApplication>
#include <QtGui>
#include <QGLFormat>
#include "ViewerMainWindow.h"

int main(int argv, char **args)
{
    QApplication app(argv, args);

    QGLFormat glFormat;
    glFormat.setVersion( 3, 2 );
    glFormat.setProfile( QGLFormat::CoreProfile );// Requires >=Qt-4.8.0
    glFormat.setSampleBuffers( true );

    ViewerMainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}

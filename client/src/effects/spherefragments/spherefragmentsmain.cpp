// Qt
#include <QApplication>

// widget
#include "widget.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);

    Widget w(format);

    w.move(0,0);
    w.resize(1024,576);
    w.show();
    
    return a.exec();
}

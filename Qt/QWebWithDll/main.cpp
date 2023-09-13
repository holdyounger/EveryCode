#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	qputenv("QT_OPENGL", "software");
	char* opengl = getenv("QT_ANGLE_PLATFORM");
	if (opengl) {
	}
	else
	{
		qputenv("QT_ANGLE_PLATFORM", "d3d9");
		opengl = getenv("QT_ANGLE_PLATFORM");
	}

	QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}

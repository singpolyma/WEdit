#include <QApplication>
#include "wedit.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	WEdit we;
	we.show();

	return app.exec();
}

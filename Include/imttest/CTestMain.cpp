#include <itest/CStandardTestExecutor.h>

// Qt includes
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{

	QApplication app(argc, argv);
	app.setAttribute(Qt::AA_Use96Dpi, true);
	itest::CStandardTestExecutor instance;

	return instance.RunTests(argc, argv);
}


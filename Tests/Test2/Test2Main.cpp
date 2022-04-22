#include <itest/CStandardTestExecutor.h>

// Qt includes
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{

	QApplication app(argc, argv);
	itest::CStandardTestExecutor instance;
	return instance.RunTests(argc, argv);
}


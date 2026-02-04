#include <itest/CStandardTestExecutor.h>


// Qt includes
#include <QtCore/QCoreApplication>


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	itest::CStandardTestExecutor instance;
	return instance.RunTests(argc, argv);
}

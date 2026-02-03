#include "CFunctionalityManagerTest.h"


// Qt includes
#include <QtTest/QtTest>


int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);
	
	imtfunctest::CFunctionalityManagerTest test;
	
	return QTest::qExec(&test, argc, argv);
}


// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <itest/CStandardTestExecutor.h>


// Qt includes
#include <QtCore/QCoreApplication>


int main(int argc, char *argv[])
{

	QCoreApplication app(argc, argv);
	itest::CStandardTestExecutor instance;
	return instance.RunTests(argc, argv);
}


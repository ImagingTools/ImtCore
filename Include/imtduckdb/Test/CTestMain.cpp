// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <itest/CStandardTestExecutor.h>

// Qt includes
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{

	QApplication app(argc, argv);
	itest::CStandardTestExecutor instance;
	return instance.RunTests(argc, argv);
}


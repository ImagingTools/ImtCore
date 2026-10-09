// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <QtQuickTest/quicktest.h>

// ImtCore includes
#include "CQmlTestSetup.h"


int main(int argc, char** argv)
{
	// Quick Test shows a window for every test file; an explicit QT_QPA_PLATFORM still wins
	if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM")){
		qputenv("QT_QPA_PLATFORM", "offscreen");
	}

	QTEST_SET_MAIN_SOURCE_PATH
	imtqmltest::CQmlTestSetup setup;

	return quick_test_main_with_setup(argc, argv, "ImtCoreQmlTest", QUICK_TEST_SOURCE_DIR, &setup);
}


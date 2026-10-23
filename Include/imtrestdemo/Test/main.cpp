// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
//
// Tests of imtrest with a live demo server. The tests of the classes CLive* send the requests with a client of the test, the test
// CCurlTest does the same with the program curl and is skipped if there is no curl.
//
// The path of curl is taken from the argument "--curl <path>" (or "--curl=<path>") or from the environment variable IMTREST_CURL;
// without them curl is searched in PATH.

// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QtGlobal>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrestdemo/DemoTlsTools.h>


int main(int argc, char* argv[])
{
	imtrestdemo::DemoTlsTools::PrepareOpensslBackend(argv);

	// The argument is not known to the test framework of Qt, it is moved to the environment and removed from the arguments
	int argumentCount = 0;
	for (int i = 0; i < argc; ++i){
		const QByteArray argument(argv[i]);

		if (argument == "--curl" && i + 1 < argc){
			qputenv("IMTREST_CURL", QByteArray(argv[i + 1]));
			++i;

			continue;
		}

		if (argument.startsWith("--curl=")){
			qputenv("IMTREST_CURL", argument.mid(int(qstrlen("--curl="))));

			continue;
		}

		argv[argumentCount++] = argv[i];
	}

	argv[argumentCount] = nullptr;

	QCoreApplication application(argumentCount, argv);

	itest::CStandardTestExecutor instance;

	return instance.RunTests(argumentCount, argv);
}

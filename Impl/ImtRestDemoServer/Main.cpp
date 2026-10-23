// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
//
// ImtRestDemoServer: the demo server of imtrest. It shows and lets to check all that the HTTP server of imtrest can do:
// the requests in parts, the limits and the refusals before the body, the streaming of the big bodies in both directions,
// the ranges and the conditions, the timeouts and the failures of the servlets. See imtrestdemo/CDemoServer.h for the paths.
//
// The server can be used by the demo client (ImtRestDemoClient), by curl and by any other HTTP client.

// STL includes
#include <atomic>
#include <csignal>
#include <cstdio>

// Qt includes
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QStandardPaths>
#include <QtCore/QTimer>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>
#include <imtrestdemo/CDemoServer.h>
#include <imtrestdemo/DemoTlsTools.h>
#include <GeneratedFiles/ImtRestDemoServer/CImtRestDemoServer.h>


namespace
{


std::atomic<bool> s_isStopRequested(false);


void OnSignal(int)
{
	s_isStopRequested = true;
}


}


int main(int argc, char* argv[])
{
	imtrestdemo::DemoTlsTools::PrepareOpensslBackend(argv);

	QCoreApplication application(argc, argv);
	QCoreApplication::setApplicationName("ImtRestDemoServer");

	imtrestdemo::CDemoServerSettings defaults;

	QCommandLineParser parser;
	parser.setApplicationDescription("Demo server of imtrest");
	parser.addHelpOption();

	QCommandLineOption portOption("port", "Port to listen (0 - a free port)", "port", "7400");
	QCommandLineOption directoryOption("files-dir", "Directory of the files (the sample files are created in it)", "directory");
	QCommandLineOption threadsOption("threads", "Limit of the threads of the handlers", "count", QString::number(defaults.threadsLimit));
	QCommandLineOption inactivityOption("inactivity-timeout", "Time without data while a request is received or a response is sent, ms (0 - no limit)", "ms", QString::number(defaults.requestInactivityTimeoutMs));
	QCommandLineOption keepAliveOption("keep-alive-timeout", "Time of an idle connection, ms (0 - no limit)", "ms", QString::number(defaults.keepAliveTimeoutMs));
	QCommandLineOption processingOption("processing-timeout", "Time that a servlet can work on a request, ms (0 - no limit)", "ms", QString::number(defaults.requestProcessingTimeoutMs));
	QCommandLineOption maxBodyOption("max-body-size", "Limit of the body that is received to the memory, bytes (0 - no limit)", "bytes", QString::number(defaults.maxBodySize));
	QCommandLineOption tokenOption("token", "Token of /upload and of /files/private", "token", defaults.token);
	QCommandLineOption logOption("log", "Write the messages of the server to the standard output");

	QCommandLineOption tlsOption("tls", "Encrypt the connections (TLS), needs --tls-cert and --tls-key");
	QCommandLineOption tlsCertOption("tls-cert", "Certificate file (PEM, can have the intermediate certificates after the certificate of the server)", "file");
	QCommandLineOption tlsChainOption("tls-chain", "File of an intermediate certificate (PEM), can be repeated", "file");
	QCommandLineOption tlsKeyOption("tls-key", "Private key file (PEM, without a password)", "file");
	QCommandLineOption tlsSamplesOption("tls-samples", "Directory of the test certificates: it is made with openssl if it is empty, the client can take the files from it", "directory");
	QCommandLineOption tlsPresetOption("tls-preset", "Start with TLS and this set of the test certificates (single, chain, several, bundle, ec, wrong-host)", "name");
	QCommandLineOption noPresetsOption("no-tls-presets", "Do not make the sets of the test certificates (the default: they are made with openssl in the directory of the files with the ending .tls-presets)");
	QCommandLineOption tlsStorageOption("tls-storage", "Directory for the certificates and keys that a client sends to the server (the default: the directory of the files with the ending .tls)", "directory");
	QCommandLineOption opensslOption("openssl", "Program openssl for --tls-samples (the default: the variable IMTREST_OPENSSL or PATH)", "path");

	parser.addOptions({tlsOption, tlsCertOption, tlsChainOption, tlsKeyOption, tlsSamplesOption, tlsPresetOption, noPresetsOption, tlsStorageOption, opensslOption, portOption, directoryOption, threadsOption, inactivityOption, keepAliveOption, processingOption, maxBodyOption, tokenOption, logOption});
	parser.process(application);

	imtrestdemo::CDemoServerSettings settings;
	settings.port = parser.value(portOption).toInt();
	settings.filesDirectory = parser.isSet(directoryOption)
				? parser.value(directoryOption)
				: QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/imtrestdemo-files";
	settings.threadsLimit = parser.value(threadsOption).toInt();
	settings.requestInactivityTimeoutMs = parser.value(inactivityOption).toInt();
	settings.keepAliveTimeoutMs = parser.value(keepAliveOption).toInt();
	settings.requestProcessingTimeoutMs = parser.value(processingOption).toInt();
	settings.maxBodySize = parser.value(maxBodyOption).toInt();
	settings.token = parser.value(tokenOption);
	settings.isLogEnabled = parser.isSet(logOption);

	settings.isTlsEnabled = parser.isSet(tlsOption);
	settings.tlsCertificateFile = parser.value(tlsCertOption);
	settings.tlsChainFiles = parser.values(tlsChainOption);
	settings.tlsKeyFile = parser.value(tlsKeyOption);

	if (parser.isSet(tlsStorageOption)){
		settings.tlsStorageDirectory = QFileInfo(parser.value(tlsStorageOption)).absoluteFilePath();
	}

	// The sets of the test certificates are made once and stay: the presets that a client applies with one button
	const bool isPresetsWanted = parser.isSet(tlsSamplesOption) || parser.isSet(tlsPresetOption)
				|| (!parser.isSet(noPresetsOption) && !imtrestdemo::DemoTlsTools::FindOpenssl(parser.value(opensslOption)).isEmpty());
	if (isPresetsWanted){
		settings.tlsSamplesDirectory = parser.isSet(tlsSamplesOption)
					? QFileInfo(parser.value(tlsSamplesOption)).absoluteFilePath()
					: QDir(settings.filesDirectory).absolutePath() + ".tls-presets";

		if (!QFileInfo::exists(settings.tlsSamplesDirectory + "/ca/root.pem")){
			QString errorText;
			if (!imtrestdemo::DemoTlsTools::GenerateSamples(settings.tlsSamplesDirectory, parser.value(opensslOption), errorText)){
				std::fprintf(stderr, "The test certificates are not made: %s\n", qPrintable(errorText));

				return 1;
			}
		}
	}

	if (parser.isSet(tlsPresetOption)){
		bool isFound = false;
		for (const QJsonValue& sample : imtrestdemo::DemoTlsTools::DescribeSamples(settings.tlsSamplesDirectory)){
			if (sample.toObject().value("id").toString() == parser.value(tlsPresetOption)){
				isFound = true;
				settings.isTlsEnabled = true;
				settings.tlsCertificateFile = sample.toObject().value("certificateFile").toString();
				settings.tlsKeyFile = sample.toObject().value("keyFile").toString();
				settings.tlsChainFiles.clear();
				for (const QJsonValue& chainFile : sample.toObject().value("chainFiles").toArray()){
					settings.tlsChainFiles.append(chainFile.toString());
				}
			}
		}

		if (!isFound){
			std::fprintf(stderr, "There is no set of the test certificates '%s'\n", qPrintable(parser.value(tlsPresetOption)));

			return 1;
		}
	}

	imtrestdemo::CDemoServer server(imtrestdemo::CreateDemoCompositionFactory<CImtRestDemoServer>());
	if (!server.Start(settings)){
		std::fprintf(stderr, "The server is not started: %s\n", qPrintable(server.GetErrorString()));

		return 1;
	}

	std::printf("ImtRestDemoServer listens on %s://localhost:%d\n", settings.isTlsEnabled ? "https" : "http", server.GetPort());
	std::printf("Files: %s\n", qPrintable(server.GetFilesDirectory()));
	std::printf("Paths: /echo /info /files/<path> /upload/<name> /stream/<size> /behavior/<case> /dynamic/<name> /refuse /admin/settings\n");
	std::printf("/dynamic: a body up to %d bytes goes to the memory, a bigger one to a file, a body over %d bytes is refused with 413\n",
				imtrestdemo::DemoProtocol::s_dynamicMemoryBodyLimit,
				imtrestdemo::DemoProtocol::s_dynamicMaxBodySize);
	std::printf("Token of /upload and /files/private: %s\n", qPrintable(settings.token));
	if (!settings.tlsSamplesDirectory.isEmpty()){
		std::printf("Test certificates: %s (ca, single, chain, several, bundle, ec, wrong-host); a client trusts the files from the settings of the server\n", qPrintable(settings.tlsSamplesDirectory));
	}

	std::printf("Press Ctrl+C to stop\n");
	std::fflush(stdout);

	std::signal(SIGINT, OnSignal);
	std::signal(SIGTERM, OnSignal);

	// The handler of a signal can do almost nothing, the event loop looks at the flag
	QTimer stopTimer;
	QObject::connect(&stopTimer, &QTimer::timeout, &application, [](){
		if (s_isStopRequested){
			QCoreApplication::quit();
		}
	});
	stopTimer.start(100);

	const int result = application.exec();

	server.Stop();

	return result;
}

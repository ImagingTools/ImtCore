// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
//
// ImtRestDemoClient: the demo client of imtrest with a QML interface. It sends the requests to the demo server (ImtRestDemoServer) or
// to any HTTP server and shows the debug preview: what the application has sent and got (the exchanges) and the exact bytes on
// the wire. The logic is in imtrestdemo::CDemoClientController, the QML only shows it.
//
// Arguments:
//   --server-url <url>   the server (the default is http://localhost:7400)
//   --token <token>      the token of the demo server
//   --no-capture         do not capture the wire (the client talks to the server directly)
//   --probe              ask the server for /info, print the result, exit with 0 if the server has answered (for the checks without a person)
//   --send <json>        send a request (the options of sendRequest(): method, path, bodyType, bodyFile, ...), print the status and the speed, exit
//   --language <code>    the language of the interface for this run (en, ru); the language that is chosen in the window is saved
//   --screenshot-language <code>  with --screenshot: switch to the language after the requests are done (the check of the switch at run time)
//   --screenshot <file>  send a few requests, save the picture of the window to the file and exit (for the check of the interface without a person)

// Qt includes
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QTimer>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickWindow>
#include <QtQuickControls2/QQuickStyle>

// STL includes
#include <cstdio>

// ImtCore includes
#include <imtrestdemo/CDemoClientController.h>
#include <imtrestdemo/CLanguageController.h>
#include <imtrestdemo/DemoTlsTools.h>


int main(int argc, char* argv[])
{
	imtrestdemo::DemoTlsTools::PrepareOpensslBackend(argv);

	Q_INIT_RESOURCE(imtrestdemoclient);

	QGuiApplication application(argc, argv);
	QGuiApplication::setApplicationName("ImtRestDemoClient");
	QGuiApplication::setOrganizationName("ImagingTools");

	QCommandLineParser parser;
	parser.setApplicationDescription("Demo client of imtrest");
	parser.addHelpOption();

	QCommandLineOption serverOption("server-url", "URL of the server", "url", "http://localhost:7400");
	QCommandLineOption tokenOption("token", "Token of the demo server", "token", "demo-token");
	QCommandLineOption noCaptureOption("no-capture", "Do not capture the wire");
	QCommandLineOption probeOption("probe", "Ask the server for /info and exit");
	QCommandLineOption screenshotOption("screenshot", "Send a few requests, save the picture of the window and exit", "file");
	QCommandLineOption screenshotTabOption("screenshot-tab", "Tab of the debug preview in the picture: 0 - exchanges, 1 - wire", "index", "0");

	QCommandLineOption languageOption("language", "Language of the interface for this run (en, ru)", "code");
	QCommandLineOption screenshotMainTabOption("screenshot-main-tab", "Tab of the upper part in the picture (0 - scenarios, 4 - settings of the server)", "index", "0");

	QCommandLineOption screenshotLanguageOption("screenshot-language", "With --screenshot: switch to the language after the requests are done", "code");

	QCommandLineOption sendOption("send", "Send a request (the options of sendRequest() as JSON), print the result and exit", "json");

	parser.addOptions({serverOption, tokenOption, noCaptureOption, probeOption, screenshotOption, screenshotTabOption, screenshotMainTabOption, screenshotLanguageOption, languageOption, sendOption});
	parser.process(application);

	// The style that can be changed by the application
	QQuickStyle::setStyle("Basic");

	imtrestdemo::CDemoClientController controller;
	controller.SetServerUrl(parser.value(serverOption));
	controller.SetToken(parser.value(tokenOption));
	controller.SetCaptureEnabled(!parser.isSet(noCaptureOption));

	// The language: the one that was chosen the last time, or the language of the command line for this run only
	imtrestdemo::CLanguageController languageController;
	languageController.SetTranslations(":/Translations", "imtrestdemoclient");
	languageController.LoadSavedLanguage();
	if (parser.isSet(languageOption)){
		languageController.SetLanguageTemporarily(parser.value(languageOption));
	}

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("client", &controller);
	engine.rootContext()->setContextProperty("language", &languageController);

	// The texts of the interface are made again, the logs ask their texts again
	QObject::connect(&languageController, &imtrestdemo::CLanguageController::languageChanged, &engine, [&engine, &controller](){
		engine.retranslate();
		controller.Retranslate();
	});
	engine.load(QUrl("qrc:/Qml/Main.qml"));

	if (engine.rootObjects().isEmpty()){
		std::fprintf(stderr, "The interface is not loaded\n");

		return 2;
	}

	if (parser.isSet(probeOption)){
		QObject::connect(&controller, &imtrestdemo::CDemoClientController::exchangeFinished, &application, [&controller](){
			const imtrestdemo::SDemoExchange* infoPtr = controller.GetExchanges()->FindExchange(1);
			const bool isConnected = infoPtr != nullptr && infoPtr->statusCode == 200;

			std::printf("%s\n", qPrintable(controller.GetStatusText()));
			std::printf("Exchanges in the log: %d, entries of the wire: %d\n", controller.GetExchanges()->rowCount(), controller.GetWire()->rowCount());
			std::fflush(stdout);

			QCoreApplication::exit(isConnected ? 0 : 1);
		});

		QTimer::singleShot(0, &controller, &imtrestdemo::CDemoClientController::testConnection);
		QTimer::singleShot(15000, &application, [](){
			std::printf("There is no answer\n");

			QCoreApplication::exit(1);
		});
	}

	if (parser.isSet(sendOption)){
		const QVariantMap options = QJsonDocument::fromJson(parser.value(sendOption).toUtf8()).object().toVariantMap();

		QObject::connect(&controller, &imtrestdemo::CDemoClientController::exchangeFinished, &application, [&controller](int exchangeId){
			const imtrestdemo::SDemoExchange* exchangePtr = controller.GetExchanges()->FindExchange(exchangeId);
			if (exchangePtr == nullptr){
				QCoreApplication::exit(1);

				return;
			}

			const qint64 bytes = qMax(exchangePtr->requestBodySize, exchangePtr->responseSize);
			std::printf("%d %s in %lld ms, %.1f MiB/s\n", exchangePtr->statusCode, qPrintable(exchangePtr->errorText), exchangePtr->durationMs,
						double(bytes) / (1024 * 1024) / (double(qMax<qint64>(1, exchangePtr->durationMs)) / 1000));
			std::fflush(stdout);

			QCoreApplication::exit(exchangePtr->statusCode > 0 ? 0 : 1);
		});

		QTimer::singleShot(0, &controller, [&controller, options](){
			controller.sendRequest(options);
		});
	}

	if (parser.isSet(screenshotOption)){
		QTimer::singleShot(0, &controller, [&controller](){
			controller.testConnection();

			QVariantMap echo;
			echo.insert("method", "GET");
			echo.insert("path", "/echo?name=value");
			echo.insert("headers", "X-Debug: preview");
			controller.sendRequest(echo);

			QVariantMap range;
			range.insert("method", "GET");
			range.insert("path", "/files/hello.txt");
			range.insert("headers", "Range: bytes=0-4");
			controller.sendRequest(range);

			QVariantMap upload;
			upload.insert("method", "PUT");
			upload.insert("path", "/upload/screenshot.bin");
			upload.insert("useToken", true);
			upload.insert("bodyType", "generated");
			upload.insert("bodySize", 1048576);
			upload.insert("isChunked", true);
			controller.sendRequest(upload);
		});

		if (parser.isSet(screenshotLanguageOption)){
			QTimer::singleShot(1500, &languageController, [&languageController, &parser, &screenshotLanguageOption](){
				languageController.SetLanguageTemporarily(parser.value(screenshotLanguageOption));
			});
		}

		QTimer::singleShot(2500, &application, [&engine, &parser, &screenshotOption, &screenshotTabOption, &screenshotMainTabOption](){
			QQuickWindow* windowPtr = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
			QObject* mainTabsPtr = windowPtr != nullptr ? windowPtr->findChild<QObject*>("mainTabs") : nullptr;
			if (mainTabsPtr != nullptr){
				mainTabsPtr->setProperty("currentIndex", parser.value(screenshotMainTabOption).toInt());
			}

			QObject* previewPtr = windowPtr != nullptr ? windowPtr->findChild<QObject*>("debugPreview") : nullptr;
			if (previewPtr != nullptr){
				previewPtr->setProperty("currentTab", parser.value(screenshotTabOption).toInt());
			}

			// The window shows the new tab on the next frame
			QTimer::singleShot(500, [windowPtr, &parser, &screenshotOption](){
				const bool isSaved = windowPtr != nullptr && windowPtr->grabWindow().save(parser.value(screenshotOption));

				QCoreApplication::exit(isSaved ? 0 : 1);
			});
		});
	}

	return application.exec();
}

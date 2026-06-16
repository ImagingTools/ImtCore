#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QGuiApplication>
#include <QSet>
#include <QStringList>
#include <QUrl>
#include <QtQml>
#include <QQmlApplicationEngine>

#include <imtbase/CTreeItemModel.h>
#include <imtqml/CQmlWebSocket.h>
#include <imtqml/CDataModelController.h>
#include <imtqml/CDocumentServiceController.h>
#include <imtqml/CFileIO.h>
#include <imtqml/CGqlModel.h>
#include <imtqml/CGqlRequest.h>
#include <imtqml/CNetworkEventInterceptor.h>
#include <imtqml/CQmlProcess.h>
#include <imtqml/CRemoteFileController.h>

namespace
{
void registerImtCoreQmlTypes()
{
    qmlRegisterModule("com.imtcore.imtqml", 1, 0);

    qmlRegisterType<imtbase::CTreeItemModel>("com.imtcore.imtqml", 1, 0, "TreeItemModel");
    qmlRegisterType<imtqml::CQmlWebSocket>("com.imtcore.imtqml", 1, 0, "WebSocket");
    qmlRegisterType<imtqml::CGqlModel>("com.imtcore.imtqml", 1, 0, "GqlModel");
    qmlRegisterType<imtqml::CGqlRequest>("com.imtcore.imtqml", 1, 0, "GqlRequest");
    qmlRegisterType<imtqml::CRemoteFileController>("com.imtcore.imtqml", 1, 0, "RemoteFileController");
    qmlRegisterType<imtqml::FileIO>("com.imtcore.imtqml", 1, 0, "FileIO");
    qmlRegisterType<imtqml::CQmlProcess>("com.imtcore.imtqml", 1, 0, "Process");
    qmlRegisterSingletonInstance<imtqml::CNetworkEventInterceptor>(
        "com.imtcore.imtqml", 1, 0, "NetworkEventInterceptor", imtqml::CNetworkEventInterceptor::Instance());
    qmlRegisterType<imtqml::CDocumentServiceController>("com.imtcore.imtqml", 1, 0, "DocumentServiceController");
    qmlRegisterType<imtqml::CDataModelController>("com.imtcore.imtqml", 1, 0, "DataModelController");
}

void registerCompatibilityModules()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    qmlRegisterModule("QtGraphicalEffects", 1, 12);
    qmlRegisterModule("QtGraphicalEffects", 1, 0);
    qmlRegisterModule("QtQuick.Dialogs", 1, 3);
#else
    qmlRegisterModule("QtQuick.Dialogs", 6, 2);
    qmlRegisterModule("Qt5Compat.GraphicalEffects", 6, 0);
#endif
}

void addIfDirectoryExists(QSet<QString>& paths, const QString& dirPath)
{
    if (dirPath.isEmpty()) {
        return;
    }

    const QDir dir(dirPath);
    if (dir.exists()) {
        paths.insert(dir.absolutePath());
    }
}

void collectQmlFoldersRecursively(QSet<QString>& paths, const QString& rootPath)
{
    if (rootPath.isEmpty()) {
        return;
    }

    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        return;
    }

    QDirIterator it(rootPath, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        const QFileInfo info = it.fileInfo();
        const QString name = info.fileName();

        if (name.compare("qml", Qt::CaseInsensitive) == 0 || name.compare("Qml", Qt::CaseInsensitive) == 0) {
            paths.insert(info.absoluteFilePath());
        }
    }
}

bool shouldScanRecursively()
{
    const QString value = qEnvironmentVariable("JQML_HOST_SCAN_RECURSIVE").trimmed().toLower();
    return value == "1" || value == "true" || value == "yes";
}

QSet<QString> collectImportPaths(const QStringList& extraImportPaths)
{
    QSet<QString> result;

    const QString imtcoreDir = qEnvironmentVariable("IMTCOREDIR");
    const QString prolifeDir = qEnvironmentVariable("PROLIFEDIR");
    const QString itRoot = qEnvironmentVariable("IT_QML_ROOT");
    const QString targetName = qEnvironmentVariable("TARGETNAME");

    const QStringList roots = { imtcoreDir, prolifeDir, itRoot };

    const bool recursiveScanEnabled = shouldScanRecursively();

    for (const QString& root : roots) {
        addIfDirectoryExists(result, root);
        addIfDirectoryExists(result, root + "/Qml");
        addIfDirectoryExists(result, root + "/qml");

        if (!targetName.isEmpty()) {
            addIfDirectoryExists(result, root + "/AuxInclude/" + targetName + "/GeneratedFiles");
        }

        if (recursiveScanEnabled) {
            collectQmlFoldersRecursively(result, root);
        }
    }

    addIfDirectoryExists(result, imtcoreDir + "/Include/imtstylecontrolsqml/Qml");
    addIfDirectoryExists(result, imtcoreDir + "/Qml");

    for (const QString& importPath : extraImportPaths) {
        addIfDirectoryExists(result, importPath);
    }

    return result;
}
}

int main(int argc, char* argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    app.setApplicationName("JqmlDesktopHost");

    QCommandLineParser parser;
    parser.setApplicationDescription("Minimal desktop host for JQML tests with ImtCore QML type registration");
    parser.addHelpOption();

    QCommandLineOption qmlOption(QStringList() << "f" << "qml-file", "Path to QML file", "qmlFile");
    parser.addOption(qmlOption);

    QCommandLineOption importPathOption(QStringList() << "I" << "import-path", "Extra QML import path", "path");
    parser.addOption(importPathOption);

    parser.addPositionalArgument("qmlFile", "Path to QML file (same as --qml-file)");
    parser.process(app);

    registerCompatibilityModules();
    registerImtCoreQmlTypes();

    QString qmlPath = parser.value(qmlOption).trimmed();
    if (qmlPath.isEmpty()) {
        const QStringList positional = parser.positionalArguments();
        if (!positional.isEmpty()) {
            qmlPath = positional.first().trimmed();
        }
    }

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/");

    const QSet<QString> importPaths = collectImportPaths(parser.values(importPathOption));
    for (const QString& importPath : importPaths) {
        engine.addImportPath(importPath);
        qInfo() << "QML import path added:" << importPath;
    }

    QUrl url(QStringLiteral("qrc:/qml/EmptyHost.qml"));
    if (!qmlPath.isEmpty()) {
        const QFileInfo qmlFileInfo(qmlPath);
        if (!qmlFileInfo.exists()) {
            qCritical() << "QML file not found:" << qmlPath;
            return -2;
        }
        url = QUrl::fromLocalFile(qmlFileInfo.absoluteFilePath());
    }

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject* object, const QUrl& objectUrl) {
            if (!object && objectUrl == url) {
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}

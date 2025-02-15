// Qt includes
#include <QtCore/QDirIterator>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include <QtQml/QQmlEngine>

// ACF includes
#include <ibase/IApplication.h>

// ImtCore includes
#include <imtstyle/CImtStyle.h>
#include <imtbase/CTreeItemModel.h>
#include <imtqml/CGqlModel.h>
#include <imtqml/CGqlRequest.h>
#include <imtqml/CRemoteFileController.h>
#include <imtqml/CFileIO.h>
#include <imtqml/CNetworkEventInterceptor.h>


class DefaultImtCoreQmlInitializer
{
public:
	static void Init()
	{
		InitImtCoreLocalization();
		InitImtCoreResources();
		InitStyle();
		InitTheme();
		InitImtCoreSdl();
		InitQml();
	}

	static void InitAcfLocalization()
	{
		Q_INIT_RESOURCE(AcfLoc);
		Q_INIT_RESOURCE(AcfSlnLoc);
	}

	static void InitImtCoreLocalization()
	{
		InitAcfLocalization();
		Q_INIT_RESOURCE(ImtCoreLoc);
	}

	static void InitImtCoreSdl()
	{
		Q_INIT_RESOURCE(imtbaseImtCollectionSdl);
		Q_INIT_RESOURCE(imtbaseComplexCollectionFilterSdl);
		Q_INIT_RESOURCE(imtbaseDocumentRevisionSdl);
		Q_INIT_RESOURCE(imtauthGroupsSdl);
		Q_INIT_RESOURCE(imtauthRolesSdl);
		Q_INIT_RESOURCE(imtauthUsersSdl);
		Q_INIT_RESOURCE(imtauthProfileSdl);
//		Q_INIT_RESOURCE(imtauthSessionsSdl);
		Q_INIT_RESOURCE(imtauthAuthorizationOptionsSdl);
		Q_INIT_RESOURCE(imtauthAuthorizationSdl);
		Q_INIT_RESOURCE(imtappApplicationSdl);
	}

	static void InitQml()
	{
		Q_INIT_RESOURCE(imtcontrolsqml);
		Q_INIT_RESOURCE(imtguiqml);
		Q_INIT_RESOURCE(imtcolguiqml);
		Q_INIT_RESOURCE(imtdocguiqml);
		Q_INIT_RESOURCE(imtguigqlqml);
		Q_INIT_RESOURCE(imtauthguiqml);

		qmlRegisterType<imtbase::CTreeItemModel>("Acf", 1, 0, "TreeItemModel");
		qmlRegisterType<imtqml::CGqlModel>("Acf", 1, 0, "GqlModel");
		qmlRegisterType<imtqml::CGqlRequest>("Acf", 1, 0, "GqlRequest");
		qmlRegisterType<imtqml::CRemoteFileController>("Acf", 1, 0, "RemoteFileController");
		qmlRegisterType<imtqml::FileIO>("Acf", 1, 0, "FileIO");
		qmlRegisterSingletonInstance<imtqml::CNetworkEventInterceptor>("Acf", 1, 0, "NetworkEventInterceptor", imtqml::CNetworkEventInterceptor::Instance());

	#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		qmlRegisterModule("QtGraphicalEffects", 1, 12);
		qmlRegisterModule("QtGraphicalEffects", 1, 0);
		qmlRegisterModule("QtQuick.Dialogs", 1, 3);
	#else
		qmlRegisterModule("QtQuick.Dialogs", 6, 2);
		qmlRegisterModule("Qt5Compat.GraphicalEffects", 6, 0);
	#endif
	}

	static void InitTheme()
	{
		Q_INIT_RESOURCE(imtguiTheme);
		Q_INIT_RESOURCE(imtauthguiTheme);
	}

	static void InitImtCoreResources()
	{
		Q_INIT_RESOURCE(imtstylecontrolsqml);

		Q_INIT_RESOURCE(imtauthguilight);
		Q_INIT_RESOURCE(imtauthguidark);

		Q_INIT_RESOURCE(imtlicguilight);
		Q_INIT_RESOURCE(imtlicguidark);

		Q_INIT_RESOURCE(imtgui);
		Q_INIT_RESOURCE(imtguilight);
		Q_INIT_RESOURCE(imtguidark);

		Q_INIT_RESOURCE(imtstyle);
		Q_INIT_RESOURCE(imtstylelight);
		Q_INIT_RESOURCE(imtstyledark);



		Q_INIT_RESOURCE(imtdb);
	}

	static void InitStyle()
	{
		imtstyle::CImtStyle* imtStylePtr = imtstyle::CImtStyle::GetInstance();
		Q_ASSERT(imtStylePtr != nullptr);
		imtStylePtr->SetStyleType(imtstyle::CImtStyle::ST_IMAGINGTOOLS);
	
		imtStylePtr->setBaseStyle(QStyleFactory::create("fusion"));
		QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
		QApplication::setStyle(imtStylePtr);

		QCoreApplication* appPtr = QCoreApplication::instance();
		if (appPtr != nullptr){
			appPtr->setProperty("ThemeId", "Light");
		}
	}
};

template <class ApplicationComposite, class AppInitializer, bool autoInit = false>
int Run(int argc, char *argv[])
{
	AppInitializer::Init();

	QDirIterator dirIter(":/Style");
	while (dirIter.hasNext()){
		qDebug() << dirIter.filePath();

		dirIter.next();
	}

	QDirIterator dirIter3(":/Icons");
	while (dirIter3.hasNext()){
		qDebug() << dirIter3.filePath();

		dirIter3.next();
	}

	QDirIterator dirIter2(":/Styles");
	while (dirIter2.hasNext()){
		qDebug() << dirIter2.filePath();

		dirIter2.next();
	}

	ApplicationComposite instance;

	if (autoInit){
		instance.EnsureAutoInitComponentsCreated();
	}

	ibase::IApplication* applicationPtr = instance.template GetInterface<ibase::IApplication>();
	if (applicationPtr != nullptr){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}



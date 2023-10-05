// Qt includes
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include <QQuickStyle>

// ACF includes
#include <ibase/IApplication.h>

// ImtCore includes
#include <imtstyle/CImtStyle.h>
#include <imtbase/CTreeItemModel.h>
#include <imtqml/CGqlModel.h>
#include <imtqml/CRemoteFileController.h>
#include <imtqml/CQuickApplicationComp.h>

#include <GeneratedFiles/ImtControlsGallery/CImtControlsGallery.h>


int main(int argc, char *argv[])
{
#ifdef WEB_COMPILE
	Q_INIT_RESOURCE(ImtControlsGalleryWeb);
#endif

    Q_INIT_RESOURCE(imtguigql);
    Q_INIT_RESOURCE(imtqml);
    Q_INIT_RESOURCE(imtstyle);
    Q_INIT_RESOURCE(imtstylecontrolsqml);
    Q_INIT_RESOURCE(imtcontrols);
    Q_INIT_RESOURCE(imtgui);
    Q_INIT_RESOURCE(imtauthgui);

	Q_INIT_RESOURCE(imtresthtml);
	Q_INIT_RESOURCE(ImtControlsGallery);
    Q_INIT_RESOURCE(imtguiTheme);


//	Q_INIT_RESOURCE(Webimt3dgui);
//    Q_INIT_RESOURCE(Webimtauthgui);
//    Q_INIT_RESOURCE(Webimtgui);
//	Q_INIT_RESOURCE(Webimtcontrols);
//	Q_INIT_RESOURCE(Webimtlicgui);

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	// imtstyle::CImtStyle* imtStylePtr = imtstyle::CImtStyle::GetInstance();
	// Q_ASSERT(imtStylePtr != nullptr);
	
	CImtControlsGallery instance;

	qmlRegisterType<imtbase::CTreeItemModel>("Acf", 1, 0, "TreeItemModel");
	qmlRegisterType<imtqml::CGqlModel>("Acf", 1, 0, "GqlModel");
	qmlRegisterType<imtqml::CRemoteFileController>("Acf", 1, 0, "RemoteFileController");

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	qmlRegisterModule("QtGraphicalEffects", 1, 12);
	qmlRegisterModule("QtGraphicalEffects", 1, 0);
	qmlRegisterModule("QtQuick.Dialogs", 1, 3);
#else
	qmlRegisterModule("QtQuick.Dialogs", 6, 2);
	qmlRegisterModule("Qt5Compat.GraphicalEffects", 1, 0);
#endif

	QQuickStyle::setStyle("Material");

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != nullptr){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}



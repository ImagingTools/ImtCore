// Qt includes
#include <QtQuick/QQuickWindow>

// ACF includes
#include <ibase/IApplication.h>

// ImtCore includes
#include <imtbase/Init.h>
#include <GeneratedFiles/ControlsGallery/CControlsGallery.h>



int main(int argc, char *argv[])
{
#ifdef WEB_COMPILE
	Q_INIT_RESOURCE(controlsgalleryqmlWeb);
#endif
	Q_INIT_RESOURCE(controlsgalleryqml);
	Q_INIT_RESOURCE(controlsgalleryContactInfosSdl);
	Q_INIT_RESOURCE(controlsgalleryContactInfoCollectionDocumentManagerSdl);
	Q_INIT_RESOURCE(imtguivars);

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
	QQuickWindow::setGraphicsApi(QSGRendererInterface::Direct3D12);
#endif
#endif

	return Run<CControlsGallery, DefaultImtCoreQmlInitializer>(argc, argv);
}



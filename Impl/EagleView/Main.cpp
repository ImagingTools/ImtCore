// Qt includes
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>

// ACF includes
#include <ibase/IApplication.h>
#include <GeneratedFiles/EagleView/CEagleView.h>


int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(imt3dgui);
	Q_INIT_RESOURCE(AcfLoc);
	Q_INIT_RESOURCE(AcfSlnLoc);
	Q_INIT_RESOURCE(EagleView);

	CEagleView instance;

	QApplication::setStyle("fusion");

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != NULL){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}



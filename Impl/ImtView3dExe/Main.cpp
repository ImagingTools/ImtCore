// Qt includes
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>

// ACF includes
#include <ibase/IApplication.h>
#include <GeneratedFiles/ImtView3d/CImtView3d.h>


int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(imtgui3d);
	Q_INIT_RESOURCE(AcfLoc);
	Q_INIT_RESOURCE(AcfSlnLoc);
	Q_INIT_RESOURCE(ImtView3dLoc);
	Q_INIT_RESOURCE(CImtView3d);
	Q_INIT_RESOURCE(iqtgui);

	CImtView3d instance;

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != NULL){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}



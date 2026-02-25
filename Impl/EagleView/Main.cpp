// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
// Qt includes
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>

// ACF includes
#include <ibase/IApplication.h>
#include <GeneratedFiles/EagleView/CEagleView.h>

// ImtCore includes
#include <imtbase/Init.h>
#include <imtstyle/CImtStyle.h>


int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(EagleView);

	Q_INIT_RESOURCE(imt3dgui);
	Q_INIT_RESOURCE(imt3dguilight);
	Q_INIT_RESOURCE(imt3dguidark);

	Q_INIT_RESOURCE(imtguiTheme);

	Q_INIT_RESOURCE(imtgui);
	Q_INIT_RESOURCE(imtguilight);
	Q_INIT_RESOURCE(imtguidark);

	Q_INIT_RESOURCE(imtstyle);
	Q_INIT_RESOURCE(imtstylelight);
	Q_INIT_RESOURCE(imtstyledark);

	CEagleView instance;

	QApplication::setStyle("fusion");

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != NULL){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}



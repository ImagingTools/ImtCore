// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/Init.h>
#include <imtlic/IProductInfo.h>


template <class ApplicationComposite, class AppInitializer, void (*ProductFiller)(imtlic::IProductInfo&) = nullptr, bool autoInit = false>
int ProductFeatureRun(int argc, char* argv[])
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
		imtlic::IProductInfo* productInfoPtr = instance.template GetInterface<imtlic::IProductInfo>();
		if (productInfoPtr != nullptr && ProductFiller != nullptr){
			ProductFiller(*productInfoPtr);
		}

		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}



#pragma once

// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>

// ImtCore includes
#include <imtstyle/CImtStyle.h>


inline void InitializeImtCoreStyle()
{
	auto* imtStylePtr = imtstyle::CImtStyle::GetInstance();
	Q_ASSERT(imtStylePtr != nullptr);
	imtStylePtr->SetStyleType(imtstyle::CImtStyle::ST_IMAGINGTOOLS);

	imtStylePtr->setBaseStyle(QStyleFactory::create("fusion"));
	QApplication::setStyle(imtStylePtr);

	auto* appPtr = QCoreApplication::instance();
	if (appPtr != nullptr) {
		appPtr->setProperty("ThemeId", QVariant("Light"));
	}
}

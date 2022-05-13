#pragma once

// Qt includes
#include <QtCore/QTranslator>

// ACF includes
#include <icomp/CComponentBase.h>

//ImtCore includes
#include <imtgql/CGqlObject.h>


namespace imtqml
{


/**
	Client options provider
*/
class CUserOptionsProviderCompBase:
		public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	QByteArray GetLanguageIdFromInputParams(const QList<imtgql::CGqlObject>& inputParams) const;
	QByteArray GetDesignSchemeIdFromInputParams(const QList<imtgql::CGqlObject>& inputParams) const;

	CUserOptionsProviderCompBase();
};


} // namespace imtqml



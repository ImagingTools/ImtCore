#pragma once

//ImtCore includes
#include <imtgql/CGqlObject.h>


namespace imtqml
{


/**
	Client options provider
*/
class CUserOptionsProviderBase
{

public:
	QByteArray GetLanguageIdFromInputParams(const QList<imtgql::CGqlObject>& inputParams) const;
	QByteArray GetDesignSchemeIdFromInputParams(const QList<imtgql::CGqlObject>& inputParams) const;
};


} // namespace imtqml

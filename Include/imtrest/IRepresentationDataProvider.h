#pragma once

// ACF includes
#include <istd/IPolymorphic.h>
#include <imtbase/CTreeItemModel.h>

// QtCore includes
#include <imtrest/imtrest.h>


namespace imtrest
{


/**
	Base interface for all data providers
	\ingroup LicenseManagement
*/
class IRepresentationDataProvider: virtual public istd::IPolymorphic
{
public:
	enum Format
	{
		RF_JSON,
		RF_XML
	};

	virtual bool GetRepresentationData(
				Format format,
				QByteArray& representationData,
				const QList<QByteArray>& query, const imtrest::QweryParams& params) = 0;
	virtual bool SetRepresentationData(
				Format format,
				QByteArray& representationData,
				const QByteArray& commandId = "") = 0;
};


} // namespace imtauthgui



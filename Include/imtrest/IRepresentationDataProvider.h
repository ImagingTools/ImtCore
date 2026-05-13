// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
				const QList<QByteArray>& query,
				const imtrest::QueryParams& params) = 0;
	virtual bool SetRepresentationData(
				Format format,
				QByteArray& representationData,
				const QByteArray& commandId = "") = 0;
};


} // namespace imtauthgui



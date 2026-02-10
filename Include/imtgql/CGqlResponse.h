// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlResponse.h>
#include <imtgql/TGraphQlJsonSerializerWrap.h>


namespace imtgql
{


class CGqlResponseBase: virtual public IGqlResponse
{
public:
	explicit CGqlResponseBase(const GqlRequestPtr& originalRequestPtr);

	void SetResponseData(const QByteArray& data);

	// reimplemented (IGqlResponse)
	virtual GqlRequestPtr GetOriginalRequest() const override;
	virtual QByteArray GetResponseData() const override;

protected:
	GqlRequestPtr m_originalRequestPtr;
	QByteArray m_responseData;
};


typedef TGraphQlJsonSerializerWrap<CGqlResponseBase> CGqlResponse;


} // namespace imtgql



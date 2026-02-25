// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CGqlResponse.h"


namespace imtgql
{


// public methods

CGqlResponseBase::CGqlResponseBase(const GqlRequestPtr& originalRequestPtr)
	:m_originalRequestPtr(originalRequestPtr)
{
}


void CGqlResponseBase::SetResponseData(const QByteArray& data)
{
	m_responseData = data;
}


// reimplemented (IGqlResponse)

IGqlResponse::GqlRequestPtr CGqlResponseBase::GetOriginalRequest() const
{
	return m_originalRequestPtr;
}


QByteArray CGqlResponseBase::GetResponseData() const
{
	return m_responseData;
}


} // namespace imtgql



#include "CGqlResponse.h"


namespace imtgql
{


// public methods

CGqlResponse::CGqlResponse(GqlRequestPtr originalRequestPtr)
	:m_originalRequestPtr(originalRequestPtr)
{
}


void CGqlResponse::SetResponseData(const QByteArray& data)
{
	m_responseData = data;
}


// reimplemented (IGqlResponse)

IGqlResponse::GqlRequestPtr CGqlResponse::GetOriginalRequest() const
{
	return m_originalRequestPtr;
}


QByteArray CGqlResponse::GetResponseData() const
{
	return m_responseData;
}


} // namespace imtgql



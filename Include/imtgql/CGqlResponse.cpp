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



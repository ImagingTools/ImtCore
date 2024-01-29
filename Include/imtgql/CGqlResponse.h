#pragma once


// ImtCore includes
#include <imtgql/IGqlResponse.h>


namespace imtgql
{


class CGqlResponse : virtual public IGqlResponse
{
public:
	explicit CGqlResponse(GqlRequestPtr originalRequestPtr);

	void SetResponseData(const QByteArray& data);

	// reimplemented (IGqlResponse)
	virtual GqlRequestPtr GetOriginalRequest() const override;
	virtual QByteArray GetResponseData() const override;

protected:
	GqlRequestPtr m_originalRequestPtr;
	QByteArray m_responseData;
};


} // namespace imtgql



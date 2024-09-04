#pragma once


// ImtCore includes
#include <imtgql/IGqlResponse.h>
#include <imtgql/TGraphQlJsonSerializerWrap.h>


namespace imtgql
{


class CGqlResponseBase: virtual public IGqlResponse
{
public:
	explicit CGqlResponseBase(GqlRequestPtr originalRequestPtr);

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



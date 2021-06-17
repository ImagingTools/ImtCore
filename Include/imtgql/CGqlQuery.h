#pragma once


// ImtCore includes
#include <imtgql/CGqlRequestBase.h>


namespace imtgql
{


class CGqlQuery: public CGqlRequestBase
{
public:
	typedef CGqlRequestBase BaseClass;

	CGqlQuery(const QByteArray& commandId = QByteArray());

	// reimplemented (IGqlRequest)
	virtual RequestType GetRequestType() const override;
	virtual QByteArray GetQuery() const override;

protected:
	virtual QString CreateFieldQueryPart() const;
};


} // namespace imtgql



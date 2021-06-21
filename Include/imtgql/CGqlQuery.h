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

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

protected:
	virtual QString CreateFieldQueryPart() const;
	virtual QString CreateQueryParams() const;
};


} // namespace imtgql



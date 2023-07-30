#pragma once


// ImtCore includes
#include <imtbase/IOperationContext.h>


namespace imtbase
{


class COperationContext : virtual public imtbase::IOperationContext
{
public:
	void SetOperationOwnerId(IdentifableObjectInfo operationOwnerId);
	void SetOperationDescription(const QString& operationDescription);

	// reimplemented (imtbase::IOperationContext)
	virtual IdentifableObjectInfo GetOperationOwnerId() const override;
	virtual QString GetOperationDescription() const override;

private:
	IdentifableObjectInfo m_operationOwnerId;
	QString m_operationDescription;
};


} // namespace imtbase



#pragma once


// ImtCore includes
#include <imtbase/IOperationContext.h>
#include <imtbase/CObjectCollection.h>


namespace imtbase
{


class COperationContext: virtual public imtbase::IOperationContext
{
public:
	COperationContext();

	void SetOperationOwnerId(IdentifableObjectInfo operationOwnerId);
	void SetOperationDescription(const QString& operationDescription);

	// reimplemented (imtbase::IOperationContext)
	virtual IdentifableObjectInfo GetOperationOwnerId() const override;
	virtual QString GetOperationDescription() const override;
	virtual imtbase::IObjectCollection* GetChangesCollection() override;

private:
	IdentifableObjectInfo m_operationOwnerId;
	QString m_operationDescription;
	imtbase::CObjectCollection m_changesCollection;
};


} // namespace imtbase



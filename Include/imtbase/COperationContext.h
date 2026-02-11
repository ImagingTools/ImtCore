// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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

	void SetOperationOwnerId(const imtbase::IOperationContext::IdentifableObjectInfo& operationOwnerId);
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



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/COperationContext.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TSingleFactory.h>

// ImtCore includes
#include <imtbase/COperationDescription.h>


namespace imtbase
{


// public methods

COperationContext::COperationContext()
	:m_tenantType(TT_ORGANIZATION)
{
	typedef istd::TSingleFactory<istd::IChangeable, imtbase::COperationDescription> FactoryOperationDescriptionImpl;
	m_changesCollection.RegisterFactory<FactoryOperationDescriptionImpl>("OperationInfo");
}


void COperationContext::SetOperationOwnerId(const imtbase::IOperationContext::IdentifableObjectInfo& operationOwnerId)
{
	if (m_operationOwnerId != operationOwnerId){
		istd::CChangeNotifier changeNotifier(this);

		m_operationOwnerId = operationOwnerId;
	}
}


void COperationContext::SetTenantId(const QByteArray& tenantId, TenantType tenantType)
{
	if (m_tenantId != tenantId || m_tenantType != tenantType){
		istd::CChangeNotifier changeNotifier(this);

		m_tenantId = tenantId;
		m_tenantType = tenantType;
	}
}


void COperationContext::SetOperationDescription(const QString& operationDescription)
{
	if (m_operationDescription != operationDescription){
		istd::CChangeNotifier changeNotifier(this);

		m_operationDescription = operationDescription;
	}
}


void COperationContext::SetChangeSetId(const QByteArray& changeSetId)
{
	if (m_changeSetId != changeSetId){
		istd::CChangeNotifier changeNotifier(this);

		m_changeSetId = changeSetId;
	}
}


// reimplemented (imtbase::IOperationContext)

imtbase::IOperationContext::IdentifableObjectInfo COperationContext::GetOperationOwnerId() const
{
	return m_operationOwnerId;
}


QByteArray COperationContext::GetTenantId() const
{
	return m_tenantId;
}


TenantType COperationContext::GetTenantType() const
{
	return m_tenantType;
}


QString COperationContext::GetOperationDescription() const
{
	return m_operationDescription;
}


imtbase::IObjectCollection* COperationContext::GetChangesCollection()
{
	return &m_changesCollection;
}


// reimplemented (imtbase::IChangeSetContext)

QByteArray COperationContext::GetChangeSetId() const
{
	return m_changeSetId;
}


} // namespace imtbase



#include <imtbase/COperationContext.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

void COperationContext::SetOperationOwnerId(imtbase::IOperationContext::IdentifableObjectInfo operationOwnerId)
{
	if (m_operationOwnerId != operationOwnerId){
		istd::CChangeNotifier changeNotifier(this);

		m_operationOwnerId = operationOwnerId;
	}
}


void COperationContext::SetOperationDescription(const QString& operationDescription)
{
	if (m_operationDescription != operationDescription){
		istd::CChangeNotifier changeNotifier(this);

		m_operationDescription = operationDescription;
	}
}


// reimplemented (imtbase::IOperationContext)

imtbase::IOperationContext::IdentifableObjectInfo COperationContext::GetOperationOwnerId() const
{
	return m_operationOwnerId;
}


QString COperationContext::GetOperationDescription() const
{
	return m_operationDescription;
}


} // namespace imtbase



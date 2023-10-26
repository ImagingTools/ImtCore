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
{
	typedef istd::TSingleFactory<istd::IChangeable, imtbase::COperationDescription> FactoryOperationDescriptionImpl;
	m_changesCollection.RegisterFactory<FactoryOperationDescriptionImpl>("OperationInfo");
}


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


imtbase::IObjectCollection* COperationContext::GetChangesCollection()
{
	return &m_changesCollection;
}


} // namespace imtbase



#pragma once

#include "imtbase/CDocumentChangeGeneratorCompBase.h"


// ACF includes
#include <istd/TSingleFactory.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::IDocumentChangeGenerator)

bool CDocumentChangeGeneratorCompBase::GenerateDocumentChanges(
			const QByteArray& documentId,
			const istd::IChangeable* documentPtr,
			CObjectCollection& documentChangeCollection,
			QString& errorMessage)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to generate document changes. Internal error");

		return false;
	}

	if (documentPtr == nullptr){
		errorMessage = QString("Unable to generate document changes for invalid document.");

		return false;
	}

	typedef istd::TSingleFactory<istd::IChangeable, imtbase::COperationDescription> FactoryOperationDescriptionImpl;
	documentChangeCollection.RegisterFactory<FactoryOperationDescriptionImpl>("OperationInfo");

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(documentId, dataPtr)){
		if (dataPtr.IsValid()){
			istd::TDelPtr<const istd::IChangeable> oldDocumentPtr;
			oldDocumentPtr.SetCastedOrRemove(dataPtr->CloneMe());
			if (!oldDocumentPtr.IsValid()){
				errorMessage = QString("Unable to get document with ID: %1").arg(qPrintable(documentId));

				return false;
			}

			return CompareDocuments(oldDocumentPtr.PopPtr(), documentPtr, documentChangeCollection, errorMessage);
		}
	}

	errorMessage = QString("Unable to get document with ID: %1").arg(qPrintable(documentId));

	return false;
}


imtbase::COperationDescription* CDocumentChangeGeneratorCompBase::CreateOperationDescription(
			const QByteArray& operationTypeId,
			const QByteArray& key,
			const QString& keyName,
			const QByteArray& oldValue,
			const QByteArray& newValue) const
{
	istd::TDelPtr<imtbase::COperationDescription> operationDescriptionPtr;
	operationDescriptionPtr.SetPtr(new imtbase::COperationDescription);

	operationDescriptionPtr->SetOperationTypeId(operationTypeId);
	operationDescriptionPtr->SetKey(key);
	operationDescriptionPtr->SetKeyName(keyName);
	operationDescriptionPtr->SetOldValue(oldValue);
	operationDescriptionPtr->SetNewValue(newValue);

	return operationDescriptionPtr.PopPtr();
}


} // namespace imtlic



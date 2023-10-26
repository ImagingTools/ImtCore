#pragma once

#include "imtbase/CDocumentChangeGeneratorCompBase.h"


// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::IDocumentChangeGenerator)


bool CDocumentChangeGeneratorCompBase::GenerateDocumentChanges(
			int operationType,
			const QByteArray& documentId,
			const istd::IChangeable* documentPtr,
			CObjectCollection& documentChangeCollection,
			QString& errorMessage,
			const iprm::IParamsSet* /*paramsPtr*/)
{
	if (operationType == OperationType::OT_CREATE){
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("Create", "", "", "", ""));
	}
	else if (operationType == OperationType::OT_UPDATE){
		if (!m_objectCollectionCompPtr.IsValid()){
			errorMessage = QString("Unable to generate document changes. Internal error");

			return false;
		}

		if (documentPtr == nullptr){
			errorMessage = QString("Unable to generate document changes for invalid document.");

			return false;
		}

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
	}

	errorMessage = QString("Unable to get document with ID: %1").arg(qPrintable(documentId));

	return false;
}


QString CDocumentChangeGeneratorCompBase::GetOperationDescription(CObjectCollection& documentChangeCollection, const QByteArray& languageId)
{
	QString retVal;
	imtbase::ICollectionInfo::Ids elementIds = documentChangeCollection.GetElementIds();
	for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (documentChangeCollection.GetObjectData(elementId, dataPtr)){
			const imtbase::COperationDescription* operationDescriptionPtr = dynamic_cast<const imtbase::COperationDescription*>(dataPtr.GetPtr());
			if (operationDescriptionPtr != nullptr){
				QByteArray typeId = operationDescriptionPtr->GetOperationTypeId();
				QByteArray key = operationDescriptionPtr->GetKey();
				QByteArray oldValue = operationDescriptionPtr->GetOldValue();
				QByteArray newValue = operationDescriptionPtr->GetNewValue();

				QString keyName = operationDescriptionPtr->GetKeyName();
				keyName = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), keyName.toUtf8(), languageId, "Attribute");

				if (typeId == QByteArray("Create")){
					QString change = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Created the document"), languageId, "imtbase::CDocumentChangeGeneratorCompBase");
					retVal += change + "\n";
				}
				else if (typeId == QByteArray("Change")){
					QString change = imtbase::GetTranslation(
								m_translationManagerCompPtr.GetPtr(),
								QString(QT_TR_NOOP("%1 changed from %2 to %3")).toUtf8(),
								languageId,
								"imtbase::CDocumentChangeGeneratorCompBase");

					change = change.arg(keyName).arg(qPrintable(oldValue)).arg(qPrintable(newValue));

					retVal += change + "\n";
				}
				else if (typeId == QByteArray("Set")){
					QString change = imtbase::GetTranslation(
								m_translationManagerCompPtr.GetPtr(),
								QString(QT_TR_NOOP("%1 was set to %2")).toUtf8(),
								languageId,
								"imtbase::CDocumentChangeGeneratorCompBase");

					change = change.arg(keyName).arg(qPrintable(newValue));

					retVal += change + "\n";
				}
				else if (typeId == QByteArray("Clear")){
					QString change = imtbase::GetTranslation(
								m_translationManagerCompPtr.GetPtr(),
								QString(QT_TR_NOOP("%1 was cleared")).toUtf8(),
								languageId,
								"imtbase::CDocumentChangeGeneratorCompBase");

					change = change.arg(keyName);

					retVal += change + "\n";
				}
			}
		}
	}

	return retVal;
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

	if (operationTypeId.isEmpty()){
		if (!oldValue.isEmpty() && !newValue.isEmpty()){
			operationDescriptionPtr->SetOperationTypeId(QByteArray("Change"));
		}
		else if (oldValue.isEmpty() && !newValue.isEmpty()){
			operationDescriptionPtr->SetOperationTypeId(QByteArray("Set"));
		}
		else if (!oldValue.isEmpty() && newValue.isEmpty()){
			operationDescriptionPtr->SetOperationTypeId(QByteArray("Clear"));
		}
	}

	return operationDescriptionPtr.PopPtr();
}


} // namespace imtlic



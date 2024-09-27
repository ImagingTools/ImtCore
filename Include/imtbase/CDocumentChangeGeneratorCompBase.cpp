#include <imtbase/CDocumentChangeGeneratorCompBase.h>


// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::IDocumentChangeGenerator)


bool CDocumentChangeGeneratorCompBase::GenerateDocumentChanges(
			int operationType,
			const QByteArray& documentId,
			const istd::IChangeable& document,
			CObjectCollection& documentChangeCollection,
			QString& errorMessage,
			const iprm::IParamsSet* /*paramsPtr*/)
{
	if (operationType == imtbase::IOperationDescription::OperationType::OT_CREATE){
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(operationType, "", "", "", ""));
	}
	else if (operationType == imtbase::IOperationDescription::OperationType::OT_UPDATE){
		if (!m_objectCollectionCompPtr.IsValid()){
			errorMessage = QString("Unable to generate document changes. Internal error");

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

				bool ok = CompareDocuments(*oldDocumentPtr.GetPtr(), document, documentChangeCollection, errorMessage);
				return ok;
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
				int typeId = operationDescriptionPtr->GetOperationType();
				QByteArray key = operationDescriptionPtr->GetKey();
				QByteArray oldValue = operationDescriptionPtr->GetOldValue();
				QByteArray newValue = operationDescriptionPtr->GetNewValue();

				QString keyName = operationDescriptionPtr->GetKeyName();
				keyName = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), keyName.toUtf8(), languageId, "Attribute");

				if (typeId == imtbase::IOperationDescription::OT_CREATE){
					QString change = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Created the document"), languageId, "imtbase::CDocumentChangeGeneratorCompBase");
					retVal += change + "\n";
				}
				else if (typeId == imtbase::IOperationDescription::OT_CHANGE){
					QString change = imtbase::GetTranslation(
						m_translationManagerCompPtr.GetPtr(),
						QString(QT_TR_NOOP("'%1' changed from '%2' to '%3'")).toUtf8(),
						languageId,
						"imtbase::CDocumentChangeGeneratorCompBase");

					change = change.arg(keyName).arg(qPrintable(oldValue)).arg(qPrintable(newValue));

					retVal += change + "\n";
				}
				else if (typeId == imtbase::IOperationDescription::OT_SET){
					QString change = imtbase::GetTranslation(
						m_translationManagerCompPtr.GetPtr(),
						QString(QT_TR_NOOP("'%1' was set to '%2'")).toUtf8(),
						languageId,
						"imtbase::CDocumentChangeGeneratorCompBase");

					change = change.arg(keyName).arg(qPrintable(newValue));

					retVal += change + "\n";
				}
				else if (typeId == imtbase::IOperationDescription::OT_CLEAR){
					QString change = imtbase::GetTranslation(
						m_translationManagerCompPtr.GetPtr(),
						QString(QT_TR_NOOP("'%1' was cleared")).toUtf8(),
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
			int operationType,
			const QByteArray& key,
			const QString& keyName,
			const QByteArray& oldValue,
			const QByteArray& newValue) const
{
	istd::TDelPtr<imtbase::COperationDescription> operationDescriptionPtr;
	operationDescriptionPtr.SetPtr(new imtbase::COperationDescription);

	operationDescriptionPtr->SetOperationType(operationType);
	operationDescriptionPtr->SetKey(key);
	operationDescriptionPtr->SetKeyName(keyName);
	operationDescriptionPtr->SetOldValue(oldValue);
	operationDescriptionPtr->SetNewValue(newValue);

	if (operationType == imtbase::IOperationDescription::OT_UNKNOWN){
		if (!oldValue.isEmpty() && !newValue.isEmpty()){
			operationDescriptionPtr->SetOperationType(imtbase::IOperationDescription::OT_CHANGE);
		}
		else if (oldValue.isEmpty() && !newValue.isEmpty()){
			operationDescriptionPtr->SetOperationType(imtbase::IOperationDescription::OT_SET);
		}
		else if (!oldValue.isEmpty() && newValue.isEmpty()){
			operationDescriptionPtr->SetOperationType(imtbase::IOperationDescription::OT_CLEAR);
		}
	}

	return operationDescriptionPtr.PopPtr();
}


} // namespace imtlic



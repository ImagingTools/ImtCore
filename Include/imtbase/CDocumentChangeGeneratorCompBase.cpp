#include <imtbase/CDocumentChangeGeneratorCompBase.h>


// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::IDocumentChangeGenerator)

bool CDocumentChangeGeneratorCompBase::GenerateDocumentChanges(
			const QByteArray& operationTypeId,
			const QByteArray& documentId,
			const istd::IChangeable& document,
			CObjectCollection& documentChangeCollection,
			QString& errorMessage,
			const iprm::IParamsSet* /*paramsPtr*/)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'm_objectCollectionCompPtr' was not set", "CDocumentChangeGeneratorCompBase");
		return false;
	}

	if (operationTypeId == QByteArray("Update")){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(documentId, dataPtr)){
			if (dataPtr.IsValid()){
				istd::TDelPtr<const istd::IChangeable> oldDocumentPtr;
				oldDocumentPtr.SetCastedOrRemove(dataPtr->CloneMe());
				if (!oldDocumentPtr.IsValid()){
					errorMessage = QString("Unable to generate changes for document with ID: '%1'. Error: Document is invalid").arg(qPrintable(documentId));

					return false;
				}

				return CompareDocuments(*oldDocumentPtr.GetPtr(), document, documentChangeCollection, errorMessage);
			}
		}
	}
	else{
		istd::TDelPtr<imtbase::COperationDescription> operationDescriptionPtr = CreateOperationDescription(operationTypeId, "", "", "", "");
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", operationDescriptionPtr.GetPtr());
	}

	return true;
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

				if (typeId == "Create"){
					QString change = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Created the document"), languageId, "imtbase::CDocumentChangeGeneratorCompBase");
					retVal += change + "\n";
				}
				else if (typeId == "Change"){
					QString change = imtbase::GetTranslation(
						m_translationManagerCompPtr.GetPtr(),
						QString(QT_TR_NOOP("'%1' changed from '%2' to '%3'")).toUtf8(),
						languageId,
						"imtbase::CDocumentChangeGeneratorCompBase");

					change = change.arg(keyName).arg(qPrintable(oldValue)).arg(qPrintable(newValue));

					retVal += change + "\n";
				}
				else if (typeId == "Set"){
					QString change = imtbase::GetTranslation(
						m_translationManagerCompPtr.GetPtr(),
						QString(QT_TR_NOOP("'%1' was set to '%2'")).toUtf8(),
						languageId,
						"imtbase::CDocumentChangeGeneratorCompBase");

					change = change.arg(keyName).arg(qPrintable(newValue));

					retVal += change + "\n";
				}
				else if (typeId == "Clear"){
					QString change = imtbase::GetTranslation(
						m_translationManagerCompPtr.GetPtr(),
						QString(QT_TR_NOOP("'%1' was cleared")).toUtf8(),
						languageId,
						"imtbase::CDocumentChangeGeneratorCompBase");

					change = change.arg(keyName);

					retVal += change + "\n";
				}
				else{
					QString change = CreateCustomOperationDescription(*operationDescriptionPtr);
					if (!change.isEmpty()){
						retVal += change + "\n";
					}
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
			operationDescriptionPtr->SetOperationTypeId("Change");
		}
		else if (oldValue.isEmpty() && !newValue.isEmpty()){
			operationDescriptionPtr->SetOperationTypeId("Set");
		}
		else if (!oldValue.isEmpty() && newValue.isEmpty()){
			operationDescriptionPtr->SetOperationTypeId("Clear");
		}
	}

	return operationDescriptionPtr.PopPtr();
}


QByteArray CDocumentChangeGeneratorCompBase::InsertOperationDescription(
			imtbase::CObjectCollection& documentChangeCollection,
			const QByteArray& operationTypeId,
			const QByteArray& key,
			const QString& keyName,
			const QByteArray& oldValue,
			const QByteArray& newValue) const
{
	istd::TDelPtr<imtbase::IOperationDescription> operationDescriptionPtr = CreateOperationDescription(operationTypeId, key, keyName, oldValue, newValue);
	if (operationDescriptionPtr.IsValid()){
		return documentChangeCollection.InsertNewObject(
					"OperationInfo",
					"",
					"",
					operationDescriptionPtr.GetPtr());
	}

	return QByteArray();
}


void CDocumentChangeGeneratorCompBase::GenerateChanges(
			const QByteArrayList& oldValueList,
			const QByteArrayList& newValueList,
			QByteArrayList& addedValueList,
			QByteArrayList& removedValueList) const
{
	for (const QByteArray& valueId : oldValueList){
		if (!newValueList.contains(valueId)){
			removedValueList << valueId;
		}
	}

	for (const QByteArray& valueId : newValueList){
		if (!oldValueList.contains(valueId)){
			addedValueList << valueId;
		}
	}
}


QString CDocumentChangeGeneratorCompBase::CreateCustomOperationDescription(const imtbase::COperationDescription& /*operationDescription*/, const QByteArray& /*languageId*/) const
{
	return QString();
}


} // namespace imtlic



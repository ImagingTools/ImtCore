// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CDocumentChangeGeneratorCompBase.h>


// ACF includes
#include <iqt/iqt.h>


namespace imtbase
{


// protected methods

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


bool CDocumentChangeGeneratorCompBase::IsUuid(const QByteArray& data) const
{
	if (data.size() == 36 || data.size() == 38){
		const QString str = QString::fromLatin1(data);
		QUuid uuid(str);
		return !uuid.isNull();
	}

	return false;
}


// reimplemented (imtbase::IDocumentChangeGenerator)

bool CDocumentChangeGeneratorCompBase::GenerateDocumentChanges(
			const QByteArray& operationTypeId,
			const QByteArray& documentId,
			const istd::IChangeable* documentPtr,
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
				return CompareDocuments(*dataPtr, *documentPtr, documentChangeCollection, errorMessage);
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
				oldValue = GetKeyNameForOperation(key, oldValue).toUtf8();
				newValue = GetKeyNameForOperation(key, newValue).toUtf8();

				QString keyName = operationDescriptionPtr->GetKeyName();
				keyName = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), keyName.toUtf8(), languageId, "Attribute");

				if (typeId == "Create"){
					QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Created the document"), languageId, "imtbase::CDocumentChangeGeneratorCompBase");
					retVal += change + "\n";
				}
				else if (typeId == "Change"){
					QString change = iqt::GetTranslation(
						m_translationManagerCompPtr.GetPtr(),
						QString(QT_TR_NOOP("'%1' changed from '%2' to '%3'")).toUtf8(),
						languageId,
						"imtbase::CDocumentChangeGeneratorCompBase");

					change = change.arg("<b>" + keyName + "</b>").arg(qPrintable(oldValue)).arg(qPrintable(newValue));

					retVal += "<p>" + change + "</p>";
				}
				else if (typeId == "Set"){
					QString change = iqt::GetTranslation(
						m_translationManagerCompPtr.GetPtr(),
						QString(QT_TR_NOOP("'%1' was set to '%2'")).toUtf8(),
						languageId,
						"imtbase::CDocumentChangeGeneratorCompBase");

					change = change.arg("<b>" + keyName + "</b>").arg(qPrintable(newValue));

					retVal += "<p>" + change + "</p>";
				}
				else if (typeId == "Clear"){
					QString change = iqt::GetTranslation(
						m_translationManagerCompPtr.GetPtr(),
						QString(QT_TR_NOOP("'%1' was cleared")).toUtf8(),
						languageId,
						"imtbase::CDocumentChangeGeneratorCompBase");

					change = change.arg("<b>" + keyName + "</b>");

					retVal += "<p>" + change + "</p>";
				}
				else{
					QString change = CreateCustomOperationDescription(*operationDescriptionPtr);
					if (!change.isEmpty()){
						retVal += "<p>" + change + "</p>";
					}
				}
			}
		}
	}

	return retVal;
}


QString CDocumentChangeGeneratorCompBase::GetKeyNameForOperation(const QByteArray& /*key*/, const QByteArray& value) const
{
	return value;
}


QString CDocumentChangeGeneratorCompBase::CreateCustomOperationDescription(const imtbase::COperationDescription& /*operationDescription*/, const QByteArray& /*languageId*/) const
{
	return QString();
}


// private methods

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


} // namespace imtlic



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CDocumentChangeGeneratorCompBase.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>
#include <iqt/iqt.h>


namespace imtbase
{


const QByteArray CDocumentChangeGeneratorCompBase::VALUE_YES = QByteArrayLiteral(QT_TRANSLATE_NOOP("Attribute", "Yes"));
const QByteArray CDocumentChangeGeneratorCompBase::VALUE_NO = QByteArrayLiteral(QT_TRANSLATE_NOOP("Attribute", "No"));


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


QString CDocumentChangeGeneratorCompBase::GetTextParamValue(const iprm::IParamsSet* paramsPtr, const QByteArray& paramId)
{
	iprm::TParamsPtr<iprm::ITextParam> textParamPtr(paramsPtr, paramId, false);
	if (textParamPtr.IsValid()){
		return textParamPtr->GetText();
	}

	return QString();
}


bool CDocumentChangeGeneratorCompBase::InsertChange(
	imtbase::CObjectCollection& documentChangeCollection,
	const QByteArray& key,
	const QString& keyName,
	const QByteArray& oldValue,
	const QByteArray& newValue) const
{
	if (oldValue == newValue){
		return false;
	}

	return !InsertOperationDescription(documentChangeCollection, "", key, keyName, oldValue, newValue).isEmpty();
}


bool CDocumentChangeGeneratorCompBase::InsertTextChange(
	imtbase::CObjectCollection& documentChangeCollection,
	const QByteArray& key,
	const QString& keyName,
	const QString& oldValue,
	const QString& newValue) const
{
	if (oldValue == newValue){
		return false;
	}

	return InsertChange(documentChangeCollection, key, keyName, oldValue.toUtf8(), newValue.toUtf8());
}


bool CDocumentChangeGeneratorCompBase::InsertFlagChange(
	imtbase::CObjectCollection& documentChangeCollection,
	const QByteArray& key,
	const QString& keyName,
	bool oldValue,
	bool newValue) const
{
	if (oldValue == newValue){
		return false;
	}

	return InsertChange(documentChangeCollection, key, keyName, oldValue ? VALUE_YES : VALUE_NO, newValue ? VALUE_YES : VALUE_NO);
}


bool CDocumentChangeGeneratorCompBase::InsertNumberChange(
	imtbase::CObjectCollection& documentChangeCollection,
	const QByteArray& key,
	const QString& keyName,
	qint64 oldValue,
	qint64 newValue) const
{
	if (oldValue == newValue){
		return false;
	}

	return InsertChange(documentChangeCollection, key, keyName, QByteArray::number(oldValue), QByteArray::number(newValue));
}


bool CDocumentChangeGeneratorCompBase::InsertDateTimeChange(
	imtbase::CObjectCollection& documentChangeCollection,
	const QByteArray& key,
	const QString& keyName,
	const QDateTime& oldValue,
	const QDateTime& newValue) const
{
	if (oldValue == newValue){
		return false;
	}

	static const QString dateTimeFormat = QStringLiteral("yyyy-MM-dd");

	return InsertChange(
				documentChangeCollection,
				key,
				keyName,
				oldValue.isValid() ? oldValue.toString(dateTimeFormat).toUtf8() : QByteArray(),
				newValue.isValid() ? newValue.toString(dateTimeFormat).toUtf8() : QByteArray());
}


bool CDocumentChangeGeneratorCompBase::InsertEnumChange(
	imtbase::CObjectCollection& documentChangeCollection,
	const QByteArray& key,
	const QString& keyName,
	int oldValue,
	int newValue,
	const QStringList& enumNames) const
{
	if (oldValue == newValue){
		return false;
	}

	// Out-of-range enumeration values must not abort the whole comparison, the raw value is still informative.
	const QByteArray oldName = (oldValue >= 0 && oldValue < enumNames.size()) ? enumNames[oldValue].toUtf8() : QByteArray::number(oldValue);
	const QByteArray newName = (newValue >= 0 && newValue < enumNames.size()) ? enumNames[newValue].toUtf8() : QByteArray::number(newValue);

	return InsertChange(documentChangeCollection, key, keyName, oldName, newName);
}


void CDocumentChangeGeneratorCompBase::InsertListChanges(
	imtbase::CObjectCollection& documentChangeCollection,
	const QByteArray& addOperationTypeId,
	const QByteArray& removeOperationTypeId,
	const QByteArray& key,
	const QString& keyName,
	const QByteArrayList& oldValueList,
	const QByteArrayList& newValueList) const
{
	QByteArrayList addedValueList;
	QByteArrayList removedValueList;
	GenerateChanges(oldValueList, newValueList, addedValueList, removedValueList);

	for (const QByteArray& value : std::as_const(addedValueList)){
		InsertOperationDescription(documentChangeCollection, addOperationTypeId, key, keyName, QByteArray(), value);
	}

	for (const QByteArray& value : std::as_const(removedValueList)){
		InsertOperationDescription(documentChangeCollection, removeOperationTypeId, key, keyName, value, QByteArray());
	}
}


// reimplemented (imtbase::IDocumentChangeGenerator)

bool CDocumentChangeGeneratorCompBase::GenerateDocumentChanges(
			const QByteArray& operationTypeId,
			const QByteArray& documentId,
			const istd::IChangeable* documentPtr,
			CObjectCollection& documentChangeCollection,
			QString& errorMessage,
			const iprm::IParamsSet* paramsPtr)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'm_objectCollectionCompPtr' was not set", "CDocumentChangeGeneratorCompBase");
		return false;
	}

	if (operationTypeId == QByteArrayLiteral("Update")){
		return AppendDocumentComparison(documentId, documentPtr, documentChangeCollection, errorMessage);
	}

	// Attributes stored outside of the document body describe themselves through the optional
	// 'Key', 'KeyName', 'OldValue' and 'NewValue' parameters. Without them a bare marker entry is written.
	const QByteArray key = GetTextParamValue(paramsPtr, "Key").toUtf8();
	if (key.isEmpty()){
		InsertOperationDescription(documentChangeCollection, operationTypeId, "", "", "", "");
	}
	else{
		InsertOperationDescription(
					documentChangeCollection,
					operationTypeId,
					key,
					GetTextParamValue(paramsPtr, "KeyName"),
					GetTextParamValue(paramsPtr, "OldValue").toUtf8(),
					GetTextParamValue(paramsPtr, "NewValue").toUtf8());
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
			if (operationDescriptionPtr == nullptr){
				continue;
			}

			// Derived generators may render any operation type, the standard rendering is the fallback.
			QString change = CreateCustomOperationDescription(*operationDescriptionPtr, languageId);
			if (change.isEmpty()){
				change = CreateStandardOperationDescription(*operationDescriptionPtr, languageId);
			}

			if (!change.isEmpty()){
				retVal += "<p>" + change + "</p>";
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


QString CDocumentChangeGeneratorCompBase::Translate(const QByteArray& phrase, const QByteArray& languageId, const QByteArray& context) const
{
	return QString::fromUtf8(iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), phrase, languageId, context));
}


bool CDocumentChangeGeneratorCompBase::AppendDocumentComparison(
	const QByteArray& documentId,
	const istd::IChangeable* documentPtr,
	CObjectCollection& documentChangeCollection,
	QString& errorMessage)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'm_objectCollectionCompPtr' was not set", "CDocumentChangeGeneratorCompBase");

		return false;
	}

	if (documentPtr == nullptr){
		errorMessage = QStringLiteral("Unable to generate changes for document '%1'. Error: New document is not available").arg(documentId);

		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(documentId, dataPtr) || !dataPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to generate changes for document '%1'. Error: Stored document is not available").arg(documentId);

		return false;
	}

	// The stored instance must not alias the new one, otherwise the comparison would always be empty.
	if (dataPtr.GetPtr() == documentPtr){
		errorMessage = QStringLiteral("Unable to generate changes for document '%1'. Error: Modified document aliases the stored one").arg(documentId);

		return false;
	}

	return CompareDocuments(*dataPtr, *documentPtr, documentChangeCollection, errorMessage);
}


// private methods

QString CDocumentChangeGeneratorCompBase::CreateStandardOperationDescription(
	const imtbase::COperationDescription& operationDescription,
	const QByteArray& languageId) const
{
	static const QByteArray translationContext = QByteArrayLiteral("imtbase::CDocumentChangeGeneratorCompBase");

	const QByteArray typeId = operationDescription.GetOperationTypeId();
	const QByteArray key = operationDescription.GetKey();

	// The raw values are resolved to display names first and then escaped, because they end up in HTML markup.
	const QString oldValue = Translate(GetKeyNameForOperation(key, operationDescription.GetOldValue()).toUtf8(), languageId, "Attribute").toHtmlEscaped();
	const QString newValue = Translate(GetKeyNameForOperation(key, operationDescription.GetNewValue()).toUtf8(), languageId, "Attribute").toHtmlEscaped();
	const QString keyName = "<b>" + Translate(operationDescription.GetKeyName().toUtf8(), languageId, "Attribute").toHtmlEscaped() + "</b>";

	if (typeId == "Create"){
		return Translate(QT_TR_NOOP("Created the document"), languageId, translationContext);
	}

	if (typeId == "Remove"){
		return Translate(QT_TR_NOOP("Deleted the document"), languageId, translationContext);
	}

	if (typeId == "Restore"){
		return Translate(QT_TR_NOOP("Restored the document"), languageId, translationContext);
	}

	if (typeId == "Import"){
		return Translate(QT_TR_NOOP("Imported the document"), languageId, translationContext);
	}

	if (typeId == "Rename"){
		return Translate(QT_TR_NOOP("Renamed from '%1' to '%2'"), languageId, translationContext).arg(oldValue, newValue);
	}

	if (typeId == "Change"){
		return Translate(QT_TR_NOOP("'%1' changed from '%2' to '%3'"), languageId, translationContext).arg(keyName, oldValue, newValue);
	}

	if (typeId == "Set"){
		return Translate(QT_TR_NOOP("'%1' was set to '%2'"), languageId, translationContext).arg(keyName, newValue);
	}

	if (typeId == "Clear"){
		return Translate(QT_TR_NOOP("'%1' was cleared"), languageId, translationContext).arg(keyName);
	}

	return QString();
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


} // namespace imtbase



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CDocumentManagerCompBase.h>


// Qt includes
#include <QtCore/QString>


namespace imtdoc
{


// protected methods

// reimplemented (imtdoc::CDocumentManagerBase)

istd::IChangeableSharedPtr CDocumentManagerCompBase::CreateObject(const QByteArray& typeId) const
{
	int index = GetObjectFactoryIndex(typeId);
	if (index >= 0){
		return m_objectFactListCompPtr.CreateInstance(index);
	}

	Q_ASSERT_X(false, "CDocumentManagerCompBase::CreateObject", qPrintable(QString("Factory not found for the type: '%1'").arg(qPrintable(typeId))));

	return nullptr;
}


idoc::IUndoManagerSharedPtr CDocumentManagerCompBase::CreateUndoManager() const
{
	return m_undoManagerFactPtr.CreateInstance();
}


QString CDocumentManagerCompBase::GetDefaultDocumentName(const WorkingDocument& document) const
{
	const imtdoc::IDocumentNameProvider* nameProviderPtr = GetDocumentNameProvider(document.typeId);
	if (nameProviderPtr == nullptr){
		return QString();
	}

	return nameProviderPtr->GetDefaultDocumentName(document.objectId, *document.objectPtr);
}


bool CDocumentManagerCompBase::HasDocumentNameProvider(const QByteArray& typeId) const
{
	return GetDocumentNameProvider(typeId) != nullptr;
}


bool CDocumentManagerCompBase::ValidateDocumentData(
	const WorkingDocument& document,
	OperationStatus& status,
	QString* errorMessage) const
{
	status = OS_OK;
	if (errorMessage != nullptr) {
		errorMessage->clear();
	}

	if (!document.objectPtr.IsValid()){
		status = OS_INVALID_DOCUMENT_DATA;
		if (errorMessage != nullptr) {
			*errorMessage = GetInvalidDocumentMessage();
		}
		return false;
	}

	const imtdoc::IDocumentValidator* documentValidator = GetDocumentValidator(document.typeId);
	if (documentValidator == nullptr){
		return true;
	}

	QString validationMessage;
	if (!documentValidator->ValidateDocumentData(document.objectId, *document.objectPtr, validationMessage)){
		status = OS_INVALID_DOCUMENT_DATA;
		if (errorMessage != nullptr) {
			*errorMessage = validationMessage.isEmpty() ? GetInvalidDocumentMessage() : validationMessage;
		}
		QString warningMessage = QString("Document validation failed for type '%1'")
			.arg(QString::fromUtf8(document.typeId));
		if (!validationMessage.isEmpty()){
			warningMessage = QString("%1: %2").arg(warningMessage, validationMessage);
			SendWarningMessage(kValidationFailureWarningId, warningMessage);
		}
		else{
			SendWarningMessage(kValidationFailureWithoutMessageWarningId, warningMessage);
		}
		return false;
	}

	return true;
}


QList<imtdoc::IDocumentManagerEventHandler*> CDocumentManagerCompBase::GetDocumentManagerEventHandlers() const
{
	QList<imtdoc::IDocumentManagerEventHandler*> retVal;

	if (m_handlerCompPtr.IsValid()){
		for (int i = 0; i < m_handlerCompPtr.GetCount(); i++){
			if (m_handlerCompPtr[i] != nullptr){
				retVal.append(m_handlerCompPtr[i]);
			}
		}
	}

	return retVal;
}


// private methods

int CDocumentManagerCompBase::GetObjectFactoryIndex(const QByteArray& typeId) const
{
	int count = qMin(m_objectTypeIdsAttrPtr.GetCount(), m_objectFactListCompPtr.GetCount());
	for (int i = 0; i < count; i++){
		if (m_objectTypeIdsAttrPtr[i] == typeId){
			return i;
		}
	}

	return -1;
}


const imtdoc::IDocumentNameProvider* CDocumentManagerCompBase::GetDocumentNameProvider(const QByteArray& typeId) const
{
	int index = GetObjectFactoryIndex(typeId);
	if ((index >= 0) && (index < m_documentNameProviderCompPtr.GetCount())){
		return m_documentNameProviderCompPtr[index];
	}

	return nullptr;
}


const imtdoc::IDocumentValidator* CDocumentManagerCompBase::GetDocumentValidator(const QByteArray& typeId) const
{
	int index = GetObjectFactoryIndex(typeId);
	if ((index >= 0) && (index < m_documentValidatorCompPtr.GetCount())){
		return m_documentValidatorCompPtr[index];
	}

	return nullptr;
}


} // namespace imtdoc

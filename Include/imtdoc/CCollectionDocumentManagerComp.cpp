// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CCollectionDocumentManagerComp.h>

// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtbase/IValidationContext.h>


namespace
{


class CValidationMessageCollector: public ilog::IMessageConsumer
{
public:
	explicit CValidationMessageCollector(ilog::IMessageConsumer* targetPtr)
		:m_targetPtr(targetPtr)
	{
	}

	virtual bool IsMessageSupported(
				int /*messageCategory*/,
				int /*messageId*/,
				const istd::IInformationProvider* /*messagePtr*/) const override
	{
		return true;
	}

	virtual void AddMessage(const MessagePtr& messagePtr) override
	{
		if (messagePtr != nullptr){
			istd::IInformationProvider::InformationCategory category = messagePtr->GetInformationCategory();
			if ((category == istd::IInformationProvider::IC_ERROR
					|| category == istd::IInformationProvider::IC_CRITICAL)
					&& m_errorMessage.isEmpty()){
				m_errorMessage = messagePtr->GetInformationDescription();
			}
		}

		if (m_targetPtr != nullptr){
			m_targetPtr->AddMessage(messagePtr);
		}
	}

	QString GetErrorMessage() const
	{
		return m_errorMessage;
	}

private:
	ilog::IMessageConsumer* m_targetPtr;
	QString m_errorMessage;
};


} // namespace


namespace imtdoc
{


// protected methods

// reimplemented (imtdoc::CCollectionDocumentManager)

imtbase::IObjectCollection* CCollectionDocumentManagerComp::GetCollection() const
{
	if (m_collectionCompPtr.IsValid()){
		return m_collectionCompPtr.GetPtr();
	}

	return nullptr;
}


istd::IChangeableSharedPtr CCollectionDocumentManagerComp::CreateObject(const QByteArray& typeId) const
{
	int index = GetObjectFactoryIndex(typeId);
	if (index >= 0){
		return m_objectFactListCompPtr.CreateInstance(index);
	}

	Q_ASSERT_X(false, "CCollectionDocumentManagerComp::CreateObject", qPrintable(QString("Factory not found for the type: '%1'").arg(qPrintable(typeId))));

	return nullptr;
}


idoc::IUndoManagerSharedPtr CCollectionDocumentManagerComp::CreateUndoManager() const
{
	return m_undoManagerFactPtr.CreateInstance();
}


bool CCollectionDocumentManagerComp::ValidateDocumentData(
	const WorkingDocument& document,
	OperationStatus& status,
	QString* errorMessagePtr) const
{
	status = OS_OK;
	if (errorMessagePtr != nullptr){
		errorMessagePtr->clear();
	}

	if (!document.objectPtr.IsValid()){
		status = OS_FAILED;
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = QStringLiteral("Document data is invalid");
		}
		return false;
	}

	const imtbase::IDataValidator* validator = GetDocumentValidator(document.typeId);
	if (validator == nullptr){
		return true;
	}

	ilog::IMessageConsumer* logPtr = GetLogPtr();
	CValidationMessageCollector messageCollector(logPtr);
	// Validation context is optional; no dedicated context is available for collection documents here.
	const imtbase::IValidationContext* validationContextPtr = nullptr;
	istd::IInformationProvider::InformationCategory category =
		validator->Validate(validationContextPtr, *document.objectPtr, &messageCollector);
	if (category == istd::IInformationProvider::IC_ERROR
			|| category == istd::IInformationProvider::IC_CRITICAL){
		status = OS_FAILED;
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = messageCollector.GetErrorMessage();
		}
		return false;
	}

	return true;
}


QList<imtdoc::IDocumentManagerEventHandler*> CCollectionDocumentManagerComp::GetDocumentManagerEventHandlers() const
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

int CCollectionDocumentManagerComp::GetObjectFactoryIndex(const QByteArray& typeId) const
{
	int count = qMin(m_objectTypeIdsAttrPtr.GetCount(), m_objectFactListCompPtr.GetCount());
	for (int i = 0; i < count; i++){
		if (m_objectTypeIdsAttrPtr[i] == typeId){
			return i;
		}
	}

	return -1;
}


const imtbase::IDataValidator* CCollectionDocumentManagerComp::GetDocumentValidator(const QByteArray& typeId) const
{
	int index = GetObjectFactoryIndex(typeId);
	if ((index >= 0) && (index < m_documentValidatorCompPtr.GetCount())){
		return m_documentValidatorCompPtr[index];
	}

	return nullptr;
}


} // namespace imtdoc

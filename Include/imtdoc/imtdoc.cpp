// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/imtdoc.h>


// Qt includes
#include <QtCore/QList>

// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>
#include <imeas/CSimpleNumericValue.h>

// ImtCore includes
#include <imtdoc/IDocumentService.h>


namespace imtdoc
{


// private methods

static bool ParseSourceRef(const QByteArray& sourceRef, QByteArray& objectId, int& revision)
{
	objectId.clear();
	revision = -1;

	const QList<QByteArray> parts = sourceRef.split('/');
	if (parts.count() > 2 || parts[0].isEmpty()){
		return false;
	}

	if (parts.count() == 2){
		bool isOk = false;
		const int parsedRevision = parts[1].toInt(&isOk);
		if (!isOk || parsedRevision < 0){
			return false;
		}

		revision = parsedRevision;
	}

	objectId = parts[0];

	return true;
}


// public methods

QByteArray ObjectIdToSourceRef(const QByteArray& objectId, int revision)
{
	if (objectId.isEmpty()){
		return QByteArray();
	}

	if (revision < 0){
		return objectId;
	}

	return objectId + '/' + QByteArray::number(revision);
}


QByteArray SourceRefToObjectId(const QByteArray& sourceRef)
{
	QByteArray objectId;
	int revision = -1;
	if (!ParseSourceRef(sourceRef, objectId, revision)){
		return QByteArray();
	}

	return objectId;
}


int SourceRefToRevision(const QByteArray& sourceRef)
{
	QByteArray objectId;
	int revision = -1;
	if (!ParseSourceRef(sourceRef, objectId, revision)){
		return -1;
	}

	return revision;
}


iprm::IParamsSetUniquePtr CreateObjectSourceParams(const QByteArray& objectId, int revision)
{
	if (objectId.isEmpty()){
		return iprm::IParamsSetUniquePtr();
	}

	iprm::CParamsSet* paramsSetPtr = new iprm::CParamsSet;
	iprm::IParamsSetUniquePtr retVal(paramsSetPtr);

	iprm::CIdParam* objectIdParamPtr = new iprm::CIdParam;
	objectIdParamPtr->SetId(objectId);
	paramsSetPtr->SetEditableParameter(s_objectIdParamId, objectIdParamPtr, true);

	if (revision >= 0){
		imeas::CSimpleNumericValue* revisionParamPtr = new imeas::CSimpleNumericValue;
		revisionParamPtr->SetValues(imath::CVarVector(1, revision));
		paramsSetPtr->SetEditableParameter(s_revisionParamId, revisionParamPtr, true);
	}

	return retVal;
}


static QByteArray GetIdFromParams(const iprm::IParamsSet* paramsPtr, const QByteArray& paramId)
{
	if (paramsPtr == nullptr){
		return QByteArray();
	}

	const iprm::IIdParam* idParamPtr = dynamic_cast<const iprm::IIdParam*>(paramsPtr->GetParameter(paramId));
	if (idParamPtr == nullptr){
		return QByteArray();
	}

	return idParamPtr->GetId();
}


QByteArray GetObjectIdFromParams(const iprm::IParamsSet* paramsPtr)
{
	return GetIdFromParams(paramsPtr, s_objectIdParamId);
}


int GetRevisionFromParams(const iprm::IParamsSet* paramsPtr)
{
	if (paramsPtr == nullptr){
		return -1;
	}

	const imeas::INumericValue* revisionParamPtr = dynamic_cast<const imeas::INumericValue*>(paramsPtr->GetParameter(s_revisionParamId));
	if (revisionParamPtr == nullptr){
		return -1;
	}

	const imath::CVarVector values = revisionParamPtr->GetValues();
	if (values.GetElementsCount() != 1 || values[0] < 0){
		return -1;
	}

	return qRound(values[0]);
}


QByteArray GetLanguageIdFromParams(const iprm::IParamsSet* paramsPtr)
{
	return GetIdFromParams(paramsPtr, s_languageIdParamId);
}


QByteArray GetDocumentObjectId(const IDocumentService& documentService, const QByteArray& userId, const QByteArray& documentId)
{
	const IDocumentService::DocumentList documentList = documentService.GetOpenedDocumentList(userId);
	for (const IDocumentService::DocumentListItem& documentItem : documentList){
		if (documentItem.documentId == documentId){
			return SourceRefToObjectId(documentItem.documentSourceRef);
		}
	}

	return QByteArray();
}


bool IsDocumentStored(const IDocumentService& documentService, const QByteArray& userId, const QByteArray& documentId)
{
	const IDocumentService::DocumentList documents = documentService.GetOpenedDocumentList(userId);
	for (const IDocumentService::DocumentListItem& document : documents){
		if (document.documentId == documentId){
			return !document.documentSourceRef.isEmpty();
		}
	}

	return true;
}


} // namespace imtdoc



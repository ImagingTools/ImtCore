// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/imtdoc.h>


// Qt includes
#include <QtCore/QList>

// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>
#include <imeas/CSimpleNumericValue.h>


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


static bool ParseUrl(const QUrl& url, QByteArray& objectId, int& revision)
{
	objectId.clear();
	revision = -1;

	if (url.scheme() != QStringLiteral("collection")){
		return false;
	}

	QByteArray path = url.path().toUtf8();
	if (!path.startsWith('/')){
		return false;
	}

	path.remove(0, 1);
	if (path.endsWith('/')){
		path.chop(1);
	}

	return ParseSourceRef(path, objectId, revision);
}


// public methods

QUrl ObjectIdToUrl(const QByteArray& objectId, int revision)
{
	if (revision < 0){
		return QUrl(QStringLiteral("collection:///") + objectId);
	}

	return QUrl(QStringLiteral("collection:///") + objectId + '/' + QString::number(revision));
}


QByteArray UrlToObjectId(const QUrl& url)
{
	QByteArray objectId;
	int revision = -1;
	if (!ParseUrl(url, objectId, revision)){
		return QByteArray();
	}

	return objectId;
}


int UrlToRevision(const QUrl& url)
{
	QByteArray objectId;
	int revision = -1;
	if (!ParseUrl(url, objectId, revision)){
		return -1;
	}

	return revision;
}


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


QUrl SourceRefToUrl(const QByteArray& sourceRef)
{
	QByteArray objectId;
	int revision = -1;
	if (!ParseSourceRef(sourceRef, objectId, revision)){
		return QUrl();
	}

	return ObjectIdToUrl(objectId, revision);
}


QByteArray UrlToSourceRef(const QUrl& url)
{
	QByteArray objectId;
	int revision = -1;
	if (!ParseUrl(url, objectId, revision)){
		return QByteArray();
	}

	return ObjectIdToSourceRef(objectId, revision);
}


iprm::IParamsSetUniquePtr CreateObjectSourceParams(const QByteArray& objectId, int revision)
{
	iprm::CParamsSet* paramsSetPtr = new iprm::CParamsSet;
	iprm::IParamsSetUniquePtr retVal(paramsSetPtr);

	if (objectId.isEmpty()){
		return retVal;
	}

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


QByteArray GetObjectIdFromParams(const iprm::IParamsSet& params)
{
	const iprm::IIdParam* objectIdParamPtr = dynamic_cast<const iprm::IIdParam*>(params.GetParameter(s_objectIdParamId));
	if (objectIdParamPtr == nullptr){
		return QByteArray();
	}

	return objectIdParamPtr->GetId();
}


int GetRevisionFromParams(const iprm::IParamsSet& params)
{
	const imeas::INumericValue* revisionParamPtr = dynamic_cast<const imeas::INumericValue*>(params.GetParameter(s_revisionParamId));
	if (revisionParamPtr == nullptr){
		return -1;
	}

	const imath::CVarVector values = revisionParamPtr->GetValues();
	if (values.GetElementsCount() != 1 || values[0] < 0){
		return -1;
	}

	return qRound(values[0]);
}


} // namespace imtdoc



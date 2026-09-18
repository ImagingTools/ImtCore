// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdbgql/CStructureControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CIdParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/IHierarchicalStructureIterator.h>
#include <imtdb/imtdb.h>
#include <imtgql/imtgql.h>

// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>


namespace imtdbgql
{


// protected methods

QByteArray CStructureControllerComp::GetElementsQuery(iprm::IParamsSet* filterParams) const
{
	QByteArray elementsQuery;
	iprm::TParamsPtr<iprm::ITextParam> parentNodeIdParamPtr(filterParams, "ParentNodeId");
	if (parentNodeIdParamPtr.IsValid()){
		QString parentNodeId = parentNodeIdParamPtr->GetText();

		elementsQuery = QStringLiteral(R"(
SELECT "DocumentId" AS "Id", "TypeId", "Name", "Description", "Size", "Permissions", false AS "IsNode",
	false AS "HasChildren" FROM public."NodeDocuments" WHERE "NodeId" = '%1'
UNION SELECT "NodeId", 'Node' AS "TypeId", "Name", "Description", 0 AS "Size", "Permissions", true AS "IsNode",
(SELECT count(*) FROM "NodeDocuments" as nd WHERE nodes."NodeId" = nd."NodeId") > 0 OR
(SELECT count(*) FROM "Nodes" as nodes2 WHERE nodes."NodeId" = nodes2."ParentId") > 0
	FROM public."Nodes" as nodes WHERE "ParentId" = '%1'
	ORDER BY "IsNode" DESC, "Name";)"
		).arg(imtdb::SqlEncode(parentNodeId)).toUtf8();

	}

	return elementsQuery;
}


QJsonObject CStructureControllerComp::GetElements(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_collectionStructureCompPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to list nodes. Component reference 'CollectionStructure' was not set");

		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();
	QJsonObject rootObj;

	if (!errorMessage.isEmpty()){
		QJsonArray errorsArray;
		QJsonObject errorObj;
		errorObj.insert(QStringLiteral("message"), errorMessage);
		errorsArray.append(errorObj);
		rootObj.insert(QStringLiteral("errors"), errorsArray);
	}
	else{
		QJsonObject dataObj;
		QJsonArray itemsArray;
		QJsonObject notificationObj;

		const imtgql::CGqlParamObject* inputObject = inputParams.GetParamArgumentObjectPtr("input");
		iprm::CParamsSet filterParams;

		QByteArray parentNodeId = inputParams.GetParamArgumentValue("ParentNodeId").toByteArray();
		QByteArray selectIndex;
		iprm::CTextParam parentNodeIdParams;
		parentNodeIdParams.SetText(parentNodeId);
		filterParams.SetEditableParameter("ParentNodeId", &parentNodeIdParams);

		const imtgql::CGqlParamObject* additionObject = inputParams.GetParamArgumentObjectPtr("addition");
		if (inputObject != nullptr){
			additionObject = inputObject->GetParamArgumentObjectPtr("addition");
		}

		if (additionObject != nullptr){
			parentNodeId = additionObject->GetParamArgumentValue("nodeId").toByteArray();
			selectIndex = additionObject->GetParamArgumentValue("selectIndex").toByteArray();
		}

		QByteArray getElementsQuery = GetElementsQuery(&filterParams);
		QSqlQuery sqlQuery;
		if (!getElementsQuery.isEmpty()){
			QSqlError sqlError;
			sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(getElementsQuery, &sqlError);

			if (sqlError.type() != QSqlError::NoError){
				SendErrorMessage(0, sqlError.text(), "Database collection");
			}
		}
		else{
			SendErrorMessage(0, "Database query could not be created", "Database collection");
		}

		notificationObj.insert(QStringLiteral("PagesCount"), 0);
		notificationObj.insert(QStringLiteral("TotalCount"), -1);

		while (sqlQuery.next()){
			QJsonObject itemObj;
			QString id = sqlQuery.value("Id").toString();
			QString name = sqlQuery.value("Name").toString();
			QString description = sqlQuery.value("Name").toString();
			QString typeId = sqlQuery.value("TypeId").toString();
			bool hasChildren = sqlQuery.value("HasChildren").toBool();
			int size = sqlQuery.value("Size").toInt();
			itemObj.insert(QStringLiteral("Id"), id);
			itemObj.insert(QStringLiteral("Name"), name);
			itemObj.insert(QStringLiteral("Description"), description);
			itemObj.insert(QStringLiteral("TypeId"), typeId);
			itemObj.insert(QStringLiteral("Size"), size);
			itemObj.insert(QStringLiteral("HasChildren"), hasChildren);
			itemsArray.append(itemObj);
		}

		dataObj.insert(QStringLiteral("items"), itemsArray);
		dataObj.insert(QStringLiteral("notification"), notificationObj);
		dataObj.insert(QStringLiteral("selectIndex"), QString::fromUtf8(selectIndex.constData()));
		rootObj.insert(QStringLiteral("data"), dataObj);
	}

	return rootObj;
}


} // namespace imtdbgql

// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdbgql/CStructureControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CIdParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/IHierarchicalStructureIterator.h>
#include <imtgql/imtgql.h>


namespace imtdbgql
{


// protected methods

QByteArray CStructureControllerComp::GetElementsQuery(iprm::IParamsSet* filterParams) const
{
	QByteArray elementsQuery;
	iprm::TParamsPtr<iprm::ITextParam> parentNodeIdParamPtr(filterParams, "ParentNodeId");
	if (parentNodeIdParamPtr.IsValid()){
		QString parentNodeId = parentNodeIdParamPtr->GetText();

		elementsQuery = QString(R"(
SELECT "DocumentId" AS "Id", "TypeId", "Name", "Description", "Size", "Permissions", false AS "IsNode",
	false AS "HasChildren" FROM public."NodeDocuments" WHERE "NodeId" = '%1'
UNION SELECT "NodeId", 'Node' AS "TypeId", "Name", "Description", 0 AS "Size", "Permissions", true AS "IsNode",
(SELECT count(*) FROM "NodeDocuments" as nd WHERE nodes."NodeId" = nd."NodeId") > 0 OR
(SELECT count(*) FROM "Nodes" as nodes2 WHERE nodes."NodeId" = nodes2."ParentId") > 0
	FROM public."Nodes" as nodes WHERE "ParentId" = '%1'
	ORDER BY "IsNode" DESC, "Name";)"
		).arg(parentNodeId).toUtf8();

	}

	return elementsQuery;
}


imtbase::CTreeItemModel* CStructureControllerComp::GetElements(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_collectionStructureCompPtr.IsValid()){
		errorMessage = QString("Unable to list nodes. Component reference 'CollectionStructure' was not set");

		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();

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

		notificationModel->SetData("PagesCount", 0);
		notificationModel->SetData("TotalCount", -1);

		while (sqlQuery.next()){
			int itemIndex = itemsModel->InsertNewItem();
			QString id = sqlQuery.value("Id").toString();
			QString name = sqlQuery.value("Name").toString();
			QString description = sqlQuery.value("Name").toString();
			QString typeId = sqlQuery.value("TypeId").toString();
			bool hasChildren = sqlQuery.value("HasChildren").toBool();
			int size = sqlQuery.value("Size").toInt();
			itemsModel->SetData("Id", id, itemIndex);
			itemsModel->SetData("Name", name, itemIndex);
			itemsModel->SetData("Description", description, itemIndex);
			itemsModel->SetData("TypeId", typeId, itemIndex);
			itemsModel->SetData("Size", size, itemIndex);
			itemsModel->SetData("HasChildren", hasChildren, itemIndex);
		}

		itemsModel->SetIsArray(true);

		dataModel->SetExternTreeModel("items", itemsModel);
		dataModel->SetExternTreeModel("notification", notificationModel);
		dataModel->SetData("selectIndex", selectIndex);
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


} // namespace imtdbgql



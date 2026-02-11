// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerCompWrap.h>
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/IGqlObjectCollectionDelegate.h>
#include <imtserverapp/CParamSetRepresentationController.h>
#include <imtserverapp/CComplexCollectionFilterRepresentationController.h>
#include <imtserverapp/CDocumentFilterRepresentationController.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtclientgql
{


class CGqlObjectCollectionDelegateComp:
			public ilog::CLoggerComponentBase,
			virtual public IGqlObjectCollectionDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlObjectCollectionDelegateComp);
		I_REGISTER_INTERFACE(IGqlObjectCollectionDelegate);
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection-ID", false, "");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", false, "VersionInfo");
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", false, "ObjectCollection");
	I_END_COMPONENT;

	virtual imtgql::IGqlRequest* CreateGetObjectTypeIdRequest(const QByteArray& objectId) const override;
	virtual imtgql::IGqlRequest* CreateGetObjectInfoRequest(const QByteArray& objectId) const override;
	virtual imtgql::IGqlRequest* CreateGetObjectMetaInfoRequest(const QByteArray& objectId) const override;
	virtual imtgql::IGqlRequest* CreateGetObjectDataMetaInfoRequest(const QByteArray& objectId) const override;
	virtual imtgql::IGqlRequest* CreateInsertObjectRequest(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* objectPtr,
				const QString& uploadUrl = QString(),
				const QByteArray& proposedObjectId = QByteArray(),
				const QByteArray& nodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateSetObjectNameRequest(
				const QByteArray& objectId,
				const QString& name,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateSetObjectDescriptionRequest(
				const QByteArray& objectId,
				const QString& description,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetObjectRequest(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateSetObjectRequest(
				const QByteArray& objectId,
				const QByteArray& typeId,
				const istd::IChangeable* objectPtr,
				const QString& uploadUrl = QString(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateRemoveObjectsRequest(
				const imtbase::ICollectionInfo::Ids& objectIds,
				int clientElementVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateRemoveObjectSetRequest(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				int clientElementVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetElementCountRequest(const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetElementListRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetSubCollectionRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	virtual bool IsValid(const imtgql::IGqlResponse& reqponse) const override;
	virtual bool GetOperationResult(const imtgql::IGqlResponse& response, bool& out) const override;
	virtual bool GetObjectId(const imtgql::IGqlResponse& response, Id& out) const override;
	virtual bool GetObjectTypeId(const imtgql::IGqlResponse& response, Id& out) const override;
	virtual bool GetObjectInfo(const imtgql::IGqlResponse& response, ObjectInfo& out) const override;
	virtual bool GetObjectData(const imtgql::IGqlResponse& response, istd::IChangeable& out) const override;
	virtual bool GetMetaInfo(const imtgql::IGqlResponse& response, idoc::IDocumentMetaInfo& out) const override;
	virtual bool GetItemCount(const imtgql::IGqlResponse& response, int& out) const override;
	virtual bool GetItemIds(const imtgql::IGqlResponse& response, Ids& out) const override;
	virtual imtbase::IObjectCollection* GetSubCollection(
				imtbase::IObjectCollection& collection,
				const imtgql::IGqlResponse& response,
				QList<imtbase::IMetaInfoCreator*> metaInfoCreatorList) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	struct ResponseData
	{
		QJsonObject data;
		QJsonObject errors;
		QByteArray commandId;
	};

	ResponseData GetResponseData(const imtgql::IGqlResponse& response) const;
	bool SerializeObject(const istd::IPolymorphic* object, QByteArray& objectData) const;
	bool DeSerializeObject(istd::IPolymorphic* object, const QByteArray& objectData) const;
	bool GetParamsSetRepresentation(const iprm::IParamsSet& paramsSet, sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& representation) const;

	template<class Arguments,class SdlRequest>
	istd::TUniqueInterfacePtr<imtgql::IGqlRequest> CreateGqlRequest(Arguments arguments) const;

private:
	imtserverapp::CParamSetRepresentationController m_paramSetRepresentationController;
	imtserverapp::CDocumentFilterRepresentationController m_documentFilterRepresentationController;
	imtserverapp::CComplexCollectionFilterRepresentationController m_complexCollectionFilterRepresentationController;

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace imtclientgql



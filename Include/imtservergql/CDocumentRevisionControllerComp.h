#pragma once


// ImtCore includes
#include <imtbase/ISearchController.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IDocumentChangeGenerator.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentRevision.h>


namespace imtservergql
{


class CDocumentRevisionControllerComp: public sdl::imtbase::DocumentRevision::V1_0::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtbase::DocumentRevision::V1_0::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDocumentRevisionControllerComp);
		I_ASSIGN_MULTI_0(m_collectionIdsAttrPtr, "CollectionIds", "Collection ID-s", false);
		I_ASSIGN_MULTI_0(m_objectCollectionsCompPtr, "ObjectCollections", "Object collections", false);
		I_ASSIGN_MULTI_0(m_documentChangeGeneratorsCompPtr, "DocumentChangeGenerators", "Document change generators", false);
	I_END_COMPONENT;

protected:
	virtual imtbase::IObjectCollection* FindObjectCollection(const QByteArray& typeId) const;
	virtual imtbase::IDocumentChangeGenerator* FindDocumentChangeGenerator(const QByteArray& typeId) const;

	// reimplemented (sdl::imtbase::DocumentRevision::V1_0::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::DocumentRevision::CRevisionInfoList::V1_0 OnGetRevisionInfoList(const sdl::imtbase::DocumentRevision::V1_0::CGetRevisionInfoListGqlRequest& getRevisionInfoListRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::DocumentRevision::CBackupRevisionResponse::V1_0 OnBackupRevision(const sdl::imtbase::DocumentRevision::V1_0::CBackupRevisionGqlRequest& backupRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::DocumentRevision::CRestoreRevisionResponse::V1_0 OnRestoreRevision(const sdl::imtbase::DocumentRevision::V1_0::CRestoreRevisionGqlRequest& restoreRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::DocumentRevision::CExportRevisionResponse::V1_0 OnExportRevision(const sdl::imtbase::DocumentRevision::V1_0::CExportRevisionGqlRequest& exportRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::DocumentRevision::CDeleteRevisionResponse::V1_0 OnDeleteRevision(const sdl::imtbase::DocumentRevision::V1_0::CDeleteRevisionGqlRequest& deleteRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_MULTIATTR(QByteArray, m_collectionIdsAttrPtr);
	I_MULTIREF(imtbase::IObjectCollection, m_objectCollectionsCompPtr);
	I_MULTIREF(imtbase::IDocumentChangeGenerator, m_documentChangeGeneratorsCompPtr);
};


} // namespace imtservergql



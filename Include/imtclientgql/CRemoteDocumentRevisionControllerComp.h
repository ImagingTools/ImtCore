#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentRevision.h>


namespace imtclientgql
{


class CRemoteDocumentRevisionControllerComp: public imtclientgql::TClientRequestManagerCompWrap<sdl::imtbase::DocumentRevision::V1_0::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::imtbase::DocumentRevision::V1_0::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteDocumentRevisionControllerComp);
		I_ASSIGN_MULTI_0(m_collectionIdsAttrPtr, "CollectionIds", "Collection ID-s", false);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::DocumentRevision::V1_0::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::DocumentRevision::CRevisionInfoList::V1_0 OnGetRevisionInfoList(const sdl::imtbase::DocumentRevision::V1_0::CGetRevisionInfoListGqlRequest& getRevisionInfoListRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::DocumentRevision::CBackupRevisionResponse::V1_0 OnBackupRevision(const sdl::imtbase::DocumentRevision::V1_0::CBackupRevisionGqlRequest& backupRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::DocumentRevision::CRestoreRevisionResponse::V1_0 OnRestoreRevision(const sdl::imtbase::DocumentRevision::V1_0::CRestoreRevisionGqlRequest& restoreRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::DocumentRevision::CExportRevisionResponse::V1_0 OnExportRevision(const sdl::imtbase::DocumentRevision::V1_0::CExportRevisionGqlRequest& exportRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::DocumentRevision::CDeleteRevisionResponse::V1_0 OnDeleteRevision(const sdl::imtbase::DocumentRevision::V1_0::CDeleteRevisionGqlRequest& deleteRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	I_MULTIATTR(QByteArray, m_collectionIdsAttrPtr);
};


} // namespace imtclientgql



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentRevision>


namespace imtclientgql
{


class CRemoteDocumentRevisionControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<
						sdl::V1_0::imtbase::CDocumentRevisionGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<
				sdl::V1_0::imtbase::CDocumentRevisionGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteDocumentRevisionControllerComp);
		I_ASSIGN_MULTI_0(m_collectionIdsAttrPtr, "CollectionIds", "Collection ID-s", false);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CDocumentRevisionGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CRevisionInfoList OnGetRevisionInfoList(const sdl::V1_0::imtbase::CGetRevisionInfoListGqlRequest& getRevisionInfoListRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CBackupRevisionResponse OnBackupRevision(const sdl::V1_0::imtbase::CBackupRevisionGqlRequest& backupRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CRestoreRevisionResponse OnRestoreRevision(const sdl::V1_0::imtbase::CRestoreRevisionGqlRequest& restoreRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CExportRevisionResponse OnExportRevision(const sdl::V1_0::imtbase::CExportRevisionGqlRequest& exportRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDeleteRevisionResponse OnDeleteRevision(const sdl::V1_0::imtbase::CDeleteRevisionGqlRequest& deleteRevisionRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	I_MULTIATTR(QByteArray, m_collectionIdsAttrPtr);
};


} // namespace imtclientgql



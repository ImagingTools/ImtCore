#pragma once


// ACF includes
#include <ifile/ITempFileManager.h>

// ImtCore includes
#include <imtservergql/ICollectionImportController.h>
#include <imthype/IJobQueueManager.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionImport.h>


namespace imtservergql
{


namespace collectionImport = sdl::imtbase::CollectionImport;


class CGqlCollectionImportControllerComp: public collectionImport::CGraphQlHandlerCompBase
{
	Q_DECLARE_TR_FUNCTIONS(CGqlCollectionImportControllerComp)
public:
	typedef collectionImport::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlCollectionImportControllerComp);
		I_ASSIGN(m_collectionImportControllerCompPtr, "CollectionImportController", "Collection import controller", false, "CollectionImportController");
	I_END_COMPONENT;

	template<typename GqlRequest>
	static bool FillSessionInfo(
		const GqlRequest& request,
		imtservergql::ICollectionImportController::SessionInfo& sessionInfo,
		QString& errorMessage);

	template<typename GqlRequest>
	static QByteArray GetCancelSessionId(const GqlRequest& request);

protected:
	// reimplemented (sdl::imtbase::CollectionImport::CGraphQlHandlerCompBase)
	virtual collectionImport::CSessionStatus OnBeginCollectionImportSession(
		const collectionImport::CBeginCollectionImportSessionGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual collectionImport::CSessionStatus OnCancelCollectionImportSession(
		const collectionImport::CCancelCollectionImportSessionGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;

private:
	I_REF(imtservergql::ICollectionImportController, m_collectionImportControllerCompPtr);
};


} // namespace imtservergql



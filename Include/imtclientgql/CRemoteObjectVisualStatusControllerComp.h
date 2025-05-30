#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtclientgql
{


class CRemoteObjectVisualStatusControllerComp: public imtclientgql::TClientRequestManagerCompWrap<sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteObjectVisualStatusControllerComp);
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "TypeIds", "Remote object type-IDs", false);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::DocumentRevision::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CRemoveElementSetPayload OnRemoveElementSet(
				const sdl::imtbase::ImtCollection::CRemoveElementSetGqlRequest& removeElementSetRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);
};


} // namespace imtclientgql



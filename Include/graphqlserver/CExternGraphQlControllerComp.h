#pragma once


// ImtCore includes
#include <graphqlserver/graphqlserver.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>
#include <graphqlserver/CExternGraphQl.h>


namespace graphqlserver
{


class CExternGraphQlControllerComp:
			public imtservergql::CPermissibleGqlRequestHandlerComp,
			public CExternGraphQl
{
public:
	typedef imtservergql::CPermissibleGqlRequestHandlerComp BaseClass;

	I_BEGIN_COMPONENT(CExternGraphQlControllerComp);
	I_END_COMPONENT;

	bool RegisterGqlHandler(const QByteArray &commandId, graphqlserver::IGqlRequestHandler& gqlRequestHandler, QString& errorMessage);

protected:
	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	QMap<QByteArray, graphqlserver::IGqlRequestHandler*> m_gqlRequestHandlers;
};


} // namespace graphqlserver



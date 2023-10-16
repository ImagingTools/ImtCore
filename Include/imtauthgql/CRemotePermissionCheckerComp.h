#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IPermissionChecker.h>
#include <imtgql/IGqlRequestHandler.h>


namespace imtauthgql
{


class CRemotePermissionCheckerComp:
			public ilog::TLoggerCompWrap<icomp::CComponentBase>,
			virtual public imtauth::IPermissionChecker
{
public:
	typedef ilog::TLoggerCompWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CRemotePermissionCheckerComp)
		I_REGISTER_INTERFACE(imtauth::IPermissionChecker);
		I_ASSIGN(m_gqlRequestHandlerCompPtr, "GqlRequest", "GraphQL request handler", true, "GqlRequest");
		I_ASSIGN(m_permissionCheckerCompPtr, "PermissionChecker", "Permission checker", true, "PermissionChecker");
	I_END_COMPONENT;

	// reimplemented (imtauth::IPermissionChecker)
	virtual bool CheckPermission(const imtauth::IUserInfo::FeatureIds& userPermissions, const QByteArrayList& permissions) override;

protected:
	I_REF(imtgql::IGqlRequestHandler, m_gqlRequestHandlerCompPtr);
	I_REF(imtauth::IPermissionChecker, m_permissionCheckerCompPtr);
};


} // namespace imtauthgql



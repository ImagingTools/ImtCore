#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ISuperuserProvider.h>
#include <imtgql/IGqlRequestHandler.h>


namespace imtauthdb
{


class CRemoteSuperuserProviderComp: public icomp::CComponentBase, virtual public imtauth::ISuperuserProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteSuperuserProviderComp);
		I_REGISTER_INTERFACE(imtauth::ISuperuserProvider);
		I_ASSIGN(m_superuserIdAttrPtr, "SuperuserId", "ID of the superuser", true, "su");
		I_ASSIGN(m_gqlRequestCompPtr, "GqlRequest", "GraphQL request", true, "GqlRequest");
	I_END_COMPONENT;

	// reimplemented (imtauth::ISuperuserProvider)
	virtual bool SuperuserExists(QString& errorMessage) const override;
	virtual QByteArray GetSuperuserId() const override;

protected:
	I_ATTR(QByteArray, m_superuserIdAttrPtr);
	I_REF(imtgql::IGqlRequestHandler, m_gqlRequestCompPtr);
};


} // namespace imtauthdb



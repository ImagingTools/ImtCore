#pragma once


// ImtCore includes
#include <imtauth/ISuperuserProvider.h>
#include <imtgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CClientRequestRemoteSuperuserProviderComp:
			public imtgql::CClientRequestManagerCompBase,
			virtual public imtauth::ISuperuserProvider
{
public:
	typedef imtgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestRemoteSuperuserProviderComp)
		I_REGISTER_INTERFACE(imtauth::ISuperuserProvider);
		I_ASSIGN(m_superuserIdAttrPtr, "SuperuserId", "ID of the superuser", false, "su");
	I_END_COMPONENT;

	// reimplemented (imtauth::ISuperuserProvider)
	virtual bool SuperuserExists() const override;
	virtual QByteArray GetSuperuserId() const override;

protected:
	I_ATTR(QByteArray, m_superuserIdAttrPtr);
};


} // namespace imtauthgql



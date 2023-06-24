#pragma once


// ImtCore includes
#include <imtauth/ISuperuserController.h>
#include <imtgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CRemoteSuperuserControllerComp:
			public imtgql::CClientRequestManagerCompBase,
			virtual public imtauth::ISuperuserController
{
public:
	typedef imtgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteSuperuserControllerComp)
		I_REGISTER_INTERFACE(imtauth::ISuperuserController);
	I_END_COMPONENT;

	// reimplemented (ISuperuserController)
	virtual bool SetSuperuserPassword(const QByteArray& password) const override;
};


} // namespace imtauthgql



#pragma once


// ImtCore includes
#include <imtauth/ISuperuserController.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CRemoteSuperuserControllerComp:
			public imtclientgql::CClientRequestManagerCompBase,
			virtual public imtauth::ISuperuserController
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteSuperuserControllerComp)
		I_REGISTER_INTERFACE(imtauth::ISuperuserController);
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "ID of the related product", true, "");
	I_END_COMPONENT;

	// reimplemented (ISuperuserController)
	virtual bool SetSuperuserPassword(const QByteArray& password) const override;

private:
	I_ATTR(QByteArray, m_productIdAttrPtr);
};


} // namespace imtauthgql



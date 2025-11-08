#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/IGqlContextController.h>
#include <imtauth/IJwtSessionController.h>


namespace imtservergql
{


class CGqlContextControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlContextController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlContextControllerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextController);
		I_ASSIGN(m_sessionCollectionCompPtr, "SessionCollection", "Session collection", true, "SessionCollection");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", false, "UserSettingsCollection");
		I_ASSIGN(m_jwtSessionControllerCompPtr, "JwtSessionController", "JWT session controller", false, "JwtSessionController");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlContextController)
	virtual imtgql::IGqlContext* GetRequestContext(
				const imtgql::CGqlRequest& gqlRequest,
				const QByteArray& token,
				const imtgql::IGqlContext::Headers& headers,
				QString& errorMessage) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtbase::IObjectCollection, m_sessionCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_REF(imtauth::IJwtSessionController, m_jwtSessionControllerCompPtr);

	mutable QMutex m_mutex;
};


} // namespace imtservergql



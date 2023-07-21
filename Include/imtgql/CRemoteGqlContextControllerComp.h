#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
//#include <imtbase/IObjectCollection.h>
#include <imtauth/ISession.h>
#include <imtgql/IGqlContextController.h>
#include <imtgql/IGqlRequestHandler.h>


namespace imtgql
{


class CRemoteGqlContextControllerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlContextController
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteGqlContextControllerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextController);
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user", true, "UserFactory");
		I_ASSIGN(m_sessionInfoFactCompPtr, "SessionFactory", "Factory used for creation of the new session", true, "SessionFactory");
		I_ASSIGN(m_gqlRequestHandlerCompPtr, "GqlRequest", "GraphQL request handler", true, "GqlRequest");
//		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", false, "UserSettingsCollection");
		I_ASSIGN(m_cacheClearingIntervalAttrPtr, "CacheClearingInterval", "Interval for cache clearing (in seconds)", false, 60.0);
	I_END_COMPONENT;

	CRemoteGqlContextControllerComp();

	// reimplemented (imtgql::IGqlContextController)
	virtual imtgql::IGqlContext* GetRequestContext(const imtgql::CGqlRequest& gqlRequest, const QByteArray& token) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void ClearCache();

private Q_SLOTS:
	void OnTimeout();

private:
	mutable QMap<QByteArray, imtgql::IGqlContext*> m_cacheMap;
	QTimer m_timer;
#if QT_VERSION >= 0x060000
	mutable QRecursiveMutex m_mutex;
#else
	mutable QMutex m_mutex;
#endif

private:
	I_FACT(imtauth::IUserInfo, m_userInfoFactCompPtr);
	I_FACT(imtauth::ISession, m_sessionInfoFactCompPtr);
	I_REF(imtgql::IGqlRequestHandler, m_gqlRequestHandlerCompPtr);
//	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_ATTR(int, m_cacheClearingIntervalAttrPtr);
};


} // namespace imtgql



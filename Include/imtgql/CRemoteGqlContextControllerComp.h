#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgql/IGqlContextController.h>
#include <imtgql/IGqlRequestHandler.h>


namespace imtgql
{


class CRemoteGqlContextControllerComp: public QObject, public ilog::CLoggerComponentBase, virtual public imtgql::IGqlContextController
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteGqlContextControllerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextController);
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user", true, "UserFactory");
		I_ASSIGN(m_gqlRequestCompPtr, "GqlRequest", "GraphQL request", true, "GqlRequest");
		I_ASSIGN(m_cacheClearingIntervalAttrPtr, "CacheClearingInterval", "Interval for cache clearing (in seconds)", false, 60.0);
	I_END_COMPONENT;

	CRemoteGqlContextControllerComp();

	// reimplemented (imtgql::IGqlContextController)
	virtual imtgql::IGqlContext* GetGqlContext(const imtgql::CGqlRequest& gqlRequest, const QByteArray& token) const override;

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
	I_REF(imtgql::IGqlRequestHandler, m_gqlRequestCompPtr);
	I_ATTR(int, m_cacheClearingIntervalAttrPtr);
};


} // namespace imtgql



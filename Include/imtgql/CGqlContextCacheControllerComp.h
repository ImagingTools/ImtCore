#pragma once


// Qt includes
#include <QtCore/QTimer>


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgql/IGqlContextCacheController.h>
#include <imtgql/IGqlRequestHandler.h>


namespace imtgql
{


class CGqlContextCacheControllerComp: public QObject, public ilog::CLoggerComponentBase, virtual public imtgql::IGqlContextCacheController
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlContextCacheControllerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextCacheController);
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user", true, "UserFactory");
		I_ASSIGN(m_gqlRequestCompPtr, "GqlRequest", "GraphQL request", true, "GqlRequest");
		I_ASSIGN(m_cacheClearingIntervalAttrPtr, "CacheClearingInterval", "Interval for cache clearing", false, 60000);
	I_END_COMPONENT;

	~CGqlContextCacheControllerComp();

	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual bool TokenIsCached(const QByteArray& token) const override;
	virtual bool CacheToken(const QByteArray& token) override;
	virtual bool RemoveCache(const QByteArray& token) override;
	virtual const imtauth::IUserInfo* GetUserInfo(const QByteArray& token) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void ClearCache();

private Q_SLOTS:
	void OnTimeout();

private:
	QMap<QByteArray, const imtauth::IUserInfo*> m_cacheMap;
	QTimer m_timer;

private:
	I_FACT(imtauth::IUserInfo, m_userInfoFactCompPtr);
	I_REF(imtgql::IGqlRequestHandler, m_gqlRequestCompPtr);
	I_ATTR(int, m_cacheClearingIntervalAttrPtr);
};


} // namespace imtgql



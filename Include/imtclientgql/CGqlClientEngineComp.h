#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtclientgql/IClientProtocolEngine.h>


namespace imtclientgql
{


class CGqlClientEngineComp:
		public icomp::CComponentBase,
		virtual public imtclientgql::IClientProtocolEngine
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlClientEngineComp);
		I_REGISTER_INTERFACE(imtclientgql::IClientProtocolEngine);
		I_ASSIGN(m_urlParamCompPtr, "UrlParam", "The object holds connection's URL", false, "UrlParam");
		I_ASSIGN(m_prefixServerAttrPtr, "ServerPrefix", "Prefix Server", true, "/");
	I_END_COMPONENT;

	CGqlClientEngineComp();

	// reimplemented (imtclientgql::IClientProtocolEngine)
	virtual QNetworkRequest* CreateNetworkRequest(const imtgql::IGqlRequest& request) const override;

protected:
	void OnUrlParamChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IUrlParam* urlParamPtr);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_ATTR(QByteArray, m_prefixServerAttrPtr);
	I_REF(imtbase::IUrlParam, m_urlParamCompPtr);

	imtbase::TModelUpdateBinder<imtbase::IUrlParam, CGqlClientEngineComp> m_urlParamObserver;

	mutable QUrl m_workingUrl;
};


} // namespace imtclientgql



#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtgql/IClientProtocolEngine.h>


namespace imtgql
{


class CGqlClientEngineComp:
		public icomp::CComponentBase,
		virtual public imtgql::IClientProtocolEngine
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlClientEngineComp);
		I_REGISTER_INTERFACE(imtgql::IClientProtocolEngine);
		I_ASSIGN(m_urlAttrPtr, "Url", "The property holds connection's url.", true, "localhost");
		I_ASSIGN(m_urlParamCompPtr, "UrlParam", "The object holds connection's url.", false, "UrlParam");
	I_END_COMPONENT;

	CGqlClientEngineComp();

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imtgql::IClientProtocolEngine)
	virtual QNetworkRequest* CreateNetworkRequest(const imtgql::IGqlRequest& request) const override;

protected:
	void OnUrlParamChanged(const istd::IChangeable::ChangeSet& changeSet, const iprm::ITextParam* textParamPtr);

private:
	I_ATTR(QByteArray, m_urlAttrPtr);
	I_REF(iprm::ITextParam, m_urlParamCompPtr);

	imtbase::TModelUpdateBinder<iprm::ITextParam, CGqlClientEngineComp> m_textParamObserver;

	QByteArray m_workingUrl;
};


} // namespace imtgql



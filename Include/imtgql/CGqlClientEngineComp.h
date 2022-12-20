#pragma once


// ACF includes
#include <icomp/CComponentBase.h>


// ImtCore includes
#include <imtgql/IClientProtocolEngine.h>


// Qt includes
#include <QtQml/QQmlEngine>


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
	I_END_COMPONENT;

	// reimplemented (imtgql::IClientProtocolEngine)
	virtual QNetworkRequest* CreateNetworkRequest(const imtgql::IGqlRequest& request) const override;

private:
	I_ATTR(QByteArray, m_urlAttrPtr);
	static QString s_accessToken;
};


} // namespace imtgql



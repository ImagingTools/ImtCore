#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtservice/IConnectionCollection.h>
#include <imtservice/CUrlConnectionParam.h>


namespace imtservice
{


class CConnectionCollectionComp:
			public ilog::CLoggerComponentBase,
			virtual public IConnectionCollection
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CConnectionCollectionComp);
		I_REGISTER_INTERFACE(imtservice::IConnectionCollection);
		I_ASSIGN(m_serviceTypeName, "ServiceTypeName", "Service type name for service", true, "");
		I_ASSIGN_MULTI_0(m_connectionServiceTypeNames, "ConnectionServiceTypeNames", "Service type name for connections", true);
		I_ASSIGN_MULTI_0(m_connectionUsageIds, "ConnectionUsageIds", "Service connection usage ids", true);
		I_ASSIGN_MULTI_0(m_connectionDescriptions, "ConnectionDescriptions", "Service connection descriptions", true);
		I_ASSIGN_MULTI_0(m_connectionTypes, "ConnectionTypes", "Service connection types: 0 - input, 1 - output", true);
		I_ASSIGN_MULTI_0(m_connectionUrlListCompPtr, "ConnectionUrlListCompPtr", "Parameter providing the server ports", true);
	I_END_COMPONENT;

	// reimplemented (imtservice::IConnectionCollection)
	QString GetServiceTypeName() const override;
	virtual bool SetServiceTypeName(const QString& serviceTypeName) const override;
	virtual const imtbase::ICollectionInfo* GetUrlList() const override;
	virtual const QUrl* GetUrl(const QByteArray& id) const override;
	virtual const IServiceConnectionParam* GetConnectionMetaInfo(const QByteArray& id) const override;
	virtual bool SetUrl(const QByteArray& id, const QUrl& url) const override;
	virtual QByteArray InsertNewConnection(
						const QByteArray& connectionId,
						const QUrl& url,
						imtservice::IServiceConnectionParam::ConnectionType connectionType,
						const QString& name = QString(),
						const QString& description = QString() ) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_serviceTypeName);
	I_MULTIATTR(QByteArray, m_connectionServiceTypeNames);
	I_MULTIATTR(QByteArray, m_connectionUsageIds);
	I_MULTIATTR(QByteArray, m_connectionDescriptions);
	I_MULTIATTR(int, m_connectionTypes);
	I_MULTIREF(imtbase::IUrlParam, m_connectionUrlListCompPtr);

	imtbase::CObjectCollection m_collection;
};


} // namespace imtservice




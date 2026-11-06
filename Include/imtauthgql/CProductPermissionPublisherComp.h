// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

// ACF includes
#include <ibase/IApplicationInfo.h>
#include <ilog/TLoggerCompWrap.h>
#include <imod/CSingleModelObserverBase.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtclientgql/IServerCredentialProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtlic/IProductInfo.h>


namespace imtauthgql
{


/**
	The server of a product sends its permission tree, untranslated, to the authorization server
	at the start and after each reconnection; the receiver stores only a changed tree.
	A failure does not touch the work of the server: it is logged and repeated.
*/
class CProductPermissionPublisherComp:
			public QObject,
			public imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase>,
			protected imod::CSingleModelObserverBase
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CProductPermissionPublisherComp);
		I_ASSIGN(m_productInfoCompPtr, "FeatureContainer", "The product and its permission tree", true, "FeatureContainer");
		I_ASSIGN(m_serverCredentialCompPtr, "ServerCredentialProvider", "Token of this server", true, "ServerCredentialProvider");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Version of the product", false, "ApplicationInfo");
		I_ASSIGN(m_connectionStatusModelCompPtr, "ConnectionStatus", "Connection to the authorization server: a reconnection publishes again", false, "ConnectionStatus");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	bool Publish() const;
	QString GetProductVersion() const;
	void Run();

private:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
	I_REF(imtclientgql::IServerCredentialProvider, m_serverCredentialCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_REF(imod::IModel, m_connectionStatusModelCompPtr);

	QThread* m_threadPtr = nullptr;
	QMutex m_mutex;
	QWaitCondition m_condition;
	bool m_isStopping = false;
	bool m_isPublishNeeded = true;
};


} // namespace imtauthgql

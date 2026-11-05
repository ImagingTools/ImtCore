// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtclientgql/IServerCredentialProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtlic/IProductInfo.h>


namespace imtauthgql
{


/**
	The server of a product sends its permission tree to the authorization server, in all its languages.
	Repeated regularly, so a reconnected server or a new version is seen soon; the receiver stores only a changed tree.
	A failure does not touch the work of the server: it is logged and repeated.
*/
class CProductPermissionPublisherComp: public imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CProductPermissionPublisherComp);
		I_ASSIGN(m_productInfoCompPtr, "FeatureContainer", "The product and its permission tree", true, "FeatureContainer");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translations of the permission tree", false, "TranslationManager");
		I_ASSIGN(m_serverCredentialCompPtr, "ServerCredentialProvider", "Token of this server", true, "ServerCredentialProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	bool Publish() const;
	void Run();

private:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
	I_REF(imtclientgql::IServerCredentialProvider, m_serverCredentialCompPtr);

	QThread* m_threadPtr = nullptr;
	QMutex m_mutex;
	QWaitCondition m_condition;
	bool m_isStopping = false;
};


} // namespace imtauthgql

// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtauth/ILoginStatusProvider.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtauthgql
{


class CLoginStatusSubscriberControllerComp:
			public imtservergql::CGqlPublisherCompBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<istd::IChangeable> BaseClass2;

	I_BEGIN_COMPONENT(CLoginStatusSubscriberControllerComp);
		I_ASSIGN(m_loginStatusProviderCompPtr, "LoginStatusProvider", "Login status provider", true, "LoginStatusProvider");
		I_ASSIGN_TO(m_loginStatusModelCompPtr, m_loginStatusProviderCompPtr, true);
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlPublisherCompBase)
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const imtgql::CGqlRequest& gqlRequest,
				const imtrest::IRequest& networkRequest,
				QString& errorMessage) override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_REF(imtauth::ILoginStatusProvider, m_loginStatusProviderCompPtr);
	I_REF(imod::IModel, m_loginStatusModelCompPtr);
};


} // namespace imtauthgql



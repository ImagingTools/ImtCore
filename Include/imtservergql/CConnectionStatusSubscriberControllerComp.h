// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtcom/IConnectionStatusProvider.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtservergql
{


class CConnectionStatusSubscriberControllerComp:
			public imtservergql::CGqlPublisherCompBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<istd::IChangeable> BaseClass2;

	I_BEGIN_COMPONENT(CConnectionStatusSubscriberControllerComp);
		I_ASSIGN(m_connectionStatusProviderCompPtr, "LoginStatusProvider", "Login status provider", true, "LoginStatusProvider");
		I_ASSIGN_TO(m_connectionStatusModelCompPtr, m_connectionStatusProviderCompPtr, true);
	I_END_COMPONENT;

protected:
	QString CreateBodySubscription() const;

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
	I_REF(imtcom::IConnectionStatusProvider, m_connectionStatusProviderCompPtr);
	I_REF(imod::IModel, m_connectionStatusModelCompPtr);
};


} // namespace imtservergql



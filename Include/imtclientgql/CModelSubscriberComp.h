// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtclientgql/CSubscriberCompBase.h>


namespace imtclientgql
{


class CModelSubscriberComp: public CSubscriberCompBase
{
public:
	typedef CSubscriberCompBase BaseClass;

	I_BEGIN_COMPONENT(CModelSubscriberComp);
		I_ASSIGN(m_changeableModelCompPtr, "Model", "Data model to be synchronized with the changes on the server side", true, "Model");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData) override;

private:
	I_REF(istd::IChangeable, m_changeableModelCompPtr);
};


} // namespace imtclientgql



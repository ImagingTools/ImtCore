// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IExternalIdentityManager.h>


namespace imtauth
{


class CExternalIdentityManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public IExternalIdentityManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CExternalIdentityManagerComp);
		I_REGISTER_INTERFACE(IExternalIdentityManager);
		I_ASSIGN(m_identityCollectionCompPtr, "IdentityCollection", "Collection of the external identity links", true, "ExternalIdentityCollection");
		I_ASSIGN(m_identityFactoryCompPtr, "IdentityFactory", "Factory of the external identity links", true, "ExternalIdentity");
	I_END_COMPONENT;

	// reimplemented (imtauth::IExternalIdentityManager)
	virtual IExternalIdentitySharedPtr FindIdentity(const QByteArray& providerId, const QString& subject) const override;
	virtual Identities GetUserIdentities(const QByteArray& userId) const override;
	virtual QByteArray LinkIdentity(const QByteArray& userId, const QByteArray& providerId, const QString& subject, const QString& email) const override;
	virtual bool RegisterSignIn(const QByteArray& identityId, const QString& email) const override;
	virtual bool RemoveIdentity(const QByteArray& identityId) const override;

private:
	Identities FindIdentities(const QByteArray& fieldId, const QVariant& value, const QByteArray& secondFieldId = QByteArray(), const QVariant& secondValue = QVariant()) const;
	IExternalIdentitySharedPtr GetIdentity(const QByteArray& identityId) const;

private:
	I_REF(imtbase::IObjectCollection, m_identityCollectionCompPtr);
	I_FACT(IExternalIdentity, m_identityFactoryCompPtr);
};


} // namespace imtauth



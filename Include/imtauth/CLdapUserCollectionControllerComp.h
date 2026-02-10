// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IEnableableParam.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/CObjectCollection.h>
#include <imtauth/IUserInfo.h>


namespace imtauth
{


class CLdapUserCollectionControllerComp:
			public icomp::CComponentBase,
			public imod::TModelWrap<imtbase::CObjectCollection>
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::TModelWrap<imtbase::CObjectCollection> BaseClass2;

	I_BEGIN_COMPONENT(CLdapUserCollectionControllerComp);
		I_REGISTER_INTERFACE(imod::IModel)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection)
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "The interval for synchronization LDAP users (in secs)", false, 60);
	I_END_COMPONENT;

	CLdapUserCollectionControllerComp();

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	class CheckLdapUsersThread: public QThread
	{
	public:
		CheckLdapUsersThread(CLdapUserCollectionControllerComp& parent);
		void Start();
		QByteArrayList GetLdapUserIds() const;
		const imtauth::IUserInfo* GetUserInfoFromLdapUserId(const QByteArray& userId) const;

	protected:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		CLdapUserCollectionControllerComp& m_parent;
	};

protected:
	CheckLdapUsersThread m_checkLdapUsersThreadThread;

protected:
	I_ATTR(int, m_checkIntervalAttrPtr);
};


} // namespace imtauth



#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserInfo.h>


namespace imtauth
{


class CLdapUserCollectionControllerComp:
			public QObject,
			public icomp::CComponentBase
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLdapUserCollectionControllerComp);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection)
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
		I_ASSIGN(m_enableableParamCompPtr, "EnableableParam", "Enable/disable", false, "EnableableParam");
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "The interval for synchronization LDAP users", false, 60000);
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
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(iprm::IEnableableParam, m_enableableParamCompPtr);
	I_ATTR(int, m_checkIntervalAttrPtr);
};


} // namespace imtauth

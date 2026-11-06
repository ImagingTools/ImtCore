// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/IEnableableParam.h>

// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtauth/CLdapGroupChangeListener.h>
#include <imtauthgql/CAuthorizationControllerComp.h>


namespace imtauthgql
{


class CLdapAuthorizationControllerComp: public CAuthorizationControllerComp
{
public:
	typedef CAuthorizationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CLdapAuthorizationControllerComp);
		I_ASSIGN(m_systemIdAttrPtr, "SystemId", "System-ID", false, "Ldap");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection", false, "RoleCollection");
		I_ASSIGN(m_enableableParamCompPtr, "LdapEnableableParam", "Ldap enableable param", false, "LdapEnableableParam");
		I_ASSIGN(m_groupSettingsFileAttrPtr, "LdapGroupSettingsFile", "Absolute path to persistent LDAP group mapping JSON; empty disables administration and mapping", false, "");
		I_ASSIGN(m_groupChangeNotificationsAttrPtr, "LdapGroupChangeNotifications", "Receive Active Directory change notifications and keep the directory group catalog synchronized", true, true);
	I_END_COMPONENT;

	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	virtual QJsonObject CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool PrepareAuthorization(imtauth::CUserInfo& userInfo, const QByteArray& productId, QString& errorMessage) const override;
	virtual bool DiscoverDirectoryGroups(const QString& pattern, QMap<QByteArray, QString>& groups) const;
	virtual bool ReadDirectoryMembership(const QByteArray& userSid, QByteArrayList& groups) const;
	virtual QByteArray GetDirectoryUserSid(const QByteArray& login) const;
	bool ReadGroupSettings(QJsonObject& settings, QString& errorMessage, bool allowMissing = false) const;
	bool ValidateGroupSettings(const QJsonObject& settings, const QByteArray& productId, QString& errorMessage) const;

	/**
		Reconciles the server-owned directory group catalog of every product with Active Directory.

		The catalog lists the security groups that match the product's pattern (SID and current name).
		Created, renamed and deleted groups are added, renamed and removed. Mappings, manual role
		assignments and directory role grants of users are never changed, and no mapping is created.
		A product whose groups cannot be read completely keeps its last synchronized catalog.
		\return false if the catalog of at least one product could not be synchronized.
	*/
	bool SynchronizeDirectoryGroups(QString& errorMessage) const;

	/**
		Receives the events of the directory change listener on the listener thread.
	*/
	void OnDirectoryChangeEvent(imtauth::CLdapGroupChangeListener::Event event, const QString& message);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;
	enum RoleType
	{
		RT_DEFAULT,
		RT_GUEST
	};

	virtual QByteArray CheckExistsRole(const QByteArray& productId, RoleType roleType, bool* isReadPtr = nullptr) const;
	/**
		Atomically ensure that a default or guest role exists for the given product.

		The check for an existing role and the creation of a missing one are guarded
		against concurrent access to the role collection, so two concurrent
		authorization requests cannot each insert a duplicate role.

		\param productId Product the role belongs to.
		\param roleType Whether a default or guest role is requested.
		\param roleId Identifier used when a new role has to be created.
		\param roleName Display name used when a new role has to be created.
		\param description Description used when a new role has to be created.
		\return Object ID of the existing or newly created role, empty on failure.
	*/
	virtual QByteArray EnsureRoleExists(
				const QByteArray& productId,
				RoleType roleType,
				const QByteArray& roleId,
				const QString& roleName,
				const QString& description) const;
	virtual QByteArray InsertNewIdentifiableRoleInfo(
				const QByteArray& roleId,
				const QString& roleName,
				const QString& description,
				const QByteArray& productId,
				bool isDefault,
				bool isGuest) const;
	virtual istd::TUniqueInterfacePtr<imtauth::IUserInfo> CreateUserInfoFromLdapUser(const QByteArray& ldapUserId) const;
	QByteArray GetUserObjectIdBySid(const QByteArray& sid, bool* isReadPtr = nullptr) const;

	// reimplemented (CAuthorizationControllerComp)
	virtual sdl::V1_0::imtauth::CAuthorizationPayload OnAuthorization(
				const sdl::V1_0::imtauth::CAuthorizationGqlRequest& authorizationRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CAuthorizationPayload OnUserToken(
				const sdl::V1_0::imtauth::CUserTokenGqlRequest& userTokenRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	bool IsGroupSettingsConfigured() const;
	QString GetGroupCatalogFilePath() const;
	bool ReadGroupCatalog(QJsonObject& catalogs, QString& errorMessage) const;
	bool WriteGroupCatalog(const QJsonObject& catalogs, QString& errorMessage) const;
	void SetNotificationState(const QString& state, const QString& message);
	void MarkSynchronized(const QStringList& products) const;
	void SetSynchronizationError(const QString& errorMessage) const;

private:
	I_REF(iprm::IEnableableParam, m_enableableParamCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_ATTR(QByteArray, m_systemIdAttrPtr);
	I_ATTR(QString, m_groupSettingsFileAttrPtr);
	I_ATTR(bool, m_groupChangeNotificationsAttrPtr);
	mutable QMutex m_groupSettingsMutex;

	/**
		Guards the notification state and the last synchronization result that are reported to administrators.
	*/
	mutable QMutex m_directoryStatusMutex;
	QString m_notificationState;
	QString m_notificationMessage;
	mutable QString m_synchronizationError;
	mutable QMap<QString, QString> m_synchronizedAt;

	imtauth::CLdapGroupChangeListener m_groupChangeListener;

	/**
		Guards concurrent access to the role collection while a default or guest
		role is looked up and, if missing, created, so the same role is not
		written twice for one product.
	*/
	mutable QMutex m_roleCollectionMutex;
};


} // namespace imtauthgql

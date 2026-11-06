// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/IEnableableParam.h>

// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QJsonObject>

// ImtCore includes
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
	I_REF(iprm::IEnableableParam, m_enableableParamCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_ATTR(QByteArray, m_systemIdAttrPtr);
	I_ATTR(QString, m_groupSettingsFileAttrPtr);
	mutable QMutex m_groupSettingsMutex;

	/**
		Guards concurrent access to the role collection while a default or guest
		role is looked up and, if missing, created, so the same role is not
		written twice for one product.
	*/
	mutable QMutex m_roleCollectionMutex;
};


} // namespace imtauthgql

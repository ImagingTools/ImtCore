// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CLdapAuthorizationControllerComp.h>


// Windows includes
#ifdef Q_OS_WIN
#include <windows.h>
#include <LMaccess.h>
#include <Lmapibuf.h>
#pragma comment(lib, "netapi32.lib")
#endif

// Qt includes
#include <QtCore/QMutexLocker>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSaveFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QRegularExpression>
#include <QtCore/QLockFile>

// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/CLdapUserCollectionControllerComp.h>
#include <imtauth/CLdapGroupDirectory.h>
#include <imtauth/CollectionSelection.h>
#include <imtauthgql/imtauthgql.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/COperationContext.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace imtauthgql
{

namespace
{
bool IsGroupCommand(const QByteArray& command)
{
	return command == "LdapGroupSettings" || command == "SaveLdapGroupSettings" || command == "DiscoverLdapGroups";
}

bool IsProductId(const QByteArray& productId)
{
	return !productId.isEmpty() && productId.size() <= 256 && !productId.contains('\0') &&
		!imtauth::CUserBaseInfo::IsReservedProductId(productId);
}


bool IsGroupSid(const QString& sid)
{
	static const QRegularExpression sidExpression(QStringLiteral("^S-1-[0-9]+(?:-[0-9]+)+$"));

	return sidExpression.match(sid).hasMatch();
}


QJsonObject CreateCatalogEntry(const QString& pattern, const QMap<QByteArray, QString>& groups)
{
	QJsonObject groupNames;
	for (auto it = groups.constBegin(); it != groups.constEnd(); ++it){
		groupNames.insert(QString::fromUtf8(it.key()), it.value());
	}

	return QJsonObject{{"pattern", pattern}, {"groups", groupNames}};
}


QJsonArray CreateCatalogRows(const QJsonObject& catalog, const QString& pattern)
{
	QJsonArray rows;

	// A catalog of another pattern does not describe the groups of the current settings.
	if (pattern.isEmpty() || catalog.value("pattern").toString() != pattern){
		return rows;
	}

	QJsonObject groups = catalog.value("groups").toObject();
	for (auto it = groups.constBegin(); it != groups.constEnd(); ++it){
		if (IsGroupSid(it.key()) && it.value().isString() && !it.value().toString().isEmpty()){
			rows.append(QJsonObject{{"id", it.key()}, {"name", it.value().toString()}});
		}
	}

	return rows;
}


const char* const s_logSource = "imtauthgql::CLdapAuthorizationControllerComp";
const qint64 s_maximumCatalogSize = 16 * 1024 * 1024;


}


bool CLdapAuthorizationControllerComp::IsRequestSupported(const imtgql::CGqlRequest& request) const
{
	return IsGroupCommand(request.GetCommandId()) || BaseClass::IsRequestSupported(request);
}


bool CLdapAuthorizationControllerComp::DiscoverDirectoryGroups(const QString& pattern, QMap<QByteArray, QString>& groups) const
{
	return imtauth::CLdapGroupDirectory::DiscoverGroups(pattern, groups);
}


bool CLdapAuthorizationControllerComp::ReadDirectoryMembership(const QByteArray& userSid, QByteArrayList& groups) const
{
	return imtauth::CLdapGroupDirectory::GetSecurityGroups(userSid, groups);
}

QByteArray CLdapAuthorizationControllerComp::GetDirectoryUserSid(const QByteArray& login) const
{
	return imtauth::CLdapUserCollectionControllerComp::GetSidForUser(login);
}


bool CLdapAuthorizationControllerComp::ReadGroupSettings(QJsonObject& settings, QString& errorMessage, bool allowMissing) const
{
	settings = QJsonObject();
	if (!IsGroupSettingsConfigured()){
		return true;
	}
	QFile file(*m_groupSettingsFileAttrPtr);
	if (!QFileInfo(file).isAbsolute()){
		errorMessage = QStringLiteral("LDAP group settings require an absolute server-side file path");
		return false;
	}
	if (!file.exists()){
		if (!allowMissing){
			errorMessage = QStringLiteral("LDAP group settings file is missing or inaccessible");
		}
		return allowMissing;
	}
	if (!file.open(QIODevice::ReadOnly) || file.size() > 1024 * 1024){
		errorMessage = QStringLiteral("Unable to read LDAP group settings");
		return false;
	}
	QJsonParseError parseError;
	QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
	if (file.error() != QFileDevice::NoError || parseError.error != QJsonParseError::NoError || !document.isObject() ||
		document.object().value("version").toInt() != 1 || !document.object().value("products").isObject()){
		errorMessage = QStringLiteral("Invalid LDAP group settings file");
		return false;
	}
	settings = document.object().value("products").toObject();
	return true;
}


bool CLdapAuthorizationControllerComp::ValidateGroupSettings(
	const QJsonObject& settings, const QByteArray& productId, QString& errorMessage) const
{
	if (!IsProductId(productId) || !settings.value("enabled").isBool() ||
		!settings.value("pattern").isString() || !settings.value("mappings").isObject()){
		errorMessage = QStringLiteral("Invalid LDAP group settings");
		return false;
	}
	QString pattern = settings.value("pattern").toString();
	QJsonObject mappings = settings.value("mappings").toObject();
	if (pattern.size() > 256 || pattern.contains(QChar('\0')) || mappings.size() > 1000 ||
		(settings.value("enabled").toBool() && pattern.isEmpty())){
		errorMessage = QStringLiteral("A group-name pattern is required (only '*' is a wildcard)");
		return false;
	}
	QByteArrayList pendingRoles;
	for (auto mapping = mappings.begin(); mapping != mappings.end(); ++mapping){
		if (!IsGroupSid(mapping.key()) || !mapping.value().isArray() ||
			mapping.value().toArray().size() > 256){
			errorMessage = QStringLiteral("Invalid LDAP group mapping");
			return false;
		}
		for (const QJsonValue& value : mapping.value().toArray()){
			QByteArray roleId = value.toString().toUtf8();
			if (!value.isString() || roleId.isEmpty()){
				errorMessage = QStringLiteral("Invalid mapped role ID");
				return false;
			}
			pendingRoles.append(roleId);
		}
	}
	if (!settings.value("enabled").toBool()){
		return true;
	}
	QSet<QByteArray> visited;
	while (!pendingRoles.isEmpty()){
		QByteArray roleId = pendingRoles.takeFirst();
		if (visited.contains(roleId)){ continue; }
		visited.insert(roleId);
		imtbase::IObjectCollection::DataPtr roleData;
		if (visited.size() > 10000 || !m_roleCollectionCompPtr.IsValid() ||
			!m_roleCollectionCompPtr->GetObjectData(roleId, roleData)){
			errorMessage = QStringLiteral("Unable to read a mapped role or its parents");
			return false;
		}
		const imtauth::IRole* role = dynamic_cast<const imtauth::IRole*>(roleData.GetPtr());
		if (role == nullptr || role->GetProductId() != productId){
			errorMessage = QStringLiteral("A mapped role or its parent does not belong to this product");
			return false;
		}
		pendingRoles += role->GetIncludedRoles();
	}
	return true;
}


QJsonObject CLdapAuthorizationControllerComp::CreateInternalResponse(
	const imtgql::CGqlRequest& request, QString& errorMessage) const
{
	const QByteArray command = request.GetCommandId();
	if (!IsGroupCommand(command)){
		return BaseClass::CreateInternalResponse(request, errorMessage);
	}
	if (!IsSuperuserRequest(request)){
		errorMessage = QStringLiteral("LDAP group administration requires a superuser");
		return QJsonObject();
	}
	if (!IsGroupSettingsConfigured()){
		errorMessage = QStringLiteral("LDAP group administration is not configured");
		return QJsonObject();
	}
	const imtgql::CGqlParamObject* input = request.GetParamObject("input");
	QByteArray productId = input != nullptr ? input->GetParamArgumentValue("productId").toByteArray() : QByteArray();
	if (!IsProductId(productId)){
		errorMessage = QStringLiteral("A valid product ID is required");
		return QJsonObject();
	}
	if ((command == "SaveLdapGroupSettings") != (request.GetRequestType() == imtgql::IGqlRequest::RT_MUTATION)){
		errorMessage = QStringLiteral("Invalid LDAP group request type");
		return QJsonObject();
	}
	QMutexLocker locker(&m_groupSettingsMutex);
	QLockFile fileLock(*m_groupSettingsFileAttrPtr + QStringLiteral(".lock"));
	if (command == "SaveLdapGroupSettings" && !fileLock.tryLock(1000)){
		errorMessage = QStringLiteral("LDAP group settings are being updated");
		return QJsonObject();
	}
	QJsonObject products;
	if (!ReadGroupSettings(products, errorMessage, true)){
		return QJsonObject();
	}
	const QString product = QString::fromUtf8(productId);
	QJsonObject settings = products.value(product).toObject();
	if (!products.contains(product)){
		settings = QJsonObject{{"enabled", false}, {"pattern", ""}, {"mappings", QJsonObject()}};
	}
	QJsonObject response;
	if (command == "SaveLdapGroupSettings"){
		QByteArray json = input->GetParamArgumentValue("settings").toString().toUtf8();
		QJsonParseError parseError;
		QJsonDocument document = QJsonDocument::fromJson(json, &parseError);
		if (json.size() > 512 * 1024 || parseError.error != QJsonParseError::NoError || !document.isObject()){
			errorMessage = QStringLiteral("Invalid LDAP group settings JSON");
			return QJsonObject();
		}
		settings = document.object();
		if (!ValidateGroupSettings(settings, productId, errorMessage)){
			return QJsonObject();
		}
		// Only the administrator-owned fields are stored; the group catalog is owned by the server.
		settings = QJsonObject{
			{"enabled", settings.value("enabled")},
			{"pattern", settings.value("pattern")},
			{"mappings", settings.value("mappings")}};
		const QString pattern = settings.value("pattern").toString();
		QMap<QByteArray, QString> groups;
		bool hasGroups = false;
		if (settings.value("enabled").toBool()){
			if (!DiscoverDirectoryGroups(pattern, groups)){
				errorMessage = QStringLiteral("Unable to discover AD security groups");
				return QJsonObject();
			}
			hasGroups = true;
			QJsonObject mappings = settings.value("mappings").toObject();
			for (auto it = mappings.begin(); it != mappings.end(); ++it){
				if (!groups.contains(it.key().toUtf8())){
					errorMessage = QStringLiteral("A mapped group does not match the directory pattern");
					return QJsonObject();
				}
			}
		}
		else if (!pattern.isEmpty()){
			// Disabled settings may be saved while the directory is unavailable; the catalog is then refreshed later.
			hasGroups = DiscoverDirectoryGroups(pattern, groups);
		}
		products.insert(product, settings);
		QByteArray data = QJsonDocument(QJsonObject{{"version", 1}, {"products", products}}).toJson();
		if (data.size() > 1024 * 1024){
			errorMessage = QStringLiteral("LDAP group settings are too large");
			return QJsonObject();
		}
		QSaveFile file(*m_groupSettingsFileAttrPtr);
		if (!file.open(QIODevice::WriteOnly) ||
			!file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner) ||
			file.write(data) != data.size() || !file.commit()){
			errorMessage = QStringLiteral("Unable to persist LDAP group settings");
			return QJsonObject();
		}
		response.insert("ok", true);
		QJsonObject catalogs;
		QString catalogError;
		if (!ReadGroupCatalog(catalogs, catalogError)){
			locker.unlock();
			fileLock.unlock();
			catalogError.clear();
			if (!SynchronizeDirectoryGroups(catalogError)){
				SetSynchronizationError(catalogError);
				SendWarningMessage(0, catalogError, s_logSource);
			}
			locker.relock();
		}
		else{
			if (hasGroups){
				catalogs.insert(product, CreateCatalogEntry(pattern, groups));
			}
			else if (catalogs.value(product).toObject().value("pattern").toString() != pattern){
				catalogs.remove(product);
			}
			if (WriteGroupCatalog(catalogs, catalogError)){
				if (hasGroups){
					MarkSynchronized(QStringList{product});
				}
			}
			else{
				SetSynchronizationError(catalogError);
				SendErrorMessage(0, catalogError, s_logSource);
			}
		}
	}
	else if (command == "DiscoverLdapGroups"){
		QString pattern = input->GetParamArgumentValue("pattern").toString();
		QMap<QByteArray, QString> groups;
		if (!DiscoverDirectoryGroups(pattern, groups)){
			errorMessage = QStringLiteral("Unable to discover AD security groups");
			return QJsonObject();
		}
		QJsonArray groupRows;
		for (auto it = groups.begin(); it != groups.end(); ++it){
			groupRows.append(QJsonObject{{"id", QString::fromUtf8(it.key())}, {"name", it.value()}});
		}
		response.insert("groups", groupRows);
	}
	if (command == "LdapGroupSettings"){
		if (!m_roleCollectionCompPtr.IsValid()){
			errorMessage = QStringLiteral("LDAP role collection is not configured");
			return QJsonObject();
		}
		bool complete = false;
		QByteArrayList ids = imtauth::CollectionSelection::SelectIds(*m_roleCollectionCompPtr,
			imtauth::CollectionSelection::ColumnEquals("ProductId", productId), {"ProductId"}, &complete);
		QJsonArray roles;
		if (!complete){
			errorMessage = QStringLiteral("Unable to select product roles");
			return QJsonObject();
		}
		for (const QByteArray& id : ids){
			imtbase::IObjectCollection::DataPtr data;
			if (!m_roleCollectionCompPtr->GetObjectData(id, data)){
				errorMessage = QStringLiteral("Unable to read product roles");
				return QJsonObject();
			}
			const imtauth::IRole* role = dynamic_cast<const imtauth::IRole*>(data.GetPtr());
			if (role == nullptr){
				errorMessage = QStringLiteral("Invalid role data");
				return QJsonObject();
			}
			if (role->GetProductId() == productId){
				roles.append(QJsonObject{{"id", QString::fromUtf8(id)}, {"name", role->GetRoleName()}});
			}
		}
		response.insert("roles", roles);
	}
	if (command != "DiscoverLdapGroups"){
		QJsonObject catalogs;
		QString catalogError;
		if (!ReadGroupCatalog(catalogs, catalogError)){
			locker.unlock();
			catalogError.clear();
			SynchronizeDirectoryGroups(catalogError);
			locker.relock();
			if (!ReadGroupCatalog(catalogs, catalogError)){
				catalogs = QJsonObject();
			}
		}
		response.insert("groups", CreateCatalogRows(catalogs.value(product).toObject(), settings.value("pattern").toString()));
		QMutexLocker statusLocker(&m_directoryStatusMutex);
		QStringList problems;
		problems << m_notificationMessage << m_synchronizationError << catalogError;
		problems.removeAll(QString());
		response.insert("notificationState", m_notificationState.isEmpty() ? QStringLiteral("Disabled") : m_notificationState);
		response.insert("synchronizedAt", m_synchronizedAt.value(product));
		response.insert("synchronizationError", problems.join(QStringLiteral(" ")));
	}
	response.insert("settings", QString::fromUtf8(QJsonDocument(settings).toJson(QJsonDocument::Compact)));
	return QJsonObject{{"data", response}};
}


bool CLdapAuthorizationControllerComp::PrepareAuthorization(
	imtauth::CUserInfo& userInfo, const QByteArray& productId, QString& errorMessage) const
{
	QMutexLocker locker(&m_groupSettingsMutex);
	bool ldapUser = false;
	for (const imtauth::IUserInfo::SystemInfo& system : userInfo.GetSystemInfos()){
		ldapUser = ldapUser || (system.enabled && system.systemId == *m_systemIdAttrPtr);
	}
	if (!ldapUser && userInfo.GetDirectoryRoles(productId).isEmpty()){
		return true;
	}
	if (!IsProductId(productId)){
		if (productId.isEmpty()){
			for (const QByteArray& product : userInfo.GetProducts()){
				if (!userInfo.GetDirectoryRoles(product).isEmpty()){
					errorMessage = QStringLiteral("LDAP group authorization requires a product ID");
					return false;
				}
			}
			if (!IsGroupSettingsConfigured()){
				return true;
			}
			QJsonObject products;
			if (!ReadGroupSettings(products, errorMessage)){
				return false;
			}
			bool mappingEnabled = false;
			for (auto it = products.begin(); it != products.end(); ++it){
				if (!it.value().isObject() || !it.value().toObject().value("enabled").isBool()){
					errorMessage = QStringLiteral("Invalid LDAP group settings");
					return false;
				}
				mappingEnabled = mappingEnabled || it.value().toObject().value("enabled").toBool();
			}
			if (!mappingEnabled){
				return true;
			}
		}
		errorMessage = QStringLiteral("LDAP group authorization requires a product ID");
		return false;
	}
	QByteArrayList roles;
	if (ldapUser && m_enableableParamCompPtr.IsValid() && m_enableableParamCompPtr->IsEnabled()){
		QJsonObject products;
		if (!ReadGroupSettings(products, errorMessage)){
			return false;
		}
		QJsonValue product = products.value(QString::fromUtf8(productId));
		if (!product.isUndefined() && !product.isObject()){
			errorMessage = QStringLiteral("Invalid product LDAP group settings");
			return false;
		}
		QJsonObject settings = product.toObject();
		if (!product.isUndefined() && !ValidateGroupSettings(settings, productId, errorMessage)){
			return false;
		}
		if (settings.value("enabled").toBool()){
			QByteArrayList memberships;
			QMap<QByteArray, QString> groups;
			QByteArray sid = GetDirectoryUserSid(userInfo.GetId());
			if (sid.isEmpty() || userInfo.GetSid() != sid){
				errorMessage = QStringLiteral("Unable to verify the stored LDAP account identity");
				return false;
			}
			if (!ReadDirectoryMembership(sid, memberships) ||
				!DiscoverDirectoryGroups(settings.value("pattern").toString(), groups)){
				errorMessage = QStringLiteral("Unable to refresh AD security group membership");
				return false;
			}
			QJsonObject mappings = settings.value("mappings").toObject();
			for (const QByteArray& sid : memberships){
				if (!groups.contains(sid)){
					continue;
				}
				for (const QJsonValue& role : mappings.value(QString::fromUtf8(sid)).toArray()){
					QByteArray roleId = role.toString().toUtf8();
					if (!roles.contains(roleId)){ roles.append(roleId); }
				}
			}
		}
	}
	if (roles == userInfo.GetDirectoryRoles(productId)){
		if (!roles.isEmpty() && userInfo.GetRoleProvider() == nullptr){
			errorMessage = QStringLiteral("LDAP mapped users require a configured role provider");
			return false;
		}
		return true;
	}
	// Reload before changing only our source-owned grants; manual assignments are never removed.
	if (!m_userCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("LDAP user collection is not configured");
		return false;
	}
	QByteArray objectId = GetUserObjectId(userInfo.GetId());
	imtbase::IObjectCollection::DataPtr data;
	QMutexLocker userUpdateLocker(&imtauth::CUserBaseInfo::GetUserUpdateMutex(objectId));
	if (objectId.isEmpty() || !m_userCollectionCompPtr.IsValid() ||
		!m_userCollectionCompPtr->GetObjectData(objectId, data)){
		errorMessage = QStringLiteral("Unable to load LDAP role grants");
		return false;
	}
	istd::IChangeableUniquePtr copy = data->CloneMe();
	imtauth::CUserInfo* storedUser = dynamic_cast<imtauth::CUserInfo*>(copy.GetPtr());
	if (storedUser == nullptr || storedUser->GetId() != userInfo.GetId() || !storedUser->IsEnabled()){
		errorMessage = QStringLiteral("Invalid LDAP user data");
		return false;
	}
	if (!roles.isEmpty() && storedUser->GetRoleProvider() == nullptr){
		errorMessage = QStringLiteral("LDAP mapped users require a configured persistent user role provider");
		return false;
	}
	storedUser->SetDirectoryRoles(productId, roles);
	if (!m_userCollectionCompPtr->SetObjectData(objectId, *storedUser)){
		errorMessage = QStringLiteral("Unable to persist LDAP role grants");
		return false;
	}
	if (!userInfo.CopyFrom(*storedUser)){
		errorMessage = QStringLiteral("Unable to apply persisted LDAP role grants");
		return false;
	}
	return true;
}


bool CLdapAuthorizationControllerComp::SynchronizeDirectoryGroups(QString& errorMessage) const
{
	if (!IsGroupSettingsConfigured()){
		return true;
	}

	QMap<QString, QString> patterns;
	{
		QMutexLocker locker(&m_groupSettingsMutex);

		QJsonObject products;
		if (!ReadGroupSettings(products, errorMessage, true)){
			return false;
		}

		for (auto it = products.constBegin(); it != products.constEnd(); ++it){
			QString pattern = it.value().toObject().value("pattern").toString();
			if (IsProductId(it.key().toUtf8()) && !pattern.isEmpty()){
				patterns.insert(it.key(), pattern);
			}
		}
	}

	// The directory is read without the settings lock, so a slow domain controller does not block sign-ins.
	QMap<QString, QMap<QByteArray, QString>> discoveredGroups;
	QStringList failedProducts;
	for (auto it = patterns.constBegin(); it != patterns.constEnd(); ++it){
		QMap<QByteArray, QString> groups;
		if (DiscoverDirectoryGroups(it.value(), groups)){
			discoveredGroups.insert(it.key(), groups);
		}
		else{
			failedProducts.append(it.key());
		}
	}

	QMutexLocker locker(&m_groupSettingsMutex);

	QLockFile fileLock(*m_groupSettingsFileAttrPtr + QStringLiteral(".lock"));
	if (!fileLock.tryLock(5000)){
		errorMessage = QStringLiteral("LDAP group settings are being updated; the directory group list was not synchronized");

		return false;
	}

	QJsonObject products;
	if (!ReadGroupSettings(products, errorMessage, true)){
		return false;
	}

	QJsonObject previousCatalogs;
	QString catalogError;
	if (!ReadGroupCatalog(previousCatalogs, catalogError)){
		// The catalog never grants access; an unreadable one is rebuilt from the directory.
		SendWarningMessage(0, catalogError, s_logSource);

		previousCatalogs = QJsonObject();
	}

	QJsonObject catalogs;
	QStringList synchronizedProducts;
	for (auto it = products.constBegin(); it != products.constEnd(); ++it){
		QString pattern = it.value().toObject().value("pattern").toString();
		if (!IsProductId(it.key().toUtf8()) || pattern.isEmpty()){
			continue;
		}

		if (discoveredGroups.contains(it.key()) && patterns.value(it.key()) == pattern){
			catalogs.insert(it.key(), CreateCatalogEntry(pattern, discoveredGroups.value(it.key())));
			synchronizedProducts.append(it.key());
		}
		else if (previousCatalogs.value(it.key()).toObject().value("pattern").toString() == pattern){
			// A failed read or a pattern changed meanwhile keeps the last complete group list.
			catalogs.insert(it.key(), previousCatalogs.value(it.key()));
		}
	}

	if (catalogs != previousCatalogs && !WriteGroupCatalog(catalogs, errorMessage)){
		return false;
	}

	MarkSynchronized(synchronizedProducts);

	if (!failedProducts.isEmpty()){
		errorMessage = QStringLiteral("Unable to read the AD security groups of product(s) %1; their last synchronized group list is kept").arg(failedProducts.join(QStringLiteral(", ")));

		return false;
	}

	return true;
}


void CLdapAuthorizationControllerComp::OnDirectoryChangeEvent(imtauth::CLdapGroupChangeListener::Event event, const QString& message)
{
	if (event == imtauth::CLdapGroupChangeListener::E_UNSUPPORTED){
		SetNotificationState(QStringLiteral("Unsupported"), message);
		SendErrorMessage(0, message, s_logSource);

		return;
	}

	if (event == imtauth::CLdapGroupChangeListener::E_DISCONNECTED){
		SetNotificationState(QStringLiteral("Disconnected"), message);
		SendWarningMessage(0, message, s_logSource);

		return;
	}

	if (event == imtauth::CLdapGroupChangeListener::E_CONNECTED){
		SetNotificationState(QStringLiteral("Connected"), QString());
	}

	QString errorMessage;
	if (SynchronizeDirectoryGroups(errorMessage)){
		SetSynchronizationError(QString());
	}
	else{
		SetSynchronizationError(errorMessage);
		SendErrorMessage(0, errorMessage, s_logSource);
	}
}


// reimplemented (icomp::CComponentBase)

void CLdapAuthorizationControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// The LDAP enable parameter may be restored after component creation, so it does not gate the listener.
	// Configuring the settings file opts into group mapping; the group list never grants access.
	bool isNotificationEnabled = m_groupChangeNotificationsAttrPtr.IsValid() && *m_groupChangeNotificationsAttrPtr;
	if (!IsGroupSettingsConfigured() || !isNotificationEnabled){
		SetNotificationState(QStringLiteral("Disabled"), QString());

		return;
	}

	SetNotificationState(QStringLiteral("Connecting"), QString());

	QString errorMessage;
	bool isStarted = m_groupChangeListener.Start(
				[this](imtauth::CLdapGroupChangeListener::Event event, const QString& message){
					OnDirectoryChangeEvent(event, message);
				},
				errorMessage);
	if (!isStarted){
		SetNotificationState(QStringLiteral("Unsupported"), errorMessage);
		SendErrorMessage(0, errorMessage, s_logSource);
	}
}


void CLdapAuthorizationControllerComp::OnComponentDestroyed()
{
	// The listener thread calls this component; it must have ended before any member is released.
	m_groupChangeListener.Stop();

	BaseClass::OnComponentDestroyed();
}


// protected methods

QByteArray CLdapAuthorizationControllerComp::CheckExistsRole(const QByteArray& productId, RoleType roleType, bool* isReadPtr) const
{
	if (isReadPtr != nullptr){ *isReadPtr = false; }
	if (m_roleCollectionCompPtr.IsValid()){
		bool complete = false;
		imtbase::IObjectCollection::Ids roleIds = imtauth::CollectionSelection::SelectIds(
			*m_roleCollectionCompPtr, imtauth::CollectionSelection::ColumnEquals("ProductId", productId), {"ProductId"}, &complete);
		if (!complete){ return QByteArray(); }
		QByteArray result;
		for (const QByteArray& roleId : roleIds){
			imtbase::IObjectCollection::DataPtr roleDataPtr;
			if (m_roleCollectionCompPtr->GetObjectData(roleId, roleDataPtr)){
				const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(roleDataPtr.GetPtr());
				if (roleInfoPtr != nullptr){
					QByteArray roleProductId = roleInfoPtr->GetProductId();
					if (roleProductId == productId && ((roleType == RT_DEFAULT && roleInfoPtr->IsDefault()) ||
						(roleType == RT_GUEST && roleInfoPtr->IsGuest()))){
						result = roleId;
					}
				}
				else{ return QByteArray(); }
			}
			else{ return QByteArray(); }
		}
		if (isReadPtr != nullptr){ *isReadPtr = true; }
		return result;
	}

	return QByteArray();
}


QByteArray CLdapAuthorizationControllerComp::EnsureRoleExists(
	const QByteArray& productId,
	RoleType roleType,
	const QByteArray& roleId,
	const QString& roleName,
	const QString& description) const
{
	// Guard the lookup and the insert against the role collection as one critical
	// section, so concurrent first-logins cannot both read a missing role and each
	// write a duplicate for the same product.
	QMutexLocker locker(&m_roleCollectionMutex);

	bool complete = false;
	QByteArray existingRoleId = CheckExistsRole(productId, roleType, &complete);
	if (!complete){
		return QByteArray();
	}
	if (!existingRoleId.isEmpty()){
		return existingRoleId;
	}

	return InsertNewIdentifiableRoleInfo(
		roleId,
		roleName,
		description,
		productId,
		roleType == RT_DEFAULT,
		roleType == RT_GUEST);
}


QByteArray CLdapAuthorizationControllerComp::InsertNewIdentifiableRoleInfo(
	const QByteArray& roleId,
	const QString& roleName,
	const QString& description,
	const QByteArray& productId,
	bool isDefault,
	bool isGuest) const
{
	istd::TDelPtr<imtauth::CIdentifiableRoleInfo> roleInfoPtr = new imtauth::CIdentifiableRoleInfo();
	if (!roleInfoPtr.IsValid()){
		return QByteArray();
	}

	QByteArray objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	roleInfoPtr->SetObjectUuid(objectId);
	roleInfoPtr->SetRoleId(roleId);
	roleInfoPtr->SetRoleName(roleName);
	roleInfoPtr->SetRoleDescription(description);
	roleInfoPtr->SetDefault(isDefault);
	roleInfoPtr->SetGuest(isGuest);
	roleInfoPtr->SetProductId(productId);

	QByteArray result = m_roleCollectionCompPtr->InsertNewObject(
		QByteArrayLiteral("Role"),
		roleInfoPtr->GetRoleName(),
		roleInfoPtr->GetRoleDescription(),
		roleInfoPtr.GetPtr(),
		objectId);

	if (result.isEmpty()){
		return QByteArray();
	}

	return objectId;
}


istd::TUniqueInterfacePtr<imtauth::IUserInfo> CLdapAuthorizationControllerComp::CreateUserInfoFromLdapUser(const QByteArray& ldapUserId) const
{
#ifdef Q_OS_WIN
	LPUSER_INFO_3 userInfo3BufPtr = NULL;

	QByteArray domain = ".";
	QByteArray username = ldapUserId;

	QByteArrayList data = ldapUserId.split('\\');
	if (data.size() >= 2){
		domain = data[0];
		username = data[1];
	}

	// Get the computer name of a DC for the specified domain.
	LPBYTE computerName;
	NetGetDCName(NULL, qUtf16Printable(domain), &computerName);

	NetUserGetInfo((LPWSTR)computerName, qUtf16Printable(ldapUserId), 3, (LPBYTE *)&userInfo3BufPtr);

	if (userInfo3BufPtr != nullptr){
		istd::TDelPtr<imtauth::CIdentifiableUserInfo> userInfoPtr;
		userInfoPtr.SetPtr(new imtauth::CIdentifiableUserInfo);

		userInfoPtr->SetObjectUuid(QUuid::createUuid().toByteArray(QUuid::WithoutBraces));

		imtauth::IUserInfo::SystemInfo systemInfo;
		systemInfo.systemId = *m_systemIdAttrPtr;
		systemInfo.systemName = "LDAP";

		userInfoPtr->AddToSystem(systemInfo);
		userInfoPtr->SetId(ldapUserId);

		QByteArray sid = GetDirectoryUserSid(ldapUserId);
		userInfoPtr->SetSid(sid);

		QByteArray password = QString::fromWCharArray(userInfo3BufPtr->usri3_password).toUtf8();
		userInfoPtr->SetPasswordHash(password);

		QString name = QString::fromWCharArray(userInfo3BufPtr->usri3_full_name);
		if (!name.isEmpty()){
			userInfoPtr->SetName(name);
		}
		else{
			userInfoPtr->SetName(ldapUserId);
		}

		QString description = QString::fromWCharArray(userInfo3BufPtr->usri3_comment);
		userInfoPtr->SetDescription(description);

		QString email = imtauth::CLdapUserCollectionControllerComp::GetEmailForUser(ldapUserId);
		if (!email.isEmpty()){
			userInfoPtr->SetMail(email);
		}

		NetApiBufferFree(userInfo3BufPtr);

		return userInfoPtr.PopPtr();
	}

#else
	Q_UNUSED(ldapUserId)
#endif

	return nullptr;
}


QByteArray CLdapAuthorizationControllerComp::GetUserObjectIdBySid(const QByteArray& sid, bool* isReadPtr) const
{
	if (isReadPtr != nullptr){ *isReadPtr = true; }
	if (sid.isEmpty()){
		return QByteArray();
	}

	if (!m_userCollectionCompPtr.IsValid()){
		if (isReadPtr != nullptr){ *isReadPtr = false; }
		return QByteArray();
	}
	bool complete = false;
	imtbase::IObjectCollection::Ids userIds = imtauth::CollectionSelection::SelectIds(
		*m_userCollectionCompPtr, imtauth::CollectionSelection::ColumnEquals("Sid", sid), {"Sid"}, &complete);
	if (!complete){
		if (isReadPtr != nullptr){ *isReadPtr = false; }
		return QByteArray();
	}
	QByteArray result;
	for (const QByteArray& userId : userIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
			const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
			if (userInfoPtr == nullptr){
				if (isReadPtr != nullptr){ *isReadPtr = false; }
				return QByteArray();
			}
			if (userInfoPtr->GetSid() == sid){
				if (!result.isEmpty()){
					if (isReadPtr != nullptr){ *isReadPtr = false; }
					return QByteArray();
				}
				result = userId;
			}
		}
		else{
			if (isReadPtr != nullptr){ *isReadPtr = false; }
			return QByteArray();
		}
	}

	return result;
}


sdl::V1_0::imtauth::CAuthorizationPayload CLdapAuthorizationControllerComp::OnUserToken(
	const sdl::V1_0::imtauth::CUserTokenGqlRequest& userTokenRequest,
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (m_enableableParamCompPtr.IsValid() && m_enableableParamCompPtr->IsEnabled()){
		sdl::V1_0::imtauth::CAuthorizationGqlRequest authorizationRequest(gqlRequest, false);
		if (!authorizationRequest.IsValid()){
			errorMessage = QStringLiteral("Invalid LDAP authorization request");
			return sdl::V1_0::imtauth::CAuthorizationPayload();
		}
		return OnAuthorization(authorizationRequest, gqlRequest, errorMessage);
	}
	return BaseClass::OnUserToken(userTokenRequest, gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CAuthorizationPayload CLdapAuthorizationControllerComp::OnAuthorization(
			const sdl::V1_0::imtauth::CAuthorizationGqlRequest& authorizationRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (m_enableableParamCompPtr.IsValid()){
		bool enabled = m_enableableParamCompPtr->IsEnabled();
		if (enabled){
			if (!m_userCollectionCompPtr.IsValid() || !m_roleCollectionCompPtr.IsValid()){
				errorMessage = QStringLiteral("LDAP user and role collections must be configured");
				return sdl::V1_0::imtauth::CAuthorizationPayload();
			}

			sdl::V1_0::imtauth::AuthorizationRequestArguments arguments = authorizationRequest.GetRequestedArguments();
			if (!arguments.input.has_value()){
				Q_ASSERT(false);
				return sdl::V1_0::imtauth::CAuthorizationPayload();
			}

			sdl::V1_0::imtauth::CAuthorizationInput inputArgument = *arguments.input;

			QByteArray login;
			if (inputArgument.login){
				login = inputArgument.login->toUtf8();
			}

			QByteArray productId;
			if (inputArgument.productId){
				productId = *inputArgument.productId;
			}

			QByteArray password;
			if (inputArgument.password){
				password = inputArgument.password->toUtf8();
			}

			qint64 remainingLockoutSecs = 0;
			if (IsAccountLocked(login, &remainingLockoutSecs)){
				return CreateAccountLockedResponse(login, remainingLockoutSecs, errorMessage);
			}

			bool complete = false;
			QByteArray userObjectId = GetUserObjectId(login, &complete);
			if (!complete){
				errorMessage = QStringLiteral("Unable to select LDAP user");
				return sdl::V1_0::imtauth::CAuthorizationPayload();
			}

			// If user not found by login name, try to find by SID
			// This handles the case where user logged in with a different login format
			if (userObjectId.isEmpty()){
				QByteArray sid = GetDirectoryUserSid(login);
				if (!sid.isEmpty()){
					userObjectId = GetUserObjectIdBySid(sid, &complete);
					if (!complete){
						errorMessage = QStringLiteral("Unable to select LDAP user by SID");
						return sdl::V1_0::imtauth::CAuthorizationPayload();
					}
				}
			}

			bool ok = CheckCredential(*m_systemIdAttrPtr, login, password);
			if (ok){
				RegisterAccessAttempt(login, true);
				QByteArray guestRoleId = EnsureRoleExists(productId, RT_GUEST, "Guest", "Guest", "Guest role");

				QByteArray defaultRoleId = EnsureRoleExists(productId, RT_DEFAULT, "Default", "Default", "Default role");
				if (guestRoleId.isEmpty() || defaultRoleId.isEmpty()){
					errorMessage = QStringLiteral("Unable to ensure LDAP default and guest roles");
					return sdl::V1_0::imtauth::CAuthorizationPayload();
				}

				istd::TUniqueInterfacePtr<imtauth::CIdentifiableUserInfo> userInfoPtr;
				if (userObjectId.isEmpty()){
					userInfoPtr.MoveCastedPtr(CreateUserInfoFromLdapUser(login));
					if (!userInfoPtr.IsValid()){
						userInfoPtr.SetPtr(new imtauth::CIdentifiableUserInfo);
						userInfoPtr->SetObjectUuid(QUuid::createUuid().toByteArray(QUuid::WithoutBraces));
						userInfoPtr->SetId(login);
						userInfoPtr->SetName(login);

						QByteArray sid = GetDirectoryUserSid(login);
						userInfoPtr->SetSid(sid);

						QString mail = imtauth::CLdapUserCollectionControllerComp::GetEmailForUser(login);
						userInfoPtr->SetMail(mail);
					}

					userObjectId = userInfoPtr->GetObjectUuid();

					imtauth::IUserInfo::SystemInfo systemInfo;
					systemInfo.systemId = *m_systemIdAttrPtr;
					systemInfo.systemName = "LDAP";
					userInfoPtr->AddToSystem(systemInfo);

					userInfoPtr->AddRole(productId, defaultRoleId);

					QByteArray retVal = m_userCollectionCompPtr->InsertNewObject("User", userInfoPtr->GetName(), "", userInfoPtr.GetPtr(), userObjectId);
					if (retVal.isEmpty()){
						errorMessage = QStringLiteral("Unable to insert LDAP user to the collection");
						return sdl::V1_0::imtauth::CAuthorizationPayload();
					}
				}
				else{
					QMutexLocker userUpdateLocker(&imtauth::CUserBaseInfo::GetUserUpdateMutex(userObjectId));
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
						userInfoPtr.MoveCastedPtr(dataPtr.GetPtr()->CloneMe());
						if (userInfoPtr.IsValid()){
							// Reached only after CheckCredential succeeded, so naming the account state is safe.
							if (!userInfoPtr->IsEnabled()){
								return CreateAccountDisabledResponse(login);
							}

							bool needsUpdate = false;
							bool hasLdapSystem = false;
							for (const imtauth::IUserInfo::SystemInfo& system : userInfoPtr->GetSystemInfos()){
								if (system.systemId == *m_systemIdAttrPtr){
									if (!system.enabled){
										userUpdateLocker.unlock();

										return BaseClass::OnAuthorization(authorizationRequest, gqlRequest, errorMessage);
									}
									hasLdapSystem = true;
								}
							}
							if (!hasLdapSystem){
								imtauth::IUserInfo::SystemInfo system;
								system.systemId = *m_systemIdAttrPtr;
								system.systemName = "LDAP";
								userInfoPtr->AddToSystem(system);
								needsUpdate = true;
							}

							// Update login in case the user logged in with a different format
							if (userInfoPtr->GetId() != login){
								userInfoPtr->SetId(login);
								needsUpdate = true;
							}

							// Ensure SID is set (may be empty for users created before SID support)
							QByteArray sid = GetDirectoryUserSid(login);
							if (!userInfoPtr->GetSid().isEmpty() && !sid.isEmpty() && userInfoPtr->GetSid() != sid){
								errorMessage = QStringLiteral("LDAP account identity does not match the stored user");
								return sdl::V1_0::imtauth::CAuthorizationPayload();
							}
							if (userInfoPtr->GetSid().isEmpty()){
								userInfoPtr->SetSid(sid);
								needsUpdate = true;
							}

							// Ensure email is set (may be empty for users created before email lookup)
							if (userInfoPtr->GetMail().isEmpty()){
								QString email = imtauth::CLdapUserCollectionControllerComp::GetEmailForUser(login);
								if (!email.isEmpty()){
									userInfoPtr->SetMail(email);
									needsUpdate = true;
								}
							}

							QByteArrayList products = userInfoPtr->GetProducts();
							if (!products.contains(productId)){
								userInfoPtr->AddRole(productId, defaultRoleId);
								needsUpdate = true;
							}

							if (needsUpdate){
								imtbase::IOperationContext::IdentifableObjectInfo ownerInfo;
								ownerInfo.id = userObjectId;
								ownerInfo.name = userInfoPtr->GetName();
								imtbase::COperationContext operationContext;
								operationContext.SetOperationOwnerId(ownerInfo);

								if (!m_userCollectionCompPtr->SetObjectData(userObjectId, *userInfoPtr.GetPtr(), istd::IChangeable::CM_WITHOUT_REFS, &operationContext)){
									errorMessage = QStringLiteral("Unable to update LDAP user data");
									return sdl::V1_0::imtauth::CAuthorizationPayload();
								}
							}
						}
						userUpdateLocker.unlock();
					}
				}

				if (userInfoPtr.IsValid()){
					userInfoPtr->SetId(login);
					sdl::V1_0::imtauth::CAuthorizationPayload retVal;
					retVal = CreateAuthorizationSuccessfulResponse(*userInfoPtr.GetPtr(), *m_systemIdAttrPtr, productId, errorMessage);

					return retVal;
				}

				sdl::V1_0::imtauth::CAuthorizationPayload retVal;
				retVal = CreateInvalidLoginOrPasswordResponse(login, errorMessage);

				return retVal;
			}
		}
	}

	return BaseClass::OnAuthorization(authorizationRequest, gqlRequest, errorMessage);
}


// private methods

bool CLdapAuthorizationControllerComp::IsGroupSettingsConfigured() const
{
	return m_groupSettingsFileAttrPtr.IsValid() && !m_groupSettingsFileAttrPtr->isEmpty();
}


QString CLdapAuthorizationControllerComp::GetGroupCatalogFilePath() const
{
	return *m_groupSettingsFileAttrPtr + QStringLiteral(".groups.json");
}


bool CLdapAuthorizationControllerComp::ReadGroupCatalog(QJsonObject& catalogs, QString& errorMessage) const
{
	catalogs = QJsonObject();

	QFile file(GetGroupCatalogFilePath());
	if (!file.exists()){
		return true;
	}

	if (!file.open(QIODevice::ReadOnly) || file.size() > s_maximumCatalogSize){
		errorMessage = QStringLiteral("Unable to read the directory group list");

		return false;
	}

	QJsonParseError parseError;
	QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
	if (file.error() != QFileDevice::NoError || parseError.error != QJsonParseError::NoError || !document.isObject() ||
		document.object().value("version").toInt() != 1 || !document.object().value("products").isObject()){
		errorMessage = QStringLiteral("Invalid directory group list file");

		return false;
	}

	catalogs = document.object().value("products").toObject();

	return true;
}


bool CLdapAuthorizationControllerComp::WriteGroupCatalog(const QJsonObject& catalogs, QString& errorMessage) const
{
	QByteArray data = QJsonDocument(QJsonObject{{"version", 1}, {"products", catalogs}}).toJson();
	if (data.size() > s_maximumCatalogSize){
		errorMessage = QStringLiteral("The directory group list is too large");

		return false;
	}

	QSaveFile file(GetGroupCatalogFilePath());
	if (!file.open(QIODevice::WriteOnly) ||
		!file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner) ||
		file.write(data) != data.size() ||
		!file.commit()){
		errorMessage = QStringLiteral("Unable to persist the directory group list");

		return false;
	}

	return true;
}


void CLdapAuthorizationControllerComp::SetNotificationState(const QString& state, const QString& message)
{
	QMutexLocker locker(&m_directoryStatusMutex);

	m_notificationState = state;
	m_notificationMessage = message;
}


void CLdapAuthorizationControllerComp::MarkSynchronized(const QStringList& products) const
{
	QMutexLocker locker(&m_directoryStatusMutex);

	const QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
	for (const QString& product : products){
		m_synchronizedAt.insert(product, now);
	}
}


void CLdapAuthorizationControllerComp::SetSynchronizationError(const QString& errorMessage) const
{
	QMutexLocker locker(&m_directoryStatusMutex);

	m_synchronizationError = errorMessage;
}


} // namespace imtauthgql

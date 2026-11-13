// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QSet>
#include <QtCore/QTimeZone>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>
#include <iqt/ITranslationManager.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtgql/IGqlContext.h>
#include <imtnotify/INotification.h>
#include <imtnotify/INotificationSender.h>
#include <imtnotify/INotificationTypeList.h>
#include <imtnotify/NotificationMessageFormat.h>
#include <imtnotify/NotificationSelection.h>

// Generated includes
#include <GeneratedFiles/imtnotifysdl/SDL/1.0/CPP/Notifications.h>


/**
	\file imtnotifygql.h
	\brief Main header file for the imtnotifygql module - GraphQL API of the notification center
*/

/**
	\namespace imtnotifygql
	\brief GraphQL API and WebSocket push of the notification center

	- \b CNotificationsControllerComp — queries and mutations of the inbox of the caller
	- \b CNotificationPublisherComp — the OnNotificationEvent subscription
*/
namespace imtnotifygql
{


/**
	The inbox of the caller. It is invalid for a request without a signed-in user.
*/
inline imtnotify::NotificationScope GetScope(const imtgql::IGqlContext* contextPtr)
{
	imtnotify::NotificationScope scope;
	if (contextPtr != nullptr){
		scope.productId = contextPtr->GetProductId();
		scope.tenantId = contextPtr->GetTenantId();
		scope.userId = contextPtr->GetUserId();
	}

	return scope;
}


/**
	The language of the client of a request: the product server forwards it in a header.
*/
inline QByteArray GetLanguageId(const imtgql::IGqlContext* contextPtr)
{
	return contextPtr != nullptr ? contextPtr->GetLanguageId() : QByteArray();
}


inline sdl::V1_0::imtnotify::NotificationSeverity GetSdlSeverity(istd::IInformationProvider::InformationCategory category)
{
	switch (category){
	case istd::IInformationProvider::IC_WARNING:
		return sdl::V1_0::imtnotify::NotificationSeverity::Warning;
	case istd::IInformationProvider::IC_ERROR:
		return sdl::V1_0::imtnotify::NotificationSeverity::Error;
	case istd::IInformationProvider::IC_CRITICAL:
		return sdl::V1_0::imtnotify::NotificationSeverity::Critical;
	default:
		return sdl::V1_0::imtnotify::NotificationSeverity::Info;
	}
}


inline istd::IInformationProvider::InformationCategory GetCategory(sdl::V1_0::imtnotify::NotificationSeverity severity)
{
	switch (severity){
	case sdl::V1_0::imtnotify::NotificationSeverity::Warning:
		return istd::IInformationProvider::IC_WARNING;
	case sdl::V1_0::imtnotify::NotificationSeverity::Error:
		return istd::IInformationProvider::IC_ERROR;
	case sdl::V1_0::imtnotify::NotificationSeverity::Critical:
		return istd::IInformationProvider::IC_CRITICAL;
	default:
		return istd::IInformationProvider::IC_INFO;
	}
}


inline istd::TNullableValue<sdl::V1_0::imtbase::CObjectLink> GetSdlObjectLink(const imtnotify::ObjectLinkInfo& link)
{
	istd::TNullableValue<sdl::V1_0::imtbase::CObjectLink> retVal;
	if (!link.IsEmpty()){
		sdl::V1_0::imtbase::CObjectLink sdlLink;
		sdlLink.id = link.id;
		sdlLink.typeId = link.typeId;
		sdlLink.name = link.name;

		retVal = sdlLink;
	}

	return retVal;
}


inline imtnotify::ObjectLinkInfo GetObjectLinkInfo(const istd::TNullableValue<sdl::V1_0::imtbase::CObjectLink>& sdlLink)
{
	imtnotify::ObjectLinkInfo retVal;
	if (sdlLink){
		retVal.id = sdlLink->id ? *sdlLink->id : QByteArray();
		retVal.typeId = sdlLink->typeId ? *sdlLink->typeId : QByteArray();
		retVal.name = sdlLink->name ? *sdlLink->name : QString();
	}

	return retVal;
}


inline sdl::V1_0::imtnotify::NotificationArgumentKind GetSdlArgumentKind(imtnotify::NotificationArgument::Kind kind)
{
	switch (kind){
	case imtnotify::NotificationArgument::K_NUMBER:
		return sdl::V1_0::imtnotify::NotificationArgumentKind::Number;
	case imtnotify::NotificationArgument::K_DATE_TIME:
		return sdl::V1_0::imtnotify::NotificationArgumentKind::DateTime;
	case imtnotify::NotificationArgument::K_OBJECT:
		return sdl::V1_0::imtnotify::NotificationArgumentKind::Object;
	default:
		return sdl::V1_0::imtnotify::NotificationArgumentKind::Text;
	}
}


inline imtnotify::NotificationArgument::Kind GetArgumentKind(sdl::V1_0::imtnotify::NotificationArgumentKind kind)
{
	switch (kind){
	case sdl::V1_0::imtnotify::NotificationArgumentKind::Number:
		return imtnotify::NotificationArgument::K_NUMBER;
	case sdl::V1_0::imtnotify::NotificationArgumentKind::DateTime:
		return imtnotify::NotificationArgument::K_DATE_TIME;
	case sdl::V1_0::imtnotify::NotificationArgumentKind::Object:
		return imtnotify::NotificationArgument::K_OBJECT;
	default:
		return imtnotify::NotificationArgument::K_TEXT;
	}
}


inline imtsdl::TElementList<sdl::V1_0::imtnotify::CNotificationArgument> GetSdlArguments(const imtnotify::NotificationArguments& arguments)
{
	imtsdl::TElementList<sdl::V1_0::imtnotify::CNotificationArgument> retVal;
	for (const imtnotify::NotificationArgument& argument : arguments){
		sdl::V1_0::imtnotify::CNotificationArgument sdlArgument;
		sdlArgument.id = argument.id;
		sdlArgument.kind = GetSdlArgumentKind(argument.kind);
		sdlArgument.value = argument.value;
		if (argument.kind == imtnotify::NotificationArgument::K_OBJECT){
			sdlArgument.objectId = argument.objectId;
			sdlArgument.objectTypeId = argument.objectTypeId;
		}

		retVal.append(sdlArgument);
	}

	return retVal;
}


inline imtnotify::NotificationArguments GetArguments(const istd::TNullableValue<imtsdl::TElementList<sdl::V1_0::imtnotify::CNotificationArgument>>& sdlArguments)
{
	imtnotify::NotificationArguments retVal;
	if (sdlArguments){
		for (const istd::TNullableValue<sdl::V1_0::imtnotify::CNotificationArgument>& sdlArgument : *sdlArguments){
			if (!sdlArgument){
				continue;
			}

			imtnotify::NotificationArgument argument;
			argument.id = sdlArgument->id ? *sdlArgument->id : QByteArray();
			argument.kind = sdlArgument->kind ? GetArgumentKind(*sdlArgument->kind) : imtnotify::NotificationArgument::K_TEXT;
			argument.value = sdlArgument->value ? *sdlArgument->value : QString();
			argument.objectId = sdlArgument->objectId ? *sdlArgument->objectId : QByteArray();
			argument.objectTypeId = sdlArgument->objectTypeId ? *sdlArgument->objectTypeId : QByteArray();
			retVal.append(argument);
		}
	}

	return retVal;
}


/**
	The input of SendNotification for a message.
*/
inline void CreateSendInput(
			const imtnotify::INotificationSender::Audience& audience,
			const imtnotify::INotificationSender::Message& message,
			sdl::V1_0::imtnotify::CSendNotificationInput& input)
{
	input.productId = audience.productId;
	input.tenantId = audience.tenantId;

	imtsdl::TElementList<QByteArray> userIds;
	userIds.FromList(audience.userIds);
	input.userIds = userIds;

	input.typeId = message.typeId;
	input.severity = GetSdlSeverity(message.severity);
	input.actor = GetSdlObjectLink(message.actor);
	input.subject = GetSdlObjectLink(message.subject);
	input.body = message.body;
	if (!message.arguments.isEmpty()){
		input.arguments = GetSdlArguments(message.arguments);
	}

	input.groupKey = message.groupKey;
	input.dedupKey = message.dedupKey;
}


/**
	The audience and the message of a SendNotification input.
*/
inline void ReadSendInput(
			const sdl::V1_0::imtnotify::CSendNotificationInput& input,
			imtnotify::INotificationSender::Audience& audience,
			imtnotify::INotificationSender::Message& message)
{
	audience.productId = input.productId ? *input.productId : QByteArray();
	audience.tenantId = input.tenantId ? *input.tenantId : QByteArray();
	audience.userIds.clear();
	if (input.userIds){
		for (const istd::TNullableValue<QByteArray>& userId : *input.userIds){
			if (userId && !userId->isEmpty()){
				audience.userIds.append(*userId);
			}
		}
	}

	message.typeId = input.typeId ? *input.typeId : QByteArray();
	message.severity = input.severity ? GetCategory(*input.severity) : istd::IInformationProvider::IC_INFO;
	message.actor = GetObjectLinkInfo(input.actor);
	message.subject = GetObjectLinkInfo(input.subject);
	message.body = input.body ? *input.body : QString();
	message.arguments = GetArguments(input.arguments);
	message.groupKey = input.groupKey ? *input.groupKey : QByteArray();
	message.dedupKey = input.dedupKey ? *input.dedupKey : QByteArray();
}


inline sdl::V1_0::imtnotify::CUserNotification GetSdlNotification(const QByteArray& id, const imtnotify::INotification& notification)
{
	sdl::V1_0::imtnotify::CUserNotification retVal;
	retVal.id = id;
	retVal.typeId = notification.GetTypeId();
	retVal.severity = GetSdlSeverity(notification.GetInformationCategory());
	if (!notification.GetActors().isEmpty()){
		imtsdl::TElementList<sdl::V1_0::imtbase::CObjectLink> actors;
		for (const imtnotify::ObjectLinkInfo& actor : notification.GetActors()){
			const istd::TNullableValue<sdl::V1_0::imtbase::CObjectLink> sdlActor = GetSdlObjectLink(actor);
			if (sdlActor){
				actors.append(*sdlActor);
			}
		}

		retVal.actors = actors;
	}

	retVal.subject = GetSdlObjectLink(notification.GetSubject());
	if (!notification.GetArguments().isEmpty()){
		retVal.arguments = GetSdlArguments(notification.GetArguments());
	}

	retVal.body = notification.GetInformationDescription();
	retVal.groupCount = notification.GetGroupCount();
	retVal.createdAt = notification.GetInformationTimeStamp().toUTC().toString(Qt::ISODateWithMs);
	retVal.isDelivered = notification.IsDelivered();
	retVal.isRead = notification.IsRead();

	return retVal;
}


/**
	A text of an .acc file in the language of the client: the texts of the attributes are translated in the context
	"Attribute".
*/
inline QString TranslateAttributeText(const QString& text, const iqt::ITranslationManager* translationManagerPtr, const QByteArray& languageId)
{
	if (translationManagerPtr == nullptr || text.isEmpty()){
		return text;
	}

	return QString::fromUtf8(iqt::GetTranslation(translationManagerPtr, text.toUtf8(), languageId, QByteArrayLiteral("Attribute")));
}


/**
	The message of a type in the language of the client. A translation that is not valid or uses other values than the
	message is replaced by the message, and the reason is added to the errors.
*/
inline QString TranslateMessage(
			const imtnotify::INotificationTypeInfo& typeInfo,
			const iqt::ITranslationManager* translationManagerPtr,
			const QByteArray& languageId,
			QStringList& errors)
{
	const QString message = typeInfo.GetMessage();
	const QString translation = TranslateAttributeText(message, translationManagerPtr, languageId);
	if (translation == message){
		return message;
	}

	QSet<QByteArray> messageIds;
	QSet<QByteArray> translationIds;
	QString errorMessage;
	if (		imtnotify::NotificationMessageFormat::GetArgumentIds(message, messageIds, errorMessage) &&
				imtnotify::NotificationMessageFormat::GetArgumentIds(translation, translationIds, errorMessage) &&
				translationIds == messageIds){
		return translation;
	}

	if (errorMessage.isEmpty()){
		errorMessage = QStringLiteral("other values than the message");
	}

	errors.append(QStringLiteral("The '%1' translation of the message of notification type '%2' is not used: %3")
				.arg(QString::fromUtf8(languageId), QString::fromUtf8(typeInfo.GetTypeId()), errorMessage));

	return message;
}


/**
	An entry of the catalog: the id, the label and the message in the language of the client and the icon.
*/
inline sdl::V1_0::imtnotify::CNotificationTypeInfo CreateTypeInfo(
			const imtnotify::INotificationTypeInfo& typeInfo,
			const iqt::ITranslationManager* translationManagerPtr,
			const QByteArray& languageId,
			QStringList& errors)
{
	sdl::V1_0::imtnotify::CNotificationTypeInfo retVal;
	retVal.id = typeInfo.GetTypeId();
	retVal.name = TranslateAttributeText(typeInfo.GetName(), translationManagerPtr, languageId);
	retVal.icon = typeInfo.GetIconPath();
	retVal.message = TranslateMessage(typeInfo, translationManagerPtr, languageId, errors);

	return retVal;
}


/**
	Messages a component reports once, e.g. the errors of a catalog that is built for every request.
*/
class CReportedMessages
{
public:
	/**
		False if the message was already reported.
	*/
	bool Insert(const QString& message)
	{
		QMutexLocker locker(&m_mutex);

		if (m_messages.contains(message)){
			return false;
		}

		m_messages.insert(message);

		return true;
	}

private:
	QMutex m_mutex;
	QSet<QString> m_messages;
};


/**
	The type lists of a multi reference to imtnotify::INotificationTypeList.
*/
template <class TypeListsRef>
inline QList<const imtnotify::INotificationTypeList*> GetTypeLists(const TypeListsRef& typeListsCompPtr)
{
	QList<const imtnotify::INotificationTypeList*> retVal;
	for (int i = 0; i < typeListsCompPtr.GetCount(); ++i){
		if (typeListsCompPtr[i] != nullptr){
			retVal.append(typeListsCompPtr[i]);
		}
	}

	return retVal;
}


/**
	The catalog a server knows: the notification types and the object types of the modules it runs.
	A type in several lists keeps its first entry.
*/
inline sdl::V1_0::imtnotify::CNotificationTypesPayload CreateTypeCatalog(
			const QList<const imtnotify::INotificationTypeList*>& typeLists,
			const iqt::ITranslationManager* translationManagerPtr,
			const QByteArray& languageId,
			QStringList& errors)
{
	imtsdl::TElementList<sdl::V1_0::imtnotify::CNotificationTypeInfo> types;
	imtsdl::TElementList<sdl::V1_0::imtnotify::CNotificationTypeInfo> objectTypes;
	QSet<QByteArray> typeIds;
	QSet<QByteArray> objectTypeIds;

	for (const imtnotify::INotificationTypeList* typeListPtr : typeLists){
		for (int i = 0; i < typeListPtr->GetTypesCount(); ++i){
			const imtnotify::INotificationTypeInfo* typeInfoPtr = typeListPtr->GetTypeInfo(i);
			if (typeInfoPtr != nullptr && !typeInfoPtr->GetTypeId().isEmpty() && !typeIds.contains(typeInfoPtr->GetTypeId())){
				typeIds.insert(typeInfoPtr->GetTypeId());
				types.append(CreateTypeInfo(*typeInfoPtr, translationManagerPtr, languageId, errors));
			}
		}

		for (int i = 0; i < typeListPtr->GetObjectTypesCount(); ++i){
			const imtnotify::INotificationTypeInfo* typeInfoPtr = typeListPtr->GetObjectTypeInfo(i);
			if (typeInfoPtr != nullptr && !typeInfoPtr->GetTypeId().isEmpty() && !objectTypeIds.contains(typeInfoPtr->GetTypeId())){
				objectTypeIds.insert(typeInfoPtr->GetTypeId());
				objectTypes.append(CreateTypeInfo(*typeInfoPtr, translationManagerPtr, languageId, errors));
			}
		}
	}

	sdl::V1_0::imtnotify::CNotificationTypesPayload retVal;
	retVal.types = types;
	retVal.objectTypes = objectTypes;
	retVal.languageId = languageId;

	return retVal;
}


/**
	A page of the notifications of an inbox, newest first, with the number of all of them.
*/
inline bool ReadNotifications(
			const imtbase::IObjectCollection& collection,
			const imtnotify::NotificationScope& scope,
			const imtnotify::FieldFilters& fieldFilters,
			int offset,
			int count,
			imtsdl::TElementList<sdl::V1_0::imtnotify::CUserNotification>& notifications,
			int* totalCountPtr = nullptr)
{
	iprm::CParamsSet params;
	imtnotify::SetInboxSelection(scope, fieldFilters, params);

	istd::TDelPtr<imtbase::IObjectCollectionIterator> iteratorPtr(collection.CreateObjectCollectionIterator(QByteArray(), offset, count, &params));
	if (!iteratorPtr.IsValid()){
		return false;
	}

	while (iteratorPtr->Next()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const imtnotify::INotification* notificationPtr = nullptr;
		if (iteratorPtr->GetObjectData(dataPtr)){
			notificationPtr = dynamic_cast<const imtnotify::INotification*>(dataPtr.GetPtr());
		}

		if (notificationPtr == nullptr){
			return false;
		}

		notifications.append(GetSdlNotification(iteratorPtr->GetObjectId(), *notificationPtr));
	}

	if (totalCountPtr != nullptr){
		*totalCountPtr = iteratorPtr->GetElementsCount();
	}

	return true;
}


/**
	The time until which a "mark all" request applies; a time without a zone is UTC. Invalid for an empty text,
	false for a text that is not an ISO 8601 time.
*/
inline bool ParseCreatedUntil(const istd::TNullableValue<QString>& text, QDateTime& createdUntil)
{
	createdUntil = QDateTime();
	if (!text || text->isEmpty()){
		return true;
	}

	createdUntil = QDateTime::fromString(*text, Qt::ISODateWithMs);
	if (createdUntil.isValid() && createdUntil.timeSpec() == Qt::LocalTime){
		createdUntil.setTimeZone(QTimeZone::UTC);
	}

	return createdUntil.isValid();
}


/**
	Number of the notifications of an inbox matching the filters; negative if it could not be read.
*/
inline int CountNotifications(const imtbase::IObjectCollection& collection, const imtnotify::NotificationScope& scope, const imtnotify::FieldFilters& fieldFilters)
{
	iprm::CParamsSet params;
	imtnotify::SetInboxSelection(scope, fieldFilters, params);

	return collection.GetElementsCount(&params);
}


/**
	Number of the unread notifications of an inbox; negative if it could not be read.
*/
inline int GetUnreadCount(const imtbase::IObjectCollection& collection, const imtnotify::NotificationScope& scope)
{
	return CountNotifications(collection, scope, {imtbase::IComplexCollectionFilter::FieldFilter(imtnotify::NotificationField::s_isRead, false)});
}


} // namespace imtnotifygql



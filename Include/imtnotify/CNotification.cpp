// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotify/CNotification.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>
#include <istd/CChangeNotifier.h>
#include <istd/CClassInfo.h>


namespace imtnotify
{


// public methods

CNotification::CNotification()
	:m_groupCount(1),
	m_isDelivered(false),
	m_isRead(false)
{
	m_category = istd::IInformationProvider::IC_INFO;
}


QByteArray CNotification::GetMessageTypeId()
{
	return istd::CClassInfo::GetName<CNotification>();
}


// reimplemented (imtnotify::INotification)

void CNotification::SetCategory(InformationCategory category)
{
	BaseClass::SetCategory(category);
}


void CNotification::SetDescription(const QString& description)
{
	BaseClass::SetText(description);
}


void CNotification::SetTimeStamp(const QDateTime& timeStamp)
{
	m_timeStamp = timeStamp;
}


const NotificationScope& CNotification::GetScope() const
{
	return m_scope;
}


void CNotification::SetScope(const NotificationScope& scope)
{
	m_scope = scope;
}


const QByteArray& CNotification::GetTypeId() const
{
	return m_typeId;
}


void CNotification::SetTypeId(const QByteArray& typeId)
{
	m_typeId = typeId;
}


const ObjectLinkInfo& CNotification::GetSubject() const
{
	return m_subject;
}


void CNotification::SetSubject(const ObjectLinkInfo& subject)
{
	m_subject = subject;
}


const ObjectLinkInfos& CNotification::GetActors() const
{
	return m_actors;
}


void CNotification::SetActors(const ObjectLinkInfos& actors)
{
	m_actors = actors.mid(0, s_maxActorCount);

	BaseClass::SetSource(m_actors.isEmpty() ? QString() : m_actors.first().name);
}


const NotificationArguments& CNotification::GetArguments() const
{
	return m_arguments;
}


void CNotification::SetArguments(const NotificationArguments& arguments)
{
	m_arguments = arguments;
}


const QByteArray& CNotification::GetGroupKey() const
{
	return m_groupKey;
}


void CNotification::SetGroupKey(const QByteArray& groupKey)
{
	m_groupKey = groupKey;
}


int CNotification::GetGroupCount() const
{
	return m_groupCount;
}


void CNotification::SetGroupCount(int groupCount)
{
	m_groupCount = groupCount;
}


bool CNotification::IsDelivered() const
{
	return m_isDelivered;
}


void CNotification::SetDelivered(bool isDelivered)
{
	m_isDelivered = isDelivered;
}


bool CNotification::IsRead() const
{
	return m_isRead;
}


void CNotification::SetRead(bool isRead)
{
	m_isRead = isRead;
}


// reimplemented (iser::IObject)

QByteArray CNotification::GetFactoryId() const
{
	return GetMessageTypeId();
}


// reimplemented (iser::ISerializable)

bool CNotification::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag productIdTag("ProductId", "Product of the inbox", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag tenantIdTag("TenantId", "Tenant of the inbox", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag userIdTag("UserId", "Owner of the inbox", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag typeIdTag("TypeId", "Event type", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag subjectIdTag("SubjectId", "ID of the subject", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag subjectTypeIdTag("SubjectTypeId", "Type of the subject", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag subjectNameTag("SubjectName", "Name of the subject", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag groupKeyTag("GroupKey", "Group key", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag groupCountTag("GroupCount", "Number of merged events", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag isDeliveredTag("IsDelivered", "Delivered to a session", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag isReadTag("IsRead", "Read by the user", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag actorsTag("Actors", "Who caused the latest events", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag actorTag("Actor", "Who caused an event", iser::CArchiveTag::TT_GROUP, &actorsTag);
	static iser::CArchiveTag argumentsTag("Arguments", "Values of the message", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag argumentTag("Argument", "Value of the message", iser::CArchiveTag::TT_GROUP, &argumentsTag);
	static iser::CArchiveTag idTag("Id", "ID", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag objectTypeIdTag("TypeId", "Type of the object", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag nameTag("Name", "Name of the object", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag kindTag("Kind", "Kind of the value", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag valueTag("Value", "The value", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag objectIdTag("ObjectId", "ID of the object", iser::CArchiveTag::TT_LEAF);

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this, &GetAllChanges());
	Q_UNUSED(notifier);

	bool retVal = BaseClass::Serialize(archive);

	retVal = retVal && archive.BeginTag(productIdTag) && archive.Process(m_scope.productId) && archive.EndTag(productIdTag);
	retVal = retVal && archive.BeginTag(tenantIdTag) && archive.Process(m_scope.tenantId) && archive.EndTag(tenantIdTag);
	retVal = retVal && archive.BeginTag(userIdTag) && archive.Process(m_scope.userId) && archive.EndTag(userIdTag);
	retVal = retVal && archive.BeginTag(typeIdTag) && archive.Process(m_typeId) && archive.EndTag(typeIdTag);
	retVal = retVal && archive.BeginTag(subjectIdTag) && archive.Process(m_subject.id) && archive.EndTag(subjectIdTag);
	retVal = retVal && archive.BeginTag(subjectTypeIdTag) && archive.Process(m_subject.typeId) && archive.EndTag(subjectTypeIdTag);
	retVal = retVal && archive.BeginTag(subjectNameTag) && archive.Process(m_subject.name) && archive.EndTag(subjectNameTag);
	retVal = retVal && archive.BeginTag(groupKeyTag) && archive.Process(m_groupKey) && archive.EndTag(groupKeyTag);
	retVal = retVal && archive.BeginTag(groupCountTag) && archive.Process(m_groupCount) && archive.EndTag(groupCountTag);
	retVal = retVal && archive.BeginTag(isDeliveredTag) && archive.Process(m_isDelivered) && archive.EndTag(isDeliveredTag);
	retVal = retVal && archive.BeginTag(isReadTag) && archive.Process(m_isRead) && archive.EndTag(isReadTag);

	int actorCount = m_actors.size();
	retVal = retVal && archive.BeginMultiTag(actorsTag, actorTag, actorCount);
	if (retVal && !archive.IsStoring()){
		m_actors.resize(actorCount);
	}

	for (int i = 0; retVal && i < actorCount; ++i){
		ObjectLinkInfo& actor = m_actors[i];
		retVal = retVal && archive.BeginTag(actorTag);
		retVal = retVal && archive.BeginTag(idTag) && archive.Process(actor.id) && archive.EndTag(idTag);
		retVal = retVal && archive.BeginTag(objectTypeIdTag) && archive.Process(actor.typeId) && archive.EndTag(objectTypeIdTag);
		retVal = retVal && archive.BeginTag(nameTag) && archive.Process(actor.name) && archive.EndTag(nameTag);
		retVal = retVal && archive.EndTag(actorTag);
	}

	retVal = retVal && archive.EndTag(actorsTag);

	int argumentCount = m_arguments.size();
	retVal = retVal && archive.BeginMultiTag(argumentsTag, argumentTag, argumentCount);
	if (retVal && !archive.IsStoring()){
		m_arguments.resize(argumentCount);
	}

	for (int i = 0; retVal && i < argumentCount; ++i){
		NotificationArgument& argument = m_arguments[i];
		QByteArray kindName = NotificationArgument::GetKindName(argument.kind);
		retVal = retVal && archive.BeginTag(argumentTag);
		retVal = retVal && archive.BeginTag(idTag) && archive.Process(argument.id) && archive.EndTag(idTag);
		retVal = retVal && archive.BeginTag(kindTag) && archive.Process(kindName) && archive.EndTag(kindTag);
		retVal = retVal && archive.BeginTag(valueTag) && archive.Process(argument.value) && archive.EndTag(valueTag);
		retVal = retVal && archive.BeginTag(objectIdTag) && archive.Process(argument.objectId) && archive.EndTag(objectIdTag);
		retVal = retVal && archive.BeginTag(objectTypeIdTag) && archive.Process(argument.objectTypeId) && archive.EndTag(objectTypeIdTag);
		retVal = retVal && archive.EndTag(argumentTag);
		retVal = retVal && (archive.IsStoring() || NotificationArgument::FindKind(kindName, argument.kind));
	}

	retVal = retVal && archive.EndTag(argumentsTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CNotification::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CNotification* sourcePtr = dynamic_cast<const CNotification*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);
	Q_UNUSED(changeNotifier);

	*this = *sourcePtr;

	return true;
}


istd::IChangeableUniquePtr CNotification::CloneMe(CompatibilityMode /*mode*/) const
{
	return new CNotification(*this);
}


} // namespace imtnotify



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotify/CNotificationComp.h>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtnotify
{


// reimplemented (imtnotify::INotification)

QByteArray CNotificationComp::GetId() const
{
	return m_id;
}


void CNotificationComp::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
	}
}


QByteArray CNotificationComp::GetRecipientId() const
{
	return m_recipientId;
}


void CNotificationComp::SetRecipientId(const QByteArray& recipientId)
{
	if (m_recipientId != recipientId){
		istd::CChangeNotifier notifier(this);

		m_recipientId = recipientId;
	}
}


QString CNotificationComp::GetCategory() const
{
	return m_category;
}


void CNotificationComp::SetCategory(const QString& category)
{
	if (m_category != category){
		istd::CChangeNotifier notifier(this);

		m_category = category;
	}
}


QString CNotificationComp::GetTitle() const
{
	return m_title;
}


void CNotificationComp::SetTitle(const QString& title)
{
	if (m_title != title){
		istd::CChangeNotifier notifier(this);

		m_title = title;
	}
}


QString CNotificationComp::GetBody() const
{
	return m_body;
}


void CNotificationComp::SetBody(const QString& body)
{
	if (m_body != body){
		istd::CChangeNotifier notifier(this);

		m_body = body;
	}
}


QString CNotificationComp::GetIconName() const
{
	return m_iconName;
}


void CNotificationComp::SetIconName(const QString& iconName)
{
	if (m_iconName != iconName){
		istd::CChangeNotifier notifier(this);

		m_iconName = iconName;
	}
}


INotification::Severity CNotificationComp::GetSeverity() const
{
	return m_severity;
}


void CNotificationComp::SetSeverity(INotification::Severity severity)
{
	if (m_severity != severity){
		istd::CChangeNotifier notifier(this);

		m_severity = severity;
	}
}


bool CNotificationComp::IsRead() const
{
	return m_isRead;
}


void CNotificationComp::SetRead(bool isRead)
{
	if (m_isRead != isRead){
		istd::CChangeNotifier notifier(this);

		m_isRead = isRead;
	}
}


QString CNotificationComp::GetSourceType() const
{
	return m_sourceType;
}


void CNotificationComp::SetSourceType(const QString& sourceType)
{
	if (m_sourceType != sourceType){
		istd::CChangeNotifier notifier(this);

		m_sourceType = sourceType;
	}
}


QByteArray CNotificationComp::GetSourceId() const
{
	return m_sourceId;
}


void CNotificationComp::SetSourceId(const QByteArray& sourceId)
{
	if (m_sourceId != sourceId){
		istd::CChangeNotifier notifier(this);

		m_sourceId = sourceId;
	}
}


QString CNotificationComp::GetTargetRoute() const
{
	return m_targetRoute;
}


void CNotificationComp::SetTargetRoute(const QString& targetRoute)
{
	if (m_targetRoute != targetRoute){
		istd::CChangeNotifier notifier(this);

		m_targetRoute = targetRoute;
	}
}


QString CNotificationComp::GetMetadata() const
{
	return m_metadata;
}


void CNotificationComp::SetMetadata(const QString& metadata)
{
	if (m_metadata != metadata){
		istd::CChangeNotifier notifier(this);

		m_metadata = metadata;
	}
}


QString CNotificationComp::GetCreatedAt() const
{
	return m_createdAt;
}


void CNotificationComp::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);

		m_createdAt = createdAt;
	}
}


QString CNotificationComp::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CNotificationComp::SetUpdatedAt(const QString& updatedAt)
{
	if (m_updatedAt != updatedAt){
		istd::CChangeNotifier notifier(this);

		m_updatedAt = updatedAt;
	}
}


// reimplemented (iser::ISerializable)

bool CNotificationComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag recipientIdTag("RecipientId", "Recipient id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(recipientIdTag);
	retVal = retVal && archive.Process(m_recipientId);
	retVal = retVal && archive.EndTag(recipientIdTag);

	static iser::CArchiveTag categoryTag("Category", "Category", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(categoryTag);
	retVal = retVal && archive.Process(m_category);
	retVal = retVal && archive.EndTag(categoryTag);

	static iser::CArchiveTag titleTag("Title", "Title", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(titleTag);
	retVal = retVal && archive.Process(m_title);
	retVal = retVal && archive.EndTag(titleTag);

	static iser::CArchiveTag bodyTag("Body", "Body", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(bodyTag);
	retVal = retVal && archive.Process(m_body);
	retVal = retVal && archive.EndTag(bodyTag);

	static iser::CArchiveTag iconNameTag("IconName", "Icon name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(iconNameTag);
	retVal = retVal && archive.Process(m_iconName);
	retVal = retVal && archive.EndTag(iconNameTag);

	retVal = retVal && I_SERIALIZE_ENUM(Severity, archive, m_severity);

	static iser::CArchiveTag isReadTag("IsRead", "Is read", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isReadTag);
	retVal = retVal && archive.Process(m_isRead);
	retVal = retVal && archive.EndTag(isReadTag);

	static iser::CArchiveTag sourceTypeTag("SourceType", "Source type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceTypeTag);
	retVal = retVal && archive.Process(m_sourceType);
	retVal = retVal && archive.EndTag(sourceTypeTag);

	static iser::CArchiveTag sourceIdTag("SourceId", "Source id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceIdTag);
	retVal = retVal && archive.Process(m_sourceId);
	retVal = retVal && archive.EndTag(sourceIdTag);

	static iser::CArchiveTag targetRouteTag("TargetRoute", "Target route", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetRouteTag);
	retVal = retVal && archive.Process(m_targetRoute);
	retVal = retVal && archive.EndTag(targetRouteTag);

	static iser::CArchiveTag metadataTag("Metadata", "Metadata", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(metadataTag);
	retVal = retVal && archive.Process(m_metadata);
	retVal = retVal && archive.EndTag(metadataTag);

	static iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	static iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CNotificationComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const INotification* srcPtr = dynamic_cast<const INotification*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_id = srcPtr->GetId();
	m_recipientId = srcPtr->GetRecipientId();
	m_category = srcPtr->GetCategory();
	m_title = srcPtr->GetTitle();
	m_body = srcPtr->GetBody();
	m_iconName = srcPtr->GetIconName();
	m_severity = srcPtr->GetSeverity();
	m_isRead = srcPtr->IsRead();
	m_sourceType = srcPtr->GetSourceType();
	m_sourceId = srcPtr->GetSourceId();
	m_targetRoute = srcPtr->GetTargetRoute();
	m_metadata = srcPtr->GetMetadata();
	m_createdAt = srcPtr->GetCreatedAt();
	m_updatedAt = srcPtr->GetUpdatedAt();
	return true;
}


bool CNotificationComp::IsEqual(const IChangeable& object) const
{
	const INotification* srcPtr = dynamic_cast<const INotification*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	return m_id == srcPtr->GetId()
		&& m_recipientId == srcPtr->GetRecipientId()
		&& m_category == srcPtr->GetCategory()
		&& m_title == srcPtr->GetTitle()
		&& m_body == srcPtr->GetBody()
		&& m_iconName == srcPtr->GetIconName()
		&& m_severity == srcPtr->GetSeverity()
		&& m_isRead == srcPtr->IsRead()
		&& m_sourceType == srcPtr->GetSourceType()
		&& m_sourceId == srcPtr->GetSourceId()
		&& m_targetRoute == srcPtr->GetTargetRoute()
		&& m_metadata == srcPtr->GetMetadata()
		&& m_createdAt == srcPtr->GetCreatedAt()
		&& m_updatedAt == srcPtr->GetUpdatedAt();
}


istd::IChangeableUniquePtr CNotificationComp::CloneMe(CompatibilityMode mode) const
{
	istd::TUniqueInterfacePtr<CNotificationComp> clonePtr(new CNotificationComp());
	clonePtr->CopyFrom(*this, mode);
	return clonePtr;
}


bool CNotificationComp::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_id.clear();
	m_recipientId.clear();
	m_category.clear();
	m_title.clear();
	m_body.clear();
	m_iconName.clear();
	m_severity = INotification::SV_INFO;
	m_isRead = false;
	m_sourceType.clear();
	m_sourceId.clear();
	m_targetRoute.clear();
	m_metadata.clear();
	m_createdAt.clear();
	m_updatedAt.clear();
	return true;
}


} // namespace imtnotify

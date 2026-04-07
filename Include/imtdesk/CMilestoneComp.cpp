// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesk/CMilestoneComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtdesk
{


// reimplemented (imtdesk::IMilestone)

QByteArray CMilestoneComp::GetId() const
{
	return m_id;
}


void CMilestoneComp::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
	}
}


QString CMilestoneComp::GetTitle() const
{
	return m_title;
}


void CMilestoneComp::SetTitle(const QString& title)
{
	if (m_title != title){
		istd::CChangeNotifier notifier(this);

		m_title = title;
	}
}


QString CMilestoneComp::GetDescription() const
{
	return m_description;
}


void CMilestoneComp::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier notifier(this);

		m_description = description;
	}
}


IMilestone::MilestoneState CMilestoneComp::GetState() const
{
	return m_state;
}


void CMilestoneComp::SetState(MilestoneState state)
{
	if (m_state != state){
		istd::CChangeNotifier notifier(this);

		m_state = state;
	}
}


QString CMilestoneComp::GetDueOn() const
{
	return m_dueOn;
}


void CMilestoneComp::SetDueOn(const QString& dueOn)
{
	if (m_dueOn != dueOn){
		istd::CChangeNotifier notifier(this);

		m_dueOn = dueOn;
	}
}


QString CMilestoneComp::GetCreatedAt() const
{
	return m_createdAt;
}


void CMilestoneComp::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);

		m_createdAt = createdAt;
	}
}


QString CMilestoneComp::GetClosedAt() const
{
	return m_closedAt;
}


void CMilestoneComp::SetClosedAt(const QString& closedAt)
{
	if (m_closedAt != closedAt){
		istd::CChangeNotifier notifier(this);

		m_closedAt = closedAt;
	}
}


// reimplemented (iser::ISerializable)

bool CMilestoneComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag titleTag("Title", "Title", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(titleTag);
	retVal = retVal && archive.Process(m_title);
	retVal = retVal && archive.EndTag(titleTag);

	static iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	static iser::CArchiveTag stateTag("State", "State", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(stateTag);
	retVal = retVal && I_SERIALIZE_ENUM(MilestoneState, archive, m_state);
	retVal = retVal && archive.EndTag(stateTag);

	static iser::CArchiveTag dueOnTag("DueOn", "Due on", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(dueOnTag);
	retVal = retVal && archive.Process(m_dueOn);
	retVal = retVal && archive.EndTag(dueOnTag);

	static iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	static iser::CArchiveTag closedAtTag("ClosedAt", "Closed at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(closedAtTag);
	retVal = retVal && archive.Process(m_closedAt);
	retVal = retVal && archive.EndTag(closedAtTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CMilestoneComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IMilestone* srcPtr = dynamic_cast<const IMilestone*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_id = srcPtr->GetId();
	m_title = srcPtr->GetTitle();
	m_description = srcPtr->GetDescription();
	m_state = srcPtr->GetState();
	m_dueOn = srcPtr->GetDueOn();
	m_createdAt = srcPtr->GetCreatedAt();
	m_closedAt = srcPtr->GetClosedAt();

	return true;
}


bool CMilestoneComp::IsEqual(const IChangeable& object) const
{
	const IMilestone* srcPtr = dynamic_cast<const IMilestone*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	return m_id == srcPtr->GetId()
		&& m_title == srcPtr->GetTitle()
		&& m_description == srcPtr->GetDescription()
		&& m_state == srcPtr->GetState()
		&& m_dueOn == srcPtr->GetDueOn()
		&& m_createdAt == srcPtr->GetCreatedAt()
		&& m_closedAt == srcPtr->GetClosedAt();
}


istd::IChangeableUniquePtr CMilestoneComp::CloneMe(CompatibilityMode mode) const
{
	istd::TUniqueInterfacePtr<CMilestoneComp> clonePtr(new CMilestoneComp());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CMilestoneComp::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_id.clear();
	m_title.clear();
	m_description.clear();
	m_state = MS_OPEN;
	m_dueOn.clear();
	m_createdAt.clear();
	m_closedAt.clear();

	return true;
}


} // namespace imtdesk

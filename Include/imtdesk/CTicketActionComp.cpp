// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesk/CTicketActionComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtdesk
{


// reimplemented (imtdesk::ITicketAction)

QByteArray CTicketActionComp::GetTicketId() const
{
	return m_ticketId;
}


void CTicketActionComp::SetTicketId(const QByteArray& ticketId)
{
	if (m_ticketId != ticketId){
		istd::CChangeNotifier notifier(this);

		m_ticketId = ticketId;
	}
}


// reimplemented (iser::ISerializable)

bool CTicketActionComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass2::Serialize(archive);

	static iser::CArchiveTag ticketIdTag("TicketId", "Ticket ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(ticketIdTag);
	retVal = retVal && archive.Process(m_ticketId);
	retVal = retVal && archive.EndTag(ticketIdTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CTicketActionComp::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const ITicketAction* srcPtr = dynamic_cast<const ITicketAction*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	BaseClass2::CopyFrom(object, mode);
	m_ticketId = srcPtr->GetTicketId();

	return true;
}


bool CTicketActionComp::IsEqual(const IChangeable& object) const
{
	const ITicketAction* srcPtr = dynamic_cast<const ITicketAction*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	return BaseClass2::IsEqual(object)
		&& m_ticketId == srcPtr->GetTicketId();
}


istd::IChangeableUniquePtr CTicketActionComp::CloneMe(CompatibilityMode mode) const
{
	istd::TUniqueInterfacePtr<CTicketActionComp> clonePtr(new CTicketActionComp());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTicketActionComp::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier notifier(this);

	BaseClass2::ResetData(mode);
	m_ticketId.clear();

	return true;
}


} // namespace imtdesk

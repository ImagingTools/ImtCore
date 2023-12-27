#include <imtauth/CContactBaseInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (IContactBaseInfo)

QString CContactBaseInfo::GetName() const
{
	return m_name;
}


void CContactBaseInfo::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNorifier(this);

		m_name = name;
	}
}


QString CContactBaseInfo::GetDescription() const
{
	return m_description;
}


void CContactBaseInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNorifier(this);

		m_description = description;
	}
}


QString CContactBaseInfo::GetEmail() const
{
	return m_email;
}


void CContactBaseInfo::SetEmail(const QString& email)
{
	if (m_email != email){
		istd::CChangeNotifier changeNorifier(this);

		m_email = email;
	}
}


const iimg::IBitmap& CContactBaseInfo::GetPicture() const
{
	return m_picture;
}


void CContactBaseInfo::SetPicture(const iimg::IBitmap& picture)
{
	istd::CChangeNotifier notifier(this);

	m_picture.CopyFrom(picture);
}


QByteArrayList CContactBaseInfo::GetGroups() const
{
	return m_groupIds;
}


bool CContactBaseInfo::AddGroup(const QByteArray& groupId)
{
	if (!m_groupIds.contains(groupId)){
		istd::CChangeNotifier changeNotifier(this);

		m_groupIds << groupId;

		return true;
	}

	return false;
}


bool CContactBaseInfo::RemoveGroup(const QByteArray& groupId)
{
	bool result = m_groupIds.removeAll(groupId);
	if (result){
		istd::CChangeNotifier changeNotifier(this);
	}

	return result;
}


// reimplemented (iser::ISerializable)

bool CContactBaseInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag accountNameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(accountNameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(accountNameTag);

	iser::CArchiveTag accountDescriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(accountDescriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(accountDescriptionTag);

	iser::CArchiveTag mailTag("Email", "Email", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(mailTag);
	retVal = retVal && archive.Process(m_email);
	retVal = retVal && archive.EndTag(mailTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_groupIds, "Groups", "Group");

	iser::CArchiveTag pictureTag("Picture", "Picture", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(pictureTag);
	retVal = retVal && m_picture.Serialize(archive);
	retVal = retVal && archive.EndTag(pictureTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CContactBaseInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CContactBaseInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CContactBaseInfo* sourcePtr = dynamic_cast<const CContactBaseInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		bool retVal = true;

		m_name = sourcePtr->m_name;
		m_description = sourcePtr->m_description;
		m_email = sourcePtr->m_email;
		m_groupIds = sourcePtr->m_groupIds;

		retVal = retVal && m_picture.CopyFrom(sourcePtr->m_picture);
		retVal = retVal && m_addresses.CopyFrom(sourcePtr->m_addresses);

		return retVal;
	}

	return false;
}


istd::IChangeable* CContactBaseInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CContactBaseInfo> clonePtr(new CContactBaseInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CContactBaseInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	bool retVal = true;

	m_name.clear();
	m_description.clear();
	m_email.clear();
	m_groupIds.clear();

	retVal = retVal && m_picture.ResetData();
	retVal = retVal && m_addresses.ResetData();

	return retVal;
}


} // namespace imtauth



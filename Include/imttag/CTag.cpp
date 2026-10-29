// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttag/CTag.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imttag
{


// public methods

CTag::CTag()
	:m_isSystem(false)
{
}


// reimplemented (imttag::ITag)

QByteArray CTag::GetId() const
{
	return m_id;
}


void CTag::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
	}
}


QString CTag::GetName() const
{
	return m_name;
}


void CTag::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier notifier(this);

		m_name = name;
	}
}


QString CTag::GetColor() const
{
	return m_color;
}


void CTag::SetColor(const QString& color)
{
	if (m_color != color){
		istd::CChangeNotifier notifier(this);

		m_color = color;
	}
}


QString CTag::GetDescription() const
{
	return m_description;
}


void CTag::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier notifier(this);

		m_description = description;
	}
}


bool CTag::IsSystem() const
{
	return m_isSystem;
}


void CTag::SetSystem(bool isSystem)
{
	if (m_isSystem != isSystem){
		istd::CChangeNotifier notifier(this);

		m_isSystem = isSystem;
	}
}


// reimplemented (iser::ISerializable)

bool CTag::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag colorTag("Color", "Color", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(colorTag);
	retVal = retVal && archive.Process(m_color);
	retVal = retVal && archive.EndTag(colorTag);

	static iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	static iser::CArchiveTag isSystemTag("IsSystem", "Is system tag", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isSystemTag);
	retVal = retVal && archive.Process(m_isSystem);
	retVal = retVal && archive.EndTag(isSystemTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTag::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CTag::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const ITag* srcPtr = dynamic_cast<const ITag*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_id = srcPtr->GetId();
	m_name = srcPtr->GetName();
	m_color = srcPtr->GetColor();
	m_description = srcPtr->GetDescription();
	m_isSystem = srcPtr->IsSystem();

	return true;
}


bool CTag::IsEqual(const IChangeable& object) const
{
	const ITag* srcPtr = dynamic_cast<const ITag*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	return m_id == srcPtr->GetId()
		&& m_name == srcPtr->GetName()
		&& m_color == srcPtr->GetColor()
		&& m_description == srcPtr->GetDescription()
		&& m_isSystem == srcPtr->IsSystem();
}


istd::IChangeableUniquePtr CTag::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTag);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTag::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_id.clear();
	m_name.clear();
	m_color.clear();
	m_description.clear();
	m_isSystem = false;

	return true;
}


} // namespace imttag



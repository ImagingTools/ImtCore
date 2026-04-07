// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesk/CLabelComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtdesk
{


// reimplemented (imtdesk::ILabel)

QByteArray CLabelComp::GetId() const
{
	return m_id;
}


void CLabelComp::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
	}
}


QString CLabelComp::GetName() const
{
	return m_name;
}


void CLabelComp::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier notifier(this);

		m_name = name;
	}
}


QString CLabelComp::GetColor() const
{
	return m_color;
}


void CLabelComp::SetColor(const QString& color)
{
	if (m_color != color){
		istd::CChangeNotifier notifier(this);

		m_color = color;
	}
}


QString CLabelComp::GetDescription() const
{
	return m_description;
}


void CLabelComp::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier notifier(this);

		m_description = description;
	}
}


// reimplemented (iser::ISerializable)

bool CLabelComp::Serialize(iser::IArchive& archive)
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

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CLabelComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const ILabel* srcPtr = dynamic_cast<const ILabel*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_id = srcPtr->GetId();
	m_name = srcPtr->GetName();
	m_color = srcPtr->GetColor();
	m_description = srcPtr->GetDescription();

	return true;
}


bool CLabelComp::IsEqual(const IChangeable& object) const
{
	const ILabel* srcPtr = dynamic_cast<const ILabel*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	return m_id == srcPtr->GetId()
		&& m_name == srcPtr->GetName()
		&& m_color == srcPtr->GetColor()
		&& m_description == srcPtr->GetDescription();
}


istd::IChangeableUniquePtr CLabelComp::CloneMe(CompatibilityMode mode) const
{
	istd::TUniqueInterfacePtr<CLabelComp> clonePtr(new CLabelComp());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CLabelComp::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_id.clear();
	m_name.clear();
	m_color.clear();
	m_description.clear();

	return true;
}


} // namespace imtdesk

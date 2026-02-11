// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CSdlField.h>

//Acf includes
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>

// ImtCore includes
#include <imtbase/imtbase.h>



namespace imtsdl
{


// public static methods

bool CSdlField::SerializeSdlFieldList(
			iser::IArchive& archive,
			QList<CSdlField>& container,
			const QByteArray& containerTagName,
			const QByteArray& elementTagName)
{
	iser::CArchiveTag elementsTag(containerTagName, "List of elements", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag elementTag(elementTagName, "Single element", iser::CArchiveTag::TT_LEAF, &elementsTag);

	bool retVal = true;

	bool isStoring = archive.IsStoring();
	int elementsCount = imtbase::narrow_cast<int>(container.count());

	retVal = retVal && archive.BeginMultiTag(elementsTag, elementTag, elementsCount);
	if (!retVal){
		return false;
	}

	if (isStoring){
		for (int i = 0; i < elementsCount; ++i){
			CSdlField sdlField = container[i];

			retVal = retVal && archive.BeginTag(elementTag);
			retVal = retVal && sdlField.Serialize(archive);
			retVal = retVal && archive.EndTag(elementTag);
		}
	}
	else{
		container.clear();

		for (int i = 0; i < elementsCount; ++i){
			CSdlField sdlField;
			retVal = retVal && archive.BeginTag(elementTag);
			retVal = retVal && sdlField.Serialize(archive);
			retVal = retVal && archive.EndTag(elementTag);

			if (retVal){
				container.push_back(sdlField);
			}
		}
	}

	retVal = retVal && archive.EndTag(elementsTag);

	return retVal;
}


// public methods

CSdlField::CSdlField():
	m_isRequired(false),
	m_isArray(false),
	m_isNonEmpty(false),
	m_type("<INVALID>")
{
}


bool CSdlField::IsRequired() const
{
	return m_isRequired;
}


void CSdlField::SetIsRequired(bool isRequired)
{
	if (m_isRequired != isRequired){
		istd::CChangeNotifier notifier(this);

		m_isRequired = isRequired;
	}
}


bool CSdlField::IsArray() const
{
	return m_isArray;
}


void CSdlField::SetIsArray(bool isArray)
{
	if (m_isArray != isArray){
		istd::CChangeNotifier notifier(this);

		m_isArray = isArray;
	}
}


bool CSdlField::IsNonEmpty() const
{
	return m_isNonEmpty;
}


void CSdlField::SetIsNonEmpty(bool isNonEmpty)
{
	if (m_isNonEmpty != isNonEmpty){
		istd::CChangeNotifier notifier(this);

		m_isNonEmpty = isNonEmpty;
	}
}


QString CSdlField::GetType() const
{
	return m_type;
}


void CSdlField::SetType(const QString& type)
{
	if (m_type != type){
		istd::CChangeNotifier notifier(this);

		m_type = type;
	}
}


QString CSdlField::GetId() const
{
	return m_id;
}


void CSdlField::SetId(const QString& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
	}
}


bool CSdlField::operator==(const CSdlField& other) const
{
	bool retVal = true;

	retVal = retVal && (other.m_isRequired == this->m_isRequired);
	retVal = retVal && (other.m_isArray == this->m_isArray);
	retVal = retVal && (other.m_isNonEmpty == this->m_isNonEmpty);
	retVal = retVal && (other.m_type == this->m_type);
	retVal = retVal && (other.m_id == this->m_id);

	return retVal;
}


// reimplemented(iser::ISerializable)

bool CSdlField::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag idTag("Id", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag typeTag("Type", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeTag);
	retVal = retVal && archive.Process(m_type);
	retVal = retVal && archive.EndTag(typeTag);

	static iser::CArchiveTag nonEmptyTag("IsNonEmpty", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nonEmptyTag);
	retVal = retVal && archive.Process(m_isNonEmpty);
	retVal = retVal && archive.EndTag(nonEmptyTag);

	static iser::CArchiveTag arrayTag("IsArray", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(arrayTag);
	retVal = retVal && archive.Process(m_isArray);
	retVal = retVal && archive.EndTag(arrayTag);

	static iser::CArchiveTag requireTag("IsRequired", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(requireTag);
	retVal = retVal && archive.Process(m_isRequired);
	retVal = retVal && archive.BeginTag(requireTag);

	return retVal;
}


} // namespace imtsdl

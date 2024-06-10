#include <imtsdl/CSdlType.h>

//Acf includes
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>


// imtsdl includes
#include <imtsdl/CSdlField.h>


namespace imtsdl
{


// public methods

QString CSdlType::GetName() const
{
	return m_name;
}


void CSdlType::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier notifier(this);
		m_name = name;
	}
}


SdlFieldList CSdlType::GetFields() const
{
	return m_fields;
}


void CSdlType::SetFields(const SdlFieldList& fields)
{
	if (fields != m_fields){
		istd::CChangeNotifier notifier(this);
		m_fields = fields;
	}
}


// reimplemented(iser::ISerializable)

bool CSdlType::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	iser::CArchiveTag nameTag("Name", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	retVal = retVal && CSdlField::SerializeSdlFieldList(archive, m_fields, "Fields", "Field");

	return retVal;
}


bool CSdlType::operator==(const CSdlType& other) const
{
	return 	m_name == other.m_name && m_fields == other.m_fields;
}


} // namespace imtsdl



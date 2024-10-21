#include <imtsdl/CSdlType.h>

//Acf includes
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>


// ImtCore includes
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


QString CSdlType::GetNamespace() const
{
	return m_namespace;
}


void CSdlType::SetNamespace(const QString& aNamespace)
{
	if (m_namespace != aNamespace){
		istd::CChangeNotifier notifier(this);
		m_namespace = aNamespace;
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

	iser::CArchiveTag namespaceTag("Namespace", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(namespaceTag);
	retVal = retVal && archive.Process(m_namespace);
	retVal = retVal && archive.EndTag(namespaceTag);

	retVal = retVal && CSdlField::SerializeSdlFieldList(archive, m_fields, "Fields", "Field");

	return retVal;
}


bool CSdlType::operator==(const CSdlType& other) const
{
	return 	m_name == other.m_name && m_fields == other.m_fields;
}


} // namespace imtsdl



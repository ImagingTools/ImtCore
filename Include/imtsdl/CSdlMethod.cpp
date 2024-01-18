#include <imtsdl/CSdlMethod.h>

// Acf includes
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtsdl
{


// public methods

QString CSdlMethod::GetName() const
{
	return m_name;
}


void CSdlMethod::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier notifier(this);
		m_name = name;
	}
}


SdlFieldList CSdlMethod::GetArguments() const
{
	return m_arguments;
}


void CSdlMethod::SetArguments(const SdlFieldList& arguments)
{
	if (m_arguments != arguments){
		istd::CChangeNotifier notifier(this);
		m_arguments = arguments;
	}
}


QString CSdlMethod::GetOutputTypeId() const
{
	return m_outputTypeId;
}


void CSdlMethod::SetOutputTypeId(const QString& outputTypeId)
{
	if (m_outputTypeId != outputTypeId){
		istd::CChangeNotifier notifier(this);
		m_outputTypeId = outputTypeId;
	}
}


// reimplemented(iser::ISerializable)

bool CSdlMethod::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	iser::CArchiveTag nameTag("Name", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	retVal = retVal && CSdlField::SerializeSdlFieldList(archive, m_arguments, "ArgumentList", "Argument");

	iser::CArchiveTag outputTag("OutputTypeId", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(outputTag);
	retVal = retVal && archive.Process(m_outputTypeId);
	retVal = retVal && archive.BeginTag(outputTag);

	return retVal;
}


} // namespace imtsdl

// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CSdlRequest.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iprm/CParamsSet.h>


namespace imtsdl
{

// public methods

CSdlRequest::CSdlRequest() : m_type(T_QUERY)
{
}


CSdlRequest::Type CSdlRequest::GetType() const
{
	return m_type;
}

void CSdlRequest::SetType(Type type)
{
	if (m_type != type){
		istd::CChangeNotifier notifier(this);
		m_type = type;
	}
}


SdlFieldList CSdlRequest::GetInputArguments() const
{
	return m_inputArguments;
}


void CSdlRequest::SetInputArguments(const SdlFieldList& inputArguments)
{
	if (m_inputArguments != inputArguments){
		istd::CChangeNotifier notifier(this);
		m_inputArguments = inputArguments;
	}
}


CSdlField CSdlRequest::GetOutputArgument() const
{
	return m_outputArgument;
}


void CSdlRequest::SetOutputArgument(const CSdlField& outputArgument)
{
	if (m_outputArgument != outputArgument){
		istd::CChangeNotifier notifier(this);
		m_outputArgument = outputArgument;
	}
}


bool CSdlRequest::operator==(const CSdlRequest& other) const
{
	return
		m_type == other.m_type &&
		m_name == other.m_name &&
		m_inputArguments == other.m_inputArguments &&
		m_outputArgument == other.m_outputArgument &&
		GetSchemaParams().IsEqual(other.GetSchemaParams());
}


// reimplemented(iser::ISerializable)

bool CSdlRequest::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	iser::CArchiveTag typeTag("Type", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeTag);
	retVal = retVal && I_SERIALIZE_ENUM(Type, archive, m_type);
	retVal = retVal && archive.EndTag(typeTag);

	iser::CArchiveTag nameTag("Name", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	retVal = retVal && CSdlField::SerializeSdlFieldList(archive, m_inputArguments, "InputArguments", "Argument");

	iser::CArchiveTag outputArgumentTag("OutputArgument", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(outputArgumentTag);
	retVal = retVal && m_outputArgument.Serialize(archive);
	retVal = retVal && archive.EndTag(outputArgumentTag);


	return retVal;
}


} // namespace imtsdl



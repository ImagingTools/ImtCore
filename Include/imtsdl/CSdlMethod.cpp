#include <imtsdl/CSdlMethod.h>

// Acf includes
#include<istd/CChangeNotifier.h>


namespace imtsdl
{


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


bool CSdlMethod::Serialize(iser::IArchive& archive)
{
	bool retVal = false;

	return retVal;
}

} // namespace imtsdl

#include <imtsdl/CSdlType.h>

//Acf includes
#include<istd/CChangeNotifier.h>


namespace imtsdl
{


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


bool CSdlType::Serialize(iser::IArchive& archive)
{
	bool retVal = false;


	return retVal;
}


} // namespace imtsdl



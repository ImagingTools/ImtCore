#include <imtbase/CStatusManagerComp.h>


namespace imtbase
{


CStatusManagerComp::CStatusManagerComp()
:	m_category(istd::IInformationProvider::IC_NONE),
	m_id(0),
	m_flags(0)
{
}


// reimplemented (iinsp::IInformationProvider)

QDateTime CStatusManagerComp::GetInformationTimeStamp() const
{
	return m_timeStamp;
}


istd::IInformationProvider::InformationCategory CStatusManagerComp::GetInformationCategory() const
{
	return m_category;
}


int CStatusManagerComp::GetInformationId() const
{
	return m_id;
}


QString CStatusManagerComp::GetInformationDescription() const
{
	if (m_description.isEmpty() && m_defaultDescriptionAttrPtr.IsValid()){
		return *m_defaultDescriptionAttrPtr;
	}

	return m_description;
}


QString CStatusManagerComp::GetInformationSource() const
{
	if (m_source.isEmpty() && m_defaultSourceAttrPtr.IsValid()){
		return *m_defaultSourceAttrPtr;
	}

	return m_source;
}


int CStatusManagerComp::GetInformationFlags() const
{
	return m_flags;
}


// reimplemented (istd::IChangeable)

bool CStatusManagerComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const istd::IInformationProvider* providerPtr = dynamic_cast<const IInformationProvider*>(&object);
	if (providerPtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_timeStamp = providerPtr->GetInformationTimeStamp();
		m_category = providerPtr->GetInformationCategory();
		m_id = providerPtr->GetInformationId();
		m_description = providerPtr->GetInformationDescription();
		m_source = providerPtr->GetInformationSource();
		m_flags = providerPtr->GetInformationFlags();

		return true;
	}

	return false;
}


bool CStatusManagerComp::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_timeStamp = QDateTime::currentDateTime();
	m_category = istd::IInformationProvider::IC_NONE;
	m_id = 0;
	m_description.clear();
	m_source.clear();
	m_flags = 0;

	return true;
}


} // namespace imtbase



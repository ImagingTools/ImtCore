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


// reimplemented (imtbase::IStatusManager)

void CStatusManagerComp::SetInformationTimeStamp(const QDateTime& timestamp)
{
	if (m_timeStamp != timestamp){
		istd::CChangeNotifier changeNotifier(this);

		m_timeStamp = timestamp;
	}
}


void CStatusManagerComp::SetInformationCategory(InformationCategory category)
{
	if (m_category != category){
		istd::CChangeNotifier changeNotifier(this);

		m_category = category;
	}
}


void CStatusManagerComp::SetInformationId(int id)
{
	if (m_id != id){
		istd::CChangeNotifier changeNotifier(this);

		m_id = id;
	}
}


void CStatusManagerComp::SetInformationDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


void CStatusManagerComp::SetInformationSource(const QString& source)
{
	if (m_source != source){
		istd::CChangeNotifier changeNotifier(this);

		m_source = source;
	}
}


void CStatusManagerComp::SetInformationFlags(int flags)
{
	if (m_flags != flags){
		istd::CChangeNotifier changeNotifier(this);

		m_flags = flags;
	}
}


// reimplemented (istd::IChangeable)

bool CStatusManagerComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtbase::CStatusManagerComp* providerPtr = dynamic_cast<const imtbase::CStatusManagerComp*>(&object);
	if (providerPtr){
		istd::CChangeNotifier changeNotifier(this);

		m_timeStamp = providerPtr->m_timeStamp;
		m_category = providerPtr->m_category;
		m_id = providerPtr->m_id;
		m_description = providerPtr->m_description;
		m_source = providerPtr->m_source;
		m_flags = providerPtr->m_flags;

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



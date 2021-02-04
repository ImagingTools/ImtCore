#include <imtbase/CSimpleStatus.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtbase
{


CSimpleStatus::CSimpleStatus()
:	m_category(istd::IInformationProvider::IC_NONE),
	m_id(0),
	m_flags(0)
{
}


// reimplemented (iinsp::IInformationProvider)

QDateTime CSimpleStatus::GetInformationTimeStamp() const
{
	return m_timeStamp;
}


istd::IInformationProvider::InformationCategory CSimpleStatus::GetInformationCategory() const
{
	return m_category;
}


int CSimpleStatus::GetInformationId() const
{
	return m_id;
}


QString CSimpleStatus::GetInformationDescription() const
{
	return m_description;
}


QString CSimpleStatus::GetInformationSource() const
{
	return m_source;
}


int CSimpleStatus::GetInformationFlags() const
{
	return m_flags;
}


// reimplemented (imtbase::IStatusManager)

void CSimpleStatus::SetInformationTimeStamp(const QDateTime& timestamp)
{
	if (m_timeStamp != timestamp){
		istd::CChangeNotifier changeNotifier(this);

		m_timeStamp = timestamp;
	}
}


void CSimpleStatus::SetInformationCategory(InformationCategory category)
{
	if (m_category != category){
		istd::CChangeNotifier changeNotifier(this);

		m_category = category;
	}
}


void CSimpleStatus::SetInformationId(int id)
{
	if (m_id != id){
		istd::CChangeNotifier changeNotifier(this);

		m_id = id;
	}
}


void CSimpleStatus::SetInformationDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


void CSimpleStatus::SetInformationSource(const QString& source)
{
	if (m_source != source){
		istd::CChangeNotifier changeNotifier(this);

		m_source = source;
	}
}


void CSimpleStatus::SetInformationFlags(int flags)
{
	if (m_flags != flags){
		istd::CChangeNotifier changeNotifier(this);

		m_flags = flags;
	}
}


// reimplemented (istd::IChangeable)

bool CSimpleStatus::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const istd::IInformationProvider* providerPtr = dynamic_cast<const istd::IInformationProvider*>(&object);
	if (providerPtr){
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


bool CSimpleStatus::ResetData(CompatibilityMode /*mode*/)
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



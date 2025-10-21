#include <imtbase/CApplicationInfoControllerComp.h>


// ACF includes
#include <icomm/CCommonVersionInfo.h>


namespace imtbase
{


// public methods

// reimplemented (imtbase::IApplicationInfoController)

void CApplicationInfoControllerComp::SetApplicationAttribute(ApplicationAttribute attributeId, const QString& attribute)
{
	// QMutexLocker locker(&m_attributesMutex);

	istd::CChangeNotifier changeNotifier(this);

	m_attributes[attributeId] = attribute;
}


// reimplemented (ibase::IApplicationInfo)

QString CApplicationInfoControllerComp::GetApplicationAttribute(int attributeId, bool /*allowTranslation*/) const
{
	// QMutexLocker locker(&m_attributesMutex);

	if (m_attributes.contains(attributeId)){
		return m_attributes[attributeId];
	}

	return QString();
}


int CApplicationInfoControllerComp::GetMainVersionId() const
{
	return -1;
}


const iser::IVersionInfo& CApplicationInfoControllerComp::GetVersionInfo() const
{
	if (m_versionInfoCompPtr.IsValid()){
		return *m_versionInfoCompPtr;
	}

	static icomm::CCommonVersionInfo commonVersion;

	return commonVersion;
}


} // namespace imtbase



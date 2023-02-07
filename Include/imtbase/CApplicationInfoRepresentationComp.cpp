#include <imtbase/CApplicationInfoRepresentationComp.h>


// ACF includes
#include <iprm/ITextParam.h>


namespace imtbase
{


// public methods

// reimplemented (imtbase::IApplicationInfoRepresentation)

bool CApplicationInfoRepresentationComp::GetRepresentationFromApplicationInfo(const ibase::IApplicationInfo& applicationInfo, CTreeItemModel& representation) const
{
	if(m_paramIdAttrPtr.IsValid()){
		representation.SetData("Id", *m_paramIdAttrPtr);
	}

	if(m_paramNameAttrPtr.IsValid()){
		representation.SetData("Id", *m_paramNameAttrPtr);
	}

	if(m_sourceAttrPtr.IsValid()){
		representation.SetData("Id", *m_sourceAttrPtr);
	}

	const iser::IVersionInfo& versionInfo =  applicationInfo.GetVersionInfo();

	int mainVersion = applicationInfo.GetMainVersionId();

	quint32 versionNumber;
	if (versionInfo.GetVersionNumber(mainVersion, versionNumber)){
		QString version = versionInfo.GetEncodedVersionName(mainVersion, versionNumber);

		representation.SetData("Value", version);

		return true;
	}

	return false;
}


} // namespace imtbase



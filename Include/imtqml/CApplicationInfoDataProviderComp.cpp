#include <imtqml/CApplicationInfoDataProviderComp.h>


namespace imtqml
{


// public methods

// reimplemented (imtgql::IItemBasedRepresentationProvider)

QByteArray CApplicationInfoDataProviderComp::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


imtbase::CTreeItemModel* CApplicationInfoDataProviderComp::GetRepresentation(
			const QList<imtgql::CGqlObject>& params,
			const QByteArrayList& fields,
			const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	if (m_paramIdAttrPtr.IsValid()){
		rootModelPtr->SetData("Id", *m_paramIdAttrPtr);
	}

	if (m_paramNameAttrPtr.IsValid()){
		rootModelPtr->SetData("Name", *m_paramNameAttrPtr);
	}

	if (m_applicationInfoCompPtr.IsValid()){
		ibase::CApplicationInfoComp* applicationInfoPtr  = dynamic_cast<ibase::CApplicationInfoComp*>(m_applicationInfoCompPtr.GetPtr());
		if (applicationInfoPtr != nullptr){
			const iser::IVersionInfo& versionInfo = applicationInfoPtr->GetVersionInfo();

			quint32 versionNumber;
			versionInfo.GetVersionNumber(1983, versionNumber);
			QString version = versionInfo.GetEncodedVersionName(1983, versionNumber);

			rootModelPtr->SetData("Value", version);
			rootModelPtr->SetData("ComponentType", "TextLabel");
		}
	}

	return rootModelPtr;
}


} // namespace imtqml



#include <imtlicgui/CFeaturePackageViewDelegateComp.h>


namespace imtlicgui
{


// public methods

// reimplemented (imtgui::ICollectionViewDelegate)

bool CFeaturePackageViewDelegateComp::IsCommandSupported(int commandId) const
{
	if (commandId == CI_DUPLICATE){
		return false;
	}

	return BaseClass::IsCommandSupported(commandId);
}


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CFeaturePackageViewDelegateComp::SetupSummaryInformation()
{
	BaseClass::SetupSummaryInformation();

	m_summaryInformationTypes.RemoveItem("TypeId");
}


} // namespace aculainspgui



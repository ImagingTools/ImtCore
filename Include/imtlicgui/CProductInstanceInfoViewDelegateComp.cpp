#include <imtlicgui/CProductInstanceInfoViewDelegateComp.h>


namespace imtlicgui
{


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CProductInstanceInfoViewDelegateComp::SetupSummaryInformation()
{
	BaseClass::SetupSummaryInformation();

	m_summaryInformationTypes.RemoveItem("TypeId");
}


} // namespace aculainspgui



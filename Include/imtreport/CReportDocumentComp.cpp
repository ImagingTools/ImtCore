#include <imtreport/CReportDocumentComp.h>


#include <imtreport/IReportPage.h>


namespace imtreport
{


// reimplemented (icomp::CComponentBase)

void CReportDocumentComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	istd::IChangeable* pagePtr = InsertPage();

	imtreport::IReportPage* pageImplPtr = dynamic_cast<imtreport::IReportPage*>(pagePtr);
	if (pageImplPtr != nullptr){
		pageImplPtr->AddText("Hello world!", i2d::CVector2d(0, 0));
		pageImplPtr->AddText("Left top", i2d::CVector2d(-200, -200));
		pageImplPtr->AddText("Right bottom", i2d::CVector2d(200, 200));
		pageImplPtr->AddImage("C:/Work/Develop/Report1.png", i2d::CVector2d(200, 400));
	}
}


} // namespace imtreport



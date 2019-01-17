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
		pageImplPtr->AddImage("C:/Work/Develop/Report1.png", QRect(0, 400, 500, 100));
		pageImplPtr->AddRectangle(QRect(100, 200, 300, 100));
		pageImplPtr->AddPolygone({ i2d::CVector2d(0, 0), i2d::CVector2d(-50, 50), i2d::CVector2d(50, 50) });
	}
}


} // namespace imtreport



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
		pageImplPtr->AddText("Left top", i2d::CVector2d(0, 0));
		pageImplPtr->AddText("Center", i2d::CVector2d(105, 149));
		pageImplPtr->AddText("Right bottom", i2d::CVector2d(160, 260));
		pageImplPtr->AddText("Center aligned", 30, imtreport::IGraphicsElement::Alignment::Center);
		pageImplPtr->AddImage("C:/Work/Develop/Report1.png", QRect(0, 50, 150, 75));
		pageImplPtr->AddRectangle(QRect(0, 150, 210, 20));
		pageImplPtr->AddPolygone({ i2d::CVector2d(0, 0), i2d::CVector2d(50, 50), i2d::CVector2d(50, 50) });
	}
}


} // namespace imtreport



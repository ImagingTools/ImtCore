#include <imtreport/CReportDocumentComp.h>


// ACF includes
#include <i2d/CRectangle.h>

// ImtCore includes
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
		pageImplPtr->AddText("Not aligned",    i2d::CVector2d(0.0, 0.0));
		pageImplPtr->AddText("Left aligned",   i2d::CVector2d(0.0, 20.0), 210.0, Qt::AlignLeft);
		pageImplPtr->AddText("Center aligned", i2d::CVector2d(0.0, 40.0), 210.0, Qt::AlignCenter);
		pageImplPtr->AddText("Right aligned",  i2d::CVector2d(0.0, 60.0), 210.0, Qt::AlignRight);
		pageImplPtr->AddLine(i2d::CLine2d(0.0, 75.0, 210.0, 75.0));
		pageImplPtr->AddRectangle(i2d::CRectangle(10.0, 80.0, 190.0, 10.0), Qt::green);
		pageImplPtr->AddImage("C:/Work/Develop/Report1.png", i2d::CRectangle(10.0, 100.0, 190.0, 80.0));
		pageImplPtr->AddPolygone({ i2d::CVector2d(20.0, 210.0), i2d::CVector2d(190.0, 210.0), i2d::CVector2d(105.0, 250.0) }, Qt::blue);
	}
}


} // namespace imtreport



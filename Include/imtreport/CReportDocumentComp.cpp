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

	// page 1
	imtreport::IReportPage* pageImplPtr = dynamic_cast<imtreport::IReportPage*>(InsertPage());
	if (pageImplPtr != nullptr){
		pageImplPtr->AddText("Page 1", i2d::CVector2d(0.0, 0.0), 210.0, Qt::AlignCenter);
		pageImplPtr->AddLine(i2d::CLine2d(0.0, 10.0, 210.0, 10.0));
		pageImplPtr->AddText("Left aligned",   i2d::CVector2d(0.0, 40.0), 210.0, Qt::AlignLeft);
		pageImplPtr->AddText("Center aligned", i2d::CVector2d(0.0, 60.0), 210.0, Qt::AlignCenter);
		pageImplPtr->AddText("Right aligned",  i2d::CVector2d(0.0, 80.0), 210.0, Qt::AlignRight);
	}

	// page 2
	pageImplPtr = dynamic_cast<imtreport::IReportPage*>(InsertPage());
	if (pageImplPtr) {
		pageImplPtr->AddText("Page 2", i2d::CVector2d(0.0, 0.0), 210.0, Qt::AlignCenter);
		pageImplPtr->AddLine(i2d::CLine2d(0.0, 10.0, 210.0, 10.0));
		pageImplPtr->AddRectangle(i2d::CRectangle(10.0, 50.0, 190.0, 30.0), Qt::green);
		pageImplPtr->AddPolygone({ i2d::CVector2d(20.0, 100.0), i2d::CVector2d(190.0, 100.0), i2d::CVector2d(105.0, 180.0) }, Qt::blue);
	}

	// page 3
	pageImplPtr = dynamic_cast<imtreport::IReportPage*>(InsertPage());
	if (pageImplPtr) {
		pageImplPtr->AddText("Page 3", i2d::CVector2d(0.0, 0.0), 210.0, Qt::AlignCenter);
		pageImplPtr->AddLine(i2d::CLine2d(0.0, 10.0, 210.0, 10.0));
		pageImplPtr->AddImage("C:/Work/Develop/Report1.png", i2d::CRectangle(10.0, 50.0, 190.0, 100.0));
	}
}


} // namespace imtreport



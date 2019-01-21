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
		//pageImplPtr->AddImage("C:/Work/Develop/Report1.png", QRect(0, 50, 150, 75));
		//pageImplPtr->AddRectangle(QRect(0, 150, 210, 20));
		//pageImplPtr->AddPolygone({ i2d::CVector2d(0, 0), i2d::CVector2d(50, 50), i2d::CVector2d(50, 50) });
	}
}


} // namespace imtreport



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml2d/CQmlLineShape.h>



namespace imtqml2d
{


// reimplemented (imod::IObserver)

bool CQmlLineShape::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool retVal = BaseClass::OnModelAttached(modelPtr, changeMask);

	i2d::CLine2d* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		BaseClass::EnsureModelDetached();
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return false;
	}

	return retVal;
}


bool CQmlLineShape::OnModelDetached(imod::IModel* modelPtr)
{
	bool retVal = BaseClass::OnModelDetached(modelPtr);

	return retVal;
}


void CQmlLineShape::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	i2d::CLine2d* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}
}

void imtqml2d::CQmlLineShape::SetPoint1(sdl::imtsdl2d::imt2d::CPoint2d newPoint1)
{
	BaseClass2::SetPoint1(newPoint1);
	i2d::CLine2d* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}

	objectPtr->SetPoint1(i2d::CVector2d(newPoint1.GetX(), newPoint1.GetY()));
}

void CQmlLineShape::SetPoint2(sdl::imtsdl2d::imt2d::CPoint2d newPoint2)
{
	BaseClass2::SetPoint2(newPoint2);
	i2d::CLine2d* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}
	
	objectPtr->SetPoint2(i2d::CVector2d(newPoint2.GetX(), newPoint2.GetY()));
}


} // namespace imtqml2d

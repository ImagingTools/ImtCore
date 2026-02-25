// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml2d/CQmlRectangleShape.h>



namespace imtqml2d
{


// reimplemented (imod::TSingleModelObserverBase)

bool CQmlRectangleShape::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool retVal = BaseClass::OnModelAttached(modelPtr, changeMask);

	i2d::CRectangle* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		BaseClass::EnsureModelDetached();
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return false;
	}

	return retVal;
}


bool CQmlRectangleShape::OnModelDetached(imod::IModel* modelPtr)
{
	bool retVal = BaseClass::OnModelDetached(modelPtr);

	return retVal;
}


void CQmlRectangleShape::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	i2d::CRectangle* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}
}

void CQmlRectangleShape::SetTopLeft(sdl::imtsdl2d::imt2d::CPoint2d newTopLeft)
{
	BaseClass2::SetTopLeft(newTopLeft);
	i2d::CRectangle* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}

	objectPtr->SetTopLeft(i2d::CVector2d(newTopLeft.GetX(),newTopLeft.GetY()));
}

void CQmlRectangleShape::SetBottomRight(sdl::imtsdl2d::imt2d::CPoint2d newBottomRight)
{
	BaseClass2::SetBottomRight(newBottomRight);
	i2d::CRectangle* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}

	objectPtr->SetBottomRight(i2d::CVector2d(newBottomRight.GetX(),newBottomRight.GetY()));
}


} // namespace imtqml2d

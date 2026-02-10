// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml2d/CQmlAnnulusShape.h>



namespace imtqml2d
{


// reimplemented (imod::IObserver)

bool CQmlAnnulusShape::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool retVal = BaseClass::OnModelAttached(modelPtr, changeMask);

	i2d::CAnnulus* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		BaseClass::EnsureModelDetached();
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return false;
	}

	return retVal;
}


bool CQmlAnnulusShape::OnModelDetached(imod::IModel* modelPtr)
{
	bool retVal = BaseClass::OnModelDetached(modelPtr);

	return retVal;
}


void CQmlAnnulusShape::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	i2d::CAnnulus* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}
}

void imtqml2d::CQmlAnnulusShape::SetCenter(sdl::imtsdl2d::imt2d::CPoint2d newCenter)
{
	BaseClass2::SetCenter(newCenter);
	i2d::CAnnulus* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}

	objectPtr->SetPosition(i2d::CVector2d(newCenter.GetX(), newCenter.GetY()));
}

void CQmlAnnulusShape::SetInnerRadius(double newInnerRadius)
{
	BaseClass2::SetInnerRadius(newInnerRadius);
	i2d::CAnnulus* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}

	objectPtr->SetInnerRadius(newInnerRadius);

}

void CQmlAnnulusShape::SetOuterRadius(double newOuterRadius)
{
	BaseClass2::SetOuterRadius(newOuterRadius);
	i2d::CAnnulus* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}

	objectPtr->SetOuterRadius(newOuterRadius);
}


} // namespace imtqml2d

// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml2d/CQmlPointShape.h>



namespace imtqml2d
{


// reimplemented (imod::IObserver)

bool CQmlPointShape::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool retVal = BaseClass::OnModelAttached(modelPtr, changeMask);

	i2d::CVector2d* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		BaseClass::EnsureModelDetached();
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return false;
	}

	return retVal;
}


bool CQmlPointShape::OnModelDetached(imod::IModel* modelPtr)
{
	bool retVal = BaseClass::OnModelDetached(modelPtr);

	return retVal;
}


void CQmlPointShape::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	i2d::CVector2d* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}
}

void imtqml2d::CQmlPointShape::SetX(double newX)
{
	BaseClass2::SetX(newX);
	i2d::CVector2d* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}

	objectPtr->SetX(newX);
}

void CQmlPointShape::SetY(double newY)
{
	BaseClass2::SetY(newY);
	i2d::CVector2d* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}
	
	objectPtr->SetY(newY);
}


} // namespace imtqml2d

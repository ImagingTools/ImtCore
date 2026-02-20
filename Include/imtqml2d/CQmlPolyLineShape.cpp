// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml2d/CQmlPolyLineShape.h>



namespace imtqml2d
{


// reimplemented (imod::IObserver)

bool CQmlPolyLineShape::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool retVal = BaseClass::OnModelAttached(modelPtr, changeMask);

	i2d::CPolyline* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		BaseClass::EnsureModelDetached();
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return false;
	}

	return retVal;
}


bool CQmlPolyLineShape::OnModelDetached(imod::IModel* modelPtr)
{
	bool retVal = BaseClass::OnModelDetached(modelPtr);

	return retVal;
}


void CQmlPolyLineShape::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	i2d::CPolyline* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}
}

void imtqml2d::CQmlPolyLineShape::SetPoints(const QList<sdl::imtsdl2d::imt2d::CPoint2d>& newPoints)
{
	BaseClass2::SetPoints(newPoints);
	i2d::CPolyline* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}
	
	objectPtr->SetNodesCount(newPoints.size());
	for (int i = 0; i < newPoints.size(); ++i){
		const sdl::imtsdl2d::imt2d::CPoint2d& currentPoint = newPoints[i];
		objectPtr->SetNodePos(i, i2d::CVector2d(currentPoint.GetX(), currentPoint.GetY()));
	}
}


} // namespace imtqml2d

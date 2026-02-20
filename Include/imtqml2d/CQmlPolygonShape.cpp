// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml2d/CQmlPolygonShape.h>



namespace imtqml2d
{


// reimplemented (imod::IObserver)

bool CQmlPolygonShape::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool retVal = BaseClass::OnModelAttached(modelPtr, changeMask);

	i2d::CPolygon* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		BaseClass::EnsureModelDetached();
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return false;
	}

	return retVal;
}


bool CQmlPolygonShape::OnModelDetached(imod::IModel* modelPtr)
{
	bool retVal = BaseClass::OnModelDetached(modelPtr);

	return retVal;
}


void CQmlPolygonShape::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	i2d::CPolygon* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}
}

void imtqml2d::CQmlPolygonShape::SetPoints(const QList<sdl::imtsdl2d::imt2d::CPoint2d>& newPoints)
{
	BaseClass2::SetPoints(newPoints);
	i2d::CPolygon* objectPtr = GetObjectPtr();
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

void CQmlPolygonShape::SetIsClosed(bool newIsClosed)
{
	BaseClass2::SetIsClosed(newIsClosed);
	i2d::CPolygon* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}
	
	// what todo??
}


} // namespace imtqml2d

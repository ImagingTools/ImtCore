#include <imt3dview/CScene3d.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imt3d/IObject3d.h>


namespace imt3dview
{


// public methods

CScene3d::CScene3d()
	:m_contextPtr(nullptr),
	m_nextModelId(0),
	m_cameraPtr(nullptr)
{
}


// reimplemented (IScene3d)

void CScene3d::SetCamera(IScene3dCamera* cameraPtr)
{
	m_cameraPtr = cameraPtr;

	for (Shapes::Iterator i = m_shapes.begin(); i != m_shapes.end(); ++i){
		const ShapeInfoPtr& shapeInfoPtr = i.value();

		if (shapeInfoPtr && shapeInfoPtr->shapePtr){
			shapeInfoPtr->shapePtr->SetCamera(cameraPtr);
		}
	}
}


void CScene3d::SetProjection(const QMatrix4x4& projection)
{
	for (Shapes::Iterator i = m_shapes.begin(); i != m_shapes.end(); ++i){
		const ShapeInfoPtr& shapeInfoPtr = i.value();

		if (shapeInfoPtr && shapeInfoPtr->shapePtr){
			shapeInfoPtr->shapePtr->SetProjection(projection);
		}
	}
}


void CScene3d::SetViewPort(const QRect& viewPort)
{
	for (Shapes::Iterator i = m_shapes.begin(); i != m_shapes.end(); ++i){
		const ShapeInfoPtr& shapeInfoPtr = i.value();

		if (shapeInfoPtr && shapeInfoPtr->shapePtr){
			shapeInfoPtr->shapePtr->SetViewPort(viewPort);
		}
	}
}


imt3d::CCuboid CScene3d::GetBoundingCuboid() const
{
	if (m_cameraPtr != nullptr) {
		return m_cameraPtr->GetBoundingCuboid();
	}

	return imt3d::CCuboid();
}


QByteArrayList CScene3d::GetShapeIds() const
{
	return m_shapes.keys();
}


IScene3dItem* CScene3d::GetShapePtr(const QByteArray& id) const
{
	Shapes::ConstIterator it = m_shapes.constFind(id);

	if (it != m_shapes.end() && it->get()){
		return it->get()->shapePtr;
	}

	return nullptr;
}


QByteArray CScene3d::AddShapeToScene(imt3dview::IScene3dItem* objectPtr, int itemFlags)
{
	if (!objectPtr){
		return QByteArray();
	}

	for (Shapes::Iterator i = m_shapes.begin(); i != m_shapes.end(); ++i){
		const ShapeInfoPtr& shapeInfoPtr = i.value();

		if (shapeInfoPtr && shapeInfoPtr->shapePtr == objectPtr){
			return i.key();
		}
	}

	ShapeInfoPtr newItemPtr(new ShapeInfo);
	newItemPtr->flags = itemFlags;
	newItemPtr->shapePtr = objectPtr;
	newItemPtr->modelId = m_nextModelId++;

	QByteArray newId = QUuid::createUuid().toByteArray();
	m_shapes.insert(newId, newItemPtr);

	objectPtr->SetContext(m_contextPtr);

	return newId;
}


void CScene3d::RemoveShapeFromScene(imt3dview::IScene3dItem* objectPtr)
{
	for (Shapes::Iterator i = m_shapes.begin(); i != m_shapes.end(); ++i){
		const ShapeInfoPtr& shapeInfoPtr = i.value();

		if (shapeInfoPtr && shapeInfoPtr->shapePtr == objectPtr){
			shapeInfoPtr->shapePtr->SetContext(nullptr);
			m_shapes.erase(i);

			return;
		}
	}
}


// reimplemented (imt3dview::IDrawable)

void CScene3d::SetContext(QOpenGLContext* contextPtr)
{
	for (Shapes::Iterator i = m_shapes.begin(); i != m_shapes.end(); ++i){
		const ShapeInfoPtr& shapeInfoPtr = i.value();

		if (shapeInfoPtr && shapeInfoPtr->shapePtr){
			shapeInfoPtr->shapePtr->SetContext(contextPtr);
		}
	}

	m_contextPtr = contextPtr;
}


void CScene3d::DrawGl(QOpenGLShaderProgram& program)
{
	UpdateBoundingCuboid();

	for (Shapes::Iterator i = m_shapes.begin(); i != m_shapes.end(); ++i){
		const ShapeInfoPtr& shapeInfoPtr = i.value();

		if (shapeInfoPtr && shapeInfoPtr->shapePtr){
			shapeInfoPtr->shapePtr->DrawGl(program);
		}
	}
}


void CScene3d::Draw(QPainter& painter)
{
	for (Shapes::Iterator i = m_shapes.begin(); i != m_shapes.end(); ++i){
		const ShapeInfoPtr& shapeInfoPtr = i.value();

		if (shapeInfoPtr && shapeInfoPtr->shapePtr){
			shapeInfoPtr->shapePtr->Draw(painter);
		}
	}
}


// private methods

void CScene3d::UpdateBoundingCuboid()
{
	for (Shapes::Iterator i = m_shapes.begin(); i != m_shapes.end(); ++i){
		const ShapeInfoPtr& shapeInfoPtr = i.value();

		if (shapeInfoPtr && shapeInfoPtr->shapePtr){
			UpdateBoundingCuboid(shapeInfoPtr->shapePtr);
		}
	}

	// bounding cuboid might have been changed, take care of items scales
	UpdateItemsScale();
}


void CScene3d::UpdateBoundingCuboid(const imt3dview::IScene3dItem* objectPtr)
{
	imt3d::CCuboid objectCuboid = GetItemBoundingCuboid(objectPtr);
	if (m_cameraPtr != nullptr) {
		m_cameraPtr->SetBoundingCuboid(objectCuboid);
	}
}


void CScene3d::UpdateItemsScale()
{
	for (Shapes::Iterator i = m_shapes.begin(); i != m_shapes.end(); ++i){
		const ShapeInfoPtr& shapeInfoPtr = i.value();

		if (shapeInfoPtr && shapeInfoPtr->shapePtr){
			imt3d::CCuboid cuboid = GetItemBoundingCuboid(shapeInfoPtr->shapePtr);
			if (cuboid.IsValid()){
				UpdateItemScale(*shapeInfoPtr->shapePtr);
			}
		}
	}
}


void CScene3d::UpdateItemScale(IScene3dItem& scene3dItem)
{
	imt3d::CCuboid sceneBoundingCuboid = GetBoundingCuboid();
	double maxBound = qAbs(sceneBoundingCuboid.GetTop());
	maxBound = qMax(maxBound, qAbs(sceneBoundingCuboid.GetBottom()));
	maxBound = qMax(maxBound, qAbs(sceneBoundingCuboid.GetLeft()));
	maxBound = qMax(maxBound, qAbs(sceneBoundingCuboid.GetRight()));
	maxBound = qMax(maxBound, qAbs(sceneBoundingCuboid.GetNear()));
	maxBound = qMax(maxBound, qAbs(sceneBoundingCuboid.GetFar()));

	if (!qFuzzyIsNull(maxBound)) {
		float scale = 1.0f / static_cast<float>(maxBound);
		scene3dItem.SetScale(scale);
	}
}


imt3d::CCuboid CScene3d::GetItemBoundingCuboid(const IScene3dItem* scene3dItemPtr)
{
	const imod::CSingleModelObserverBase* modelObserverPtr = dynamic_cast<const imod::CSingleModelObserverBase*>(scene3dItemPtr);

	if (modelObserverPtr){
		const imt3d::IObject3d* object3dPtr = dynamic_cast<const imt3d::IObject3d*>(modelObserverPtr->GetObservedModel());

		if (object3dPtr){
			return object3dPtr->GetBoundingCuboid();
		}
	}

	return imt3d::CCuboid();
}


} // namespace imt3dview



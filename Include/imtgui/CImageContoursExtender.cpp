#include <imtgui/CImageContoursExtender.h>


// ACF includes
#include <iedge/CEdgeLineContainer.h>
#include <iview/CPolylineShape.h>
#include <iqt2d/IViewProvider.h>
#include <iqt2d/IViewExtender.h>
#include <iview/IShapeView.h>
#include <iview/IViewLayer.h>
#include <iview/CSimpleShapeFactoryComp.h>


namespace imtgui
{


// public methods

// reimplemented (imtgui::IViewExtender)

bool CImageContoursExtender::AddItems(imod::IObserver* observerPtr, const istd::IChangeable* objectPtr)
{
	iqt2d::IViewProvider* viewProviderPtr = dynamic_cast<iqt2d::IViewProvider*>(observerPtr);
	const iedge::CEdgeLineContainer* edgeLineContainerPtr = dynamic_cast<const iedge::CEdgeLineContainer*>(objectPtr);

	if (viewProviderPtr == nullptr || edgeLineContainerPtr == nullptr || !m_shapeFactoryCompPtr.IsValid()){
		return false;
	}

	iview::IShapeView* shapeViewPtr = viewProviderPtr->GetView();
	if (shapeViewPtr == nullptr){
		return false;
	}

	for (int i = 0; i < edgeLineContainerPtr->GetItemsCount(); i++){
		const iedge::CEdgeLine& edgeLine = edgeLineContainerPtr->GetAt(i);

		m_shapeModels[observerPtr].append(istd::TSmartPtr<ShapeModel>(new ShapeModel()));
		edgeLine.CopyToPolyline(*m_shapeModels[observerPtr][i]);

		iview::IShape* shapePtr = m_shapeFactoryCompPtr->CreateShape(m_shapeModels[observerPtr][i].GetPtr(), true);
		if (shapePtr == nullptr){
			RemoveItems(observerPtr);
			return false;
		}

		shapeViewPtr->ConnectShape(shapePtr);
		m_shapes[observerPtr].append(shapePtr);
	}

	return true;
}


bool CImageContoursExtender::RemoveItems(imod::IObserver* observerPtr)
{
	if (!m_shapeModels.keys().contains(observerPtr)){
		return false;
	}

	iqt2d::IViewProvider* viewProviderPtr = dynamic_cast<iqt2d::IViewProvider*>(observerPtr);
	if (viewProviderPtr == nullptr){
		return false;
	}

	iview::IShapeView* shapeViewPtr = viewProviderPtr->GetView();
	if (shapeViewPtr == nullptr){
		return false;
	}

	for (iview::IShape* shapePtr : m_shapes[observerPtr]){
		shapeViewPtr->DisconnectShape(shapePtr);
		delete shapePtr;
	}

	m_shapes[observerPtr].clear();
	m_shapeModels[observerPtr].clear();

	m_shapes.remove(observerPtr);
	m_shapeModels.remove(observerPtr);

	return true;
}


// reimplemented (icomp::CComponentBase)

void CImageContoursExtender::OnComponentDestroyed()
{
	for (imod::IObserver* observerPtr : m_shapeModels.keys()){
		RemoveItems(observerPtr);
	}

	BaseClass::OnComponentDestroyed();
}


} // namespace imtgui



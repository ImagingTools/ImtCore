#include <imtloggui/CGroupViewComp.h>


// ImtCore includes
#include <imtbase/IObjectProvider.h>
#include <imtloggui/ILayerController.h>


namespace imtloggui
{


//public methods:

CGroupViewComp::CGroupViewComp()
	:m_representationViewUpdateBridge(this)
{
}


// reimplemented (imtloggui::IGraphicsItemProvider)

IGraphicsItemProvider::GraphicsItemList CGroupViewComp::GetItems() const
{
	if (m_representationViewUpdateBridge.GetModelCount() > 0){
		IGraphicsItemProvider* graphicsItemProviderPtr =
			dynamic_cast<IGraphicsItemProvider*>(m_representationViewUpdateBridge.GetObservedModel(0));

		if (graphicsItemProviderPtr != nullptr){
			return graphicsItemProviderPtr->GetItems();
		}
	}

	return IGraphicsItemProvider::GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CGroupViewComp::GetAddedItems() const
{
	if (m_representationViewUpdateBridge.GetModelCount() > 0){
		IGraphicsItemProvider* graphicsItemProviderPtr =
			dynamic_cast<IGraphicsItemProvider*>(m_representationViewUpdateBridge.GetObservedModel(0));

		if (graphicsItemProviderPtr != nullptr){
			return graphicsItemProviderPtr->GetAddedItems();
		}
	}

	return IGraphicsItemProvider::GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CGroupViewComp::GetRemovedItems() const
{
	if (m_representationViewUpdateBridge.GetModelCount() > 0){
		IGraphicsItemProvider* graphicsItemProviderPtr =
			dynamic_cast<IGraphicsItemProvider*>(m_representationViewUpdateBridge.GetObservedModel(0));

		if (graphicsItemProviderPtr != nullptr){
			return graphicsItemProviderPtr->GetRemovedItems();
		}
	}

	return IGraphicsItemProvider::GraphicsItemList();
}



// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CGroupViewComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_representationViewUpdateBridge.EnsureModelsDetached();

	const iprm::IOptionsList* optionListPtr = GetObservedObject()->GetSelectionConstraints();
	QByteArray activeLayerId = optionListPtr->GetOptionId(GetObservedObject()->GetSelectedOptionIndex());

	const ILayerController* layerControllerPtr = dynamic_cast<const ILayerController*>(GetObservedObject());
	if (layerControllerPtr != nullptr){
		QByteArray activeRepresentationId = layerControllerPtr->GetActiveRepresentationId();

		if (GetElementIds().contains(activeLayerId)){
			const imtbase::IObjectCollection* activeLayerViewPtr =
						dynamic_cast<const imtbase::IObjectCollection*>(BaseClass2::GetObjectPtr(activeLayerId));

			if (activeLayerViewPtr != nullptr){
				imod::IModel* modelPtr = const_cast<imod::IModel*>(
							dynamic_cast<const imod::IModel*>(activeLayerViewPtr->GetObjectPtr(activeRepresentationId)));

				if (modelPtr != nullptr){
					modelPtr->AttachObserver(&m_representationViewUpdateBridge);
				}
			}
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CGroupViewComp::OnComponentCreated()
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_layerViewsCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_layerViewsCompPtr[i]);
	}
}


} // namespace imtloggui



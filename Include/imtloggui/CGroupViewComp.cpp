#include <imtloggui/CGroupViewComp.h>


namespace imtloggui
{


// public methods:

CGroupViewComp::CGroupViewComp()
	:m_updateBridge(this)
{
}


// reimplemented (imtloggui::IGraphicsItemProvider)

IGraphicsItemProvider::GraphicsItemList CGroupViewComp::GetItems() const
{
	const imtloggui::IGraphicsItemProvider* layerViewPtr = GetActiveLayerView();

	if (layerViewPtr != nullptr){
		return layerViewPtr->GetItems();
	}

	return IGraphicsItemProvider::GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CGroupViewComp::GetAddedItems() const
{
	const imtloggui::IGraphicsItemProvider* layerViewPtr = GetActiveLayerView();

	if (layerViewPtr != nullptr){
		return layerViewPtr->GetAddedItems();
	}

	return IGraphicsItemProvider::GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CGroupViewComp::GetRemovedItems() const
{
	const imtloggui::IGraphicsItemProvider* layerViewPtr = GetActiveLayerView();

	if (layerViewPtr != nullptr){
		return layerViewPtr->GetRemovedItems();
	}

	return IGraphicsItemProvider::GraphicsItemList();
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CGroupViewComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_updateBridge.EnsureModelsDetached();

	imod::IModel* modelPtr = const_cast<imod::IModel*>(
				dynamic_cast<const imod::IModel*>(GetActiveLayerView()));

	if (modelPtr = nullptr){
		modelPtr->AttachObserver(&m_updateBridge);
	}
}


// reimplemented (icomp::CComponentBase)

void CGroupViewComp::OnComponentCreated()
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_layerViewsCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		Q_ASSERT(!GetElementIds().contains(m_idAttrPtr[i]));
		RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_layerViewsCompPtr[i]);
	}
}


// private methods

const IGraphicsItemProvider* CGroupViewComp::GetActiveLayerView() const
{
	if (IsModelAttached()){
		const iprm::IOptionsList* optionListPtr = GetObservedObject()->GetSelectionConstraints();
		QByteArray activeLayerId = optionListPtr->GetOptionId(GetObservedObject()->GetSelectedOptionIndex());

		if (GetElementIds().contains(activeLayerId)){
			return dynamic_cast<const IGraphicsItemProvider*>(BaseClass2::GetObjectPtr(activeLayerId));
		}
	}

	return nullptr;
}


} // namespace imtloggui



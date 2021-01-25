#include <imtloggui/CLayerViewComp.h>


namespace imtloggui
{


// public methods

CLayerViewComp::CLayerViewComp()
	:m_updateBridge(this)
{
}


// reimplemented (imtloggui::IGraphicsItemProvider)

IGraphicsItemProvider::GraphicsItemList CLayerViewComp::GetItems() const
{
	const imtloggui::IGraphicsItemProvider* representationViewPtr = GetActiveRepresentationView();

	if (representationViewPtr != nullptr){
		return representationViewPtr->GetItems();
	}

	return IGraphicsItemProvider::GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CLayerViewComp::GetAddedItems() const
{
	const imtloggui::IGraphicsItemProvider* representationViewPtr = GetActiveRepresentationView();

	if (representationViewPtr != nullptr){
		return representationViewPtr->GetAddedItems();
	}

	return IGraphicsItemProvider::GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CLayerViewComp::GetRemovedItems() const
{
	const imtloggui::IGraphicsItemProvider* representationViewPtr = GetActiveRepresentationView();

	if (representationViewPtr != nullptr){
		return representationViewPtr->GetRemovedItems();
	}

	return IGraphicsItemProvider::GraphicsItemList();
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CLayerViewComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_updateBridge.EnsureModelsDetached();

	imod::IModel* modelPtr = const_cast<imod::IModel*>(
				dynamic_cast<const imod::IModel*>(GetActiveRepresentationView()));

	if (modelPtr = nullptr){
		modelPtr->AttachObserver(&m_updateBridge);
	}
}


// reimplemented (icomp::CComponentBase)

void CLayerViewComp::OnComponentCreated()
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_representationViewCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_representationViewCompPtr[i]);
	}
}


// private methods

const IGraphicsItemProvider* CLayerViewComp::GetActiveRepresentationView() const
{
	if (IsModelAttached()){
		const iprm::IOptionsList* optionListPtr = GetObservedObject()->GetSelectionConstraints();
		QByteArray activeRepresentationViewId = optionListPtr->GetOptionId(GetObservedObject()->GetSelectedOptionIndex());

		if (GetElementIds().contains(activeRepresentationViewId)){
			return dynamic_cast<const IGraphicsItemProvider*>(BaseClass2::GetObjectPtr(activeRepresentationViewId));				
		}
	}

	return nullptr;
}


} // namespace imtloggui



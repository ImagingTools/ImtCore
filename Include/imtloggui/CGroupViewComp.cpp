#include <imtloggui/CGroupViewComp.h>


namespace imtloggui
{


//public methods:


// reimplemented (imtloggui::IGraphicsItemProvider)

IGraphicsItemProvider::GraphicsItemList CGroupViewComp::GetItems() const
{
	return GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CGroupViewComp::GetAddedItems() const
{
	return GraphicsItemList();
}


IGraphicsItemProvider::GraphicsItemList CGroupViewComp::GetRemovedItems() const
{
	return GraphicsItemList();
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CGroupViewComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
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



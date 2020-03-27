#include <imtgui/CCompositeObjectViewComp.h>


// Acf includes
#include <iwidgets/iwidgets.h>


namespace imtgui
{


// public methods

CCompositeObjectViewComp::CCompositeObjectViewComp()
	:m_commands("&View", 100)
{
	m_rootCommands.InsertChild(&m_commands);
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CCompositeObjectViewComp::GetCommands() const
{
	return &m_rootCommands;
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CCompositeObjectViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtbase::IObjectCollection* objectProviderPtr = GetObservedObject();
	Q_ASSERT(objectProviderPtr != nullptr);

	imtbase::IObjectCollection::Ids ids = objectProviderPtr->GetElementIds();

	if (ids.count() != m_objectIds.count()){
		DestroyView();
		CreateView();
		return;
	}

	for (const QByteArray &id : ids){
		if (!m_objectIds.contains(id)){
			DestroyView();
			CreateView();
			return;
		}
	}
}

void CCompositeObjectViewComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	CreateView();
}


void CCompositeObjectViewComp::OnGuiModelDetached()
{
	DestroyView();

	BaseClass::OnGuiModelDetached();
}

// reimplemented (iqtgui::TRestorableGuiWrap)

void CCompositeObjectViewComp::OnRestoreSettings(const QSettings& /*settings*/)
{
}


void CCompositeObjectViewComp::OnSaveSettings(QSettings& /*settings*/) const
{
}


// private methods

void CCompositeObjectViewComp::CreateView()
{
	imtbase::IObjectCollection* objectProviderPtr = GetObservedObject();
	Q_ASSERT(objectProviderPtr != nullptr);

	if (!m_objectViewFactoryListCompPtr.IsValid() || !m_objectTypeAttrPtr.IsValid()){
		return;
	}

	int factoryCount = qMin(m_objectViewFactoryListCompPtr.GetCount(), m_objectTypeAttrPtr.GetCount());

	imtbase::ICollectionInfo::Ids objectIds = objectProviderPtr->GetElementIds();

	for (const QByteArray& objectId : objectIds){
		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(const_cast<istd::IChangeable*>(objectProviderPtr->GetObjectPtr(objectId)));
		if (modelPtr == nullptr){
			continue;
		}

		QByteArray objectTypeId = objectProviderPtr->GetObjectTypeId(objectId);

		for (int i = 0; i < factoryCount; i++){
			if (objectTypeId == m_objectTypeAttrPtr[i]){
				icomp::IComponent* componentPtr = m_objectViewFactoryListCompPtr.CreateComponent(i);
				iqtgui::IGuiObject* viewPtr = dynamic_cast<iqtgui::IGuiObject*>(componentPtr);
				imod::IObserver* observerPtr = dynamic_cast<imod::IObserver*>(viewPtr);
				
				if (observerPtr == nullptr){
					if (componentPtr != nullptr){
						delete componentPtr;
					}

					break;
				}

				m_objectIds.append(objectId);

				QWidget* widgetPtr = new QWidget();
				QVBoxLayout* layoutPtr = new QVBoxLayout(widgetPtr);
				widgetPtr->setLayout(layoutPtr);
				GetQtWidget()->layout()->addWidget(widgetPtr);
				viewPtr->CreateGui(widgetPtr);
				
				modelPtr->AttachObserver(observerPtr);
			}
		}
	}
}


void CCompositeObjectViewComp::DestroyView()
{
	m_objectIds.clear();

	for (iqtgui::IGuiObject* viewPtr : m_objectViews){
		viewPtr->DestroyGui();
		delete viewPtr;
	}

	m_objectViews.clear();

	iwidgets::ClearLayout(GetQtWidget()->layout());
}


} // namespace imtgui



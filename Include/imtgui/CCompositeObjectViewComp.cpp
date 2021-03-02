#include <imtgui/CCompositeObjectViewComp.h>


// ACF includes
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


// private methods

void CCompositeObjectViewComp::CreateView()
{
	imtbase::IObjectCollection* objectProviderPtr = GetObservedObject();
	Q_ASSERT(objectProviderPtr != nullptr);

	if (!m_objectViewFactoryListCompPtr.IsValid() || !m_objectTypeAttrPtr.IsValid()){
		return;
	}

	imtbase::ICollectionInfo::Ids objectIds = objectProviderPtr->GetElementIds();
	for (const QByteArray& objectId : objectIds){
		m_objectIds.append(objectId);

		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(const_cast<istd::IChangeable*>(objectProviderPtr->GetObjectPtr(objectId)));
		if (modelPtr == nullptr){
			continue;
		}

		QByteArray objectTypeId = objectProviderPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_TYPE_ID).toByteArray();
		QString objectName = objectProviderPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

		int factoryCount = qMin(m_objectViewFactoryListCompPtr.GetCount(), m_objectTypeAttrPtr.GetCount());
		for (int factoryCounter = 0; factoryCounter < factoryCount; factoryCounter++){
			if (objectTypeId == m_objectTypeAttrPtr[factoryCounter]){
				icomp::IComponent* viewComponentPtr = m_objectViewFactoryListCompPtr.CreateComponent(factoryCounter);
				iqtgui::IGuiObject* viewGuiObjectPtr = dynamic_cast<iqtgui::IGuiObject*>(viewComponentPtr);
				imod::IObserver* viewObserverPtr = dynamic_cast<imod::IObserver*>(viewGuiObjectPtr);
				
				if (viewObserverPtr == nullptr){
					if (viewComponentPtr != nullptr){
						delete viewComponentPtr;
					}

					break;
				}

				m_views.append(viewGuiObjectPtr);

				QWidget* widgetPtr = new QWidget();
				QVBoxLayout* layoutPtr = new QVBoxLayout(widgetPtr);
				layoutPtr->setContentsMargins(0, 0, 0, 0);
				widgetPtr->setLayout(layoutPtr);
				GetQtWidget()->layout()->addWidget(widgetPtr);
				viewGuiObjectPtr->CreateGui(widgetPtr);
				
				modelPtr->AttachObserver(viewObserverPtr);

				if (!m_objectsToExtendAttrPtr.IsValid() || !m_objectsFromExtendAttrPtr.IsValid() || !m_viewExtendersCompPtr.IsValid()){
					break;
				}

				int extendersCount = qMin(m_objectsToExtendAttrPtr.GetCount(), m_objectsFromExtendAttrPtr.GetCount());
				extendersCount = qMin(extendersCount, m_viewExtendersCompPtr.GetCount());
				
				for (int extenderCounter = 0; extenderCounter < extendersCount; extenderCounter++){
					if (m_objectsToExtendAttrPtr[extenderCounter] == objectName){
						QString objectFromExtendName = m_objectsFromExtendAttrPtr[extenderCounter];
						const istd::IChangeable* objectFromExtendPtr = nullptr;

						for (QByteArray objectFromExtendId : objectIds){
							QString currentObjectName = objectProviderPtr->GetElementInfo(objectFromExtendId, imtbase::ICollectionInfo::EIT_NAME).toString();

							if (currentObjectName == objectFromExtendName){
								objectFromExtendPtr = objectProviderPtr->GetObjectPtr(objectFromExtendId);
								break;
							}
						}

						if (objectFromExtendPtr == nullptr){
							break;
						}

						icomp::IComponent* viewExtenderComponentPtr = m_viewExtendersCompPtr.CreateComponent(extenderCounter);
						imtgui::IViewExtender* viewExtenderPtr = dynamic_cast<imtgui::IViewExtender*>(viewExtenderComponentPtr);
						Q_ASSERT(viewExtenderPtr != nullptr);

						viewExtenderPtr->AddItems(viewObserverPtr, objectFromExtendPtr);
						m_viewExtenders.append(viewExtenderComponentPtr);

						break;
					}
				}

				break;
			}
		}
	}
}


void CCompositeObjectViewComp::DestroyView()
{
	m_objectIds.clear();

	for (icomp::IComponent* viewExtenderPtr : m_viewExtenders){
		delete viewExtenderPtr;
	}

	m_viewExtenders.clear();

	for (iqtgui::IGuiObject* viewPtr : m_views){
		viewPtr->DestroyGui();
		delete viewPtr;
	}

	m_views.clear();

	iwidgets::ClearLayout(GetQtWidget()->layout());
}


} // namespace imtgui



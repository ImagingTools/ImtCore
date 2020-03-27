#include <imtgui/CCompositeObjectViewComp.h>


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtWidgets/QLabel>

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

	//imtbase::IObjectCollection::Ids ids = objectProviderPtr->GetElementIds();

	//if (ids.count() != m_objectIds.count()){
	//	UnInitCompositeGui();
	//	InitCompositeGui();
	//	return;
	//}

	//for (const QByteArray &id : ids){
	//	if (!m_objectIds.contains(id)){
	//		UnInitCompositeGui();
	//		InitCompositeGui();
	//		return;
	//	}
	//}
}

void CCompositeObjectViewComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	InitCompositeGui();
}


void CCompositeObjectViewComp::OnGuiModelDetached()
{
	UnInitCompositeGui();

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

void CCompositeObjectViewComp::InitCompositeGui()
{
	imtbase::IObjectCollection* objectProviderPtr = GetObservedObject();
	Q_ASSERT(objectProviderPtr != nullptr);

	if (!m_objectViewFactoryListCompPtr.IsValid() || !m_objectTypeAttrPtr.IsValid()){
		return;
	}

	int factoryCount = qMin(m_objectViewFactoryListCompPtr.GetCount(), m_objectTypeAttrPtr.GetCount());

	imtbase::ICollectionInfo::Ids ids = objectProviderPtr->GetElementIds();

	for (const QByteArray& id : ids){
		QLabel *l = new QLabel();
		l->setText("123");
		GetQtWidget()->layout()->addWidget(l);

		//istd::IChangeable* objectPtr = const_cast<istd::IChangeable*>(objectProviderPtr->GetObjectPtr(id));
		//imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(objectPtr);

		//QByteArray typeId = objectProviderPtr->GetObjectTypeId(id);

		//for (int i = 0; i < factoryCount; i++){
		//	if (typeId == m_objectTypeAttrPtr[i]){
		//		icomp::IComponent* componentPtr = m_objectViewFactoryListCompPtr.CreateComponent(i);
		//		iqtgui::IGuiObject* viewPtr = dynamic_cast<iqtgui::IGuiObject*>(componentPtr);
		//		imod::IObserver* observerPtr = dynamic_cast<imod::IObserver*>(componentPtr);
		//		if (componentPtr == nullptr || viewPtr == nullptr || observerPtr == nullptr){
		//			break;
		//		}

		//		m_objectIds.append(id);

		//		QWidget* widgetPtr = new QWidget();
		//		GetQtWidget()->layout()->addWidget(widgetPtr);
		//		QLabel *l = new QLabel();
		//		l->setText("123");
		//		GetQtWidget()->layout()->addWidget(l);
		//		viewPtr->CreateGui(widgetPtr);
		//		
		//		modelPtr->AttachObserver(observerPtr);
		//	}
		//}
	}
}


void CCompositeObjectViewComp::UnInitCompositeGui()
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



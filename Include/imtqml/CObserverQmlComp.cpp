#include <imtqml/CObserverQmlComp.h>


namespace imtqml
{


CObserverQmlComp::CObserverQmlComp()
	:m_settingsObserver(*this)
{
}


// protected methods

void CObserverQmlComp::UpdateSettingsRepresentation()
{
	if (m_quickObjectCompPtr.IsValid() && m_settingsCompPtr.IsValid()){
		QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
		if (quickItem != nullptr){
			m_settingsModel.Clear();

			bool result = m_settingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(*m_settingsCompPtr, m_settingsModel);
			if (result){
				if (m_settingsModel.ContainsKey("Parameters")){
					imtbase::CTreeItemModel* parametersPtr = m_settingsModel.GetTreeItemModel("Parameters");
					if (parametersPtr != nullptr){
						QVariant data = QVariant::fromValue(parametersPtr);

						quickItem->setProperty("localSettings", data);
					}
				}
			}
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CObserverQmlComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_quickObjectCompPtr.IsValid()){
		QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
		if (quickItem != nullptr){
			connect(quickItem, SIGNAL(settingsUpdate()), this, SLOT(OnGuiChanged()));
		}
	}

	UpdateSettingsRepresentation();
}


void CObserverQmlComp::OnComponentDestroyed()
{
	m_settingsObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


void CObserverQmlComp::OnChangeSourceItem(QString src)
{
	if (m_quickObjectCompPtr.IsValid()){
		QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
		Q_ASSERT(quickItem != nullptr);

		quickItem->setProperty("sourceItem", "qrc:///qml/" + src);
	}
}


void CObserverQmlComp::OnGuiChanged()
{
	if (m_settingsRepresentationControllerCompPtr.IsValid() && m_settingsCompPtr.IsValid()){
		m_settingsRepresentationControllerCompPtr->GetDataModelFromRepresentation(m_settingsModel, *m_settingsCompPtr);
	}
}


} // namespace imtqml



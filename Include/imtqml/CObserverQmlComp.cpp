#include <imtqml/CObserverQmlComp.h>


// Qt includes
#include <QtQml/QQmlEngine>

// ACF includes
#include <iprm/ITextParam.h>


namespace imtqml
{


CObserverQmlComp::CObserverQmlComp()
	:m_settingsModelPtr(nullptr)
{
}


// protected methods

void CObserverQmlComp::ApplyUrl() const
{
	if (!m_quickObjectCompPtr.IsValid()){
		return;
	}

	QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
	Q_ASSERT(quickItem != nullptr);

	QQmlEngine* engine = qmlEngine(quickItem);
	Q_ASSERT(engine != nullptr);

	const istd::IChangeable* serverUrlDataPtr = m_settingsCompPtr->GetParameter("NetworkSettings/ServerUrl");
	if (serverUrlDataPtr != nullptr){
		const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(serverUrlDataPtr);
		if (textParamPtr != nullptr){
			QString text = textParamPtr->GetText();

			engine->setBaseUrl(text + *m_prefixServer);
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
			if (m_settingsRepresentationControllerCompPtr.IsValid() && m_settingsCompPtr.IsValid()){
				m_settingsModelPtr = new imtbase::CTreeItemModel();
				bool result = m_settingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(*m_settingsCompPtr, *m_settingsModelPtr);
				if (result){
					if (m_settingsModelPtr != nullptr){
						if (m_settingsModelPtr->ContainsKey("Parameters")){
							imtbase::CTreeItemModel* parametersPtr = m_settingsModelPtr->GetTreeItemModel("Parameters");
							if (parametersPtr != nullptr){
								QVariant data = QVariant::fromValue(parametersPtr);

								quickItem->setProperty("localSettings", data);
							}
						}
					}

					bool isConnected = connect(quickItem, SIGNAL(settingsUpdate()), this, SLOT(OnGuiChanged()));
					Q_ASSERT(isConnected);

					ApplyUrl();

					QMetaObject::invokeMethod(quickItem, "updateModels");
				}
			}
		}
	}
}


void CObserverQmlComp::OnComponentDestroyed()
{
	if (m_settingsModelPtr != nullptr){
		delete m_settingsModelPtr;

		m_settingsModelPtr = nullptr;
	}

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
		if (m_settingsModelPtr != nullptr){
			bool result = m_settingsRepresentationControllerCompPtr->GetDataModelFromRepresentation(*m_settingsModelPtr, *m_settingsCompPtr);
		}
	}
}


} // namespace imtqml



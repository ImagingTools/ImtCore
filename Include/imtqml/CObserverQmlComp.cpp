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

	const istd::IChangeable* serverUrl = m_settingsCompPtr->GetParameter("NetworkSettings/ServerUrl");
	if (serverUrl != nullptr){
		const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(serverUrl);
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
			if (m_representationControllerCompPtr.IsValid() && m_settingsCompPtr.IsValid()){
				m_settingsModelPtr = new imtbase::CTreeItemModel();
				bool result = m_representationControllerCompPtr->GetRepresentationFromDataModel(*m_settingsCompPtr, *m_settingsModelPtr);

				QString json = m_settingsModelPtr->toJSON();

				if (result){
					if (m_settingsModelPtr != nullptr){
						QVariant data = QVariant::fromValue(m_settingsModelPtr);

						quickItem->setProperty("localSettings", data);
					}

					connect(quickItem, SIGNAL(settingsUpdate(QString)), this, SLOT(OnModelChanged(QString)));

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


void CObserverQmlComp::OnModelChanged()
{
	if (m_representationControllerCompPtr.IsValid() && m_settingsCompPtr.IsValid()){
		if (m_settingsModelPtr != nullptr){
			bool result = m_representationControllerCompPtr->GetDataModelFromRepresentation(*m_settingsModelPtr, *m_settingsCompPtr);
		}
	}
}


} // namespace imtqml



#include <imtqml/CObserverQmlComp.h>


// Qt includes
#include <QtQml/QQmlEngine>

// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/ISelectionParam.h>
#include <iprm/TParamsPtr.h>


namespace imtqml
{


CObserverQmlComp::CObserverQmlComp()
	:m_settingsModelPtr(nullptr),
	m_applicationInfoModelPtr(nullptr),
	m_settingsObserver(*this),
	m_languageParamObserver(*this),
	m_urlParamObserver(*this)
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

	QUrl url;
	if (m_urlParamPtr.IsValid()){
		url = m_urlParamPtr->GetUrl();
	}

	QUrl serverUrl = url.toString() + *m_prefixServer;
	engine->setBaseUrl(serverUrl);

	quickItem->setProperty("serverReady", true);
}


void CObserverQmlComp::UpdateLanguage() const
{
	if (!m_settingsCompPtr.IsValid() || !m_translationManagerCompPtr.IsValid()){
		return;
	}

	QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
	Q_ASSERT(quickItem != nullptr);

	QQmlEngine* enginePtr = qmlEngine(quickItem);
	Q_ASSERT(enginePtr != nullptr);

	iprm::TParamsPtr<iprm::ISelectionParam> languageParamPtr(m_settingsCompPtr.GetPtr(), "General/Language");
	if (languageParamPtr.IsValid()){
		const iprm::IOptionsList* optionListPtr = languageParamPtr->GetSelectionConstraints();
		if (optionListPtr != nullptr){
			int index = languageParamPtr->GetSelectedOptionIndex();
			if (index >= 0){
				m_translationManagerCompPtr->SwitchLanguage(index);

				if (enginePtr != nullptr){
					enginePtr->retranslate();
					QString langId =  m_translationManagerCompPtr->GetLanguagesInfo().GetOptionId(index);

					SendInfoMessage(0, QString("Localization changed to %1").arg(langId));

					QMetaObject::invokeMethod(quickItem, "onLocalizationChanged", Qt::AutoConnection, Q_ARG(QVariant, langId));
				}
			}
		}
	}
}


void CObserverQmlComp::UpdateSettingsRepresentation()
{
	if (m_quickObjectCompPtr.IsValid() && m_settingsCompPtr.IsValid()){
		QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
		if (quickItem != nullptr){
			if (m_settingsModelPtr == nullptr){
				m_settingsModelPtr = new imtbase::CTreeItemModel();
			}
			else{
				m_settingsModelPtr->Clear();
			}

			bool result = m_settingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(*m_settingsCompPtr, *m_settingsModelPtr);
			if (result){
				if (m_settingsModelPtr->ContainsKey("Parameters")){
					imtbase::CTreeItemModel* parametersPtr = m_settingsModelPtr->GetTreeItemModel("Parameters");
					if (parametersPtr != nullptr){
						QVariant data = QVariant::fromValue(parametersPtr);

						quickItem->setProperty("localSettings", data);
					}
				}
			}
		}
	}
}


void CObserverQmlComp::UpdateApplicationInfoRepresentation()
{
	if (m_quickObjectCompPtr.IsValid() && m_settingsCompPtr.IsValid()){
		QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
		if (quickItem != nullptr){
			if (m_applicationInfoModelPtr != nullptr){
				delete m_applicationInfoModelPtr;
			}

			m_applicationInfoModelPtr = new imtbase::CTreeItemModel();

			if (m_applicationInfoCompPtr.IsValid() && m_applicationInfoRepresentationCompPtr.IsValid()){
				if (m_applicationInfoRepresentationCompPtr->GetRepresentationFromApplicationInfo(*m_applicationInfoCompPtr, *m_applicationInfoModelPtr)){
					QVariant data = QVariant::fromValue(m_applicationInfoModelPtr);

					quickItem->setProperty("applicationInfo", data);
				}
			}
		}
	}
}


void CObserverQmlComp::OnSettingsChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const iprm::IParamsSet* /*objectPtr*/)
{
}


void CObserverQmlComp::OnLanguageChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const iprm::ISelectionParam* /*objectPtr*/)
{
	UpdateApplicationInfoRepresentation();
	UpdateSettingsRepresentation();
}


void CObserverQmlComp::OnUrlParamChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IUrlParam* urlParamPtr)
{
	ApplyUrl();
}


// reimplemented (icomp::CComponentBase)

void CObserverQmlComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_quickObjectCompPtr.IsValid()){
		QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
		if (quickItem != nullptr){
			if (m_settingsCompPtr.IsValid()){
				m_settingsObserver.RegisterObject(m_settingsCompPtr.GetPtr(), &CObserverQmlComp::OnSettingsChanged);
			}

			if (m_languageParamPtr.IsValid()){
				m_languageParamObserver.RegisterObject(m_languageParamPtr.GetPtr(), &CObserverQmlComp::OnLanguageChanged);
			}

			if (m_urlParamPtr.IsValid()){
				m_urlParamObserver.RegisterObject(m_urlParamPtr.GetPtr(), &CObserverQmlComp::OnUrlParamChanged);
			}

			UpdateLanguage();
			UpdateApplicationInfoRepresentation();
			UpdateSettingsRepresentation();

			ApplyUrl();

			bool isConnected = connect(quickItem, SIGNAL(settingsUpdate()), this, SLOT(OnGuiChanged()));
			Q_ASSERT(isConnected);

			QMetaObject::invokeMethod(quickItem, "firstModelsInit");
		}
	}
}


void CObserverQmlComp::OnComponentDestroyed()
{
	if (m_settingsModelPtr != nullptr){
		delete m_settingsModelPtr;

		m_settingsModelPtr = nullptr;
	}

	if (m_applicationInfoModelPtr != nullptr){
		delete m_applicationInfoModelPtr;

		m_applicationInfoModelPtr = nullptr;
	}

	m_settingsObserver.UnregisterAllObjects();
	m_languageParamObserver.UnregisterAllObjects();
	m_urlParamObserver.UnregisterAllObjects();

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
			m_settingsRepresentationControllerCompPtr->GetDataModelFromRepresentation(*m_settingsModelPtr, *m_settingsCompPtr);
		}
	}
}


} // namespace imtqml



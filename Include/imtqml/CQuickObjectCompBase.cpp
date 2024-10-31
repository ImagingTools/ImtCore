#include <imtqml/CQuickObjectCompBase.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>


namespace imtqml
{


// public methods

CQuickObjectCompBase::CQuickObjectCompBase()
	:m_quickItemPtr(nullptr),
	m_urlParamObserver(*this)
{
}


QString CQuickObjectCompBase::GetPathToQml()
{
	return "QmlFilePath";
}


void CQuickObjectCompBase::OnUrlParamChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IUrlParam* urlParamPtr)
{
	if (!m_applicationInfoCompPtr.IsValid()){
		return;
	}

	if (urlParamPtr != nullptr){
		if (m_quickItemPtr != nullptr){
			QQmlEngine* qmlEnginePtr = qmlEngine(m_quickItemPtr);
			if (qmlEnginePtr != nullptr){
				SetBaseUrl(*qmlEnginePtr);
			}
		}
	}
}


// reimplemented (imtgui::IQuickObject)

bool CQuickObjectCompBase::IsItemCreated() const
{
	return (m_quickItemPtr != nullptr);
}


bool CQuickObjectCompBase::CreateQuickItem(QQuickItem* parentPtr)
{
	if (parentPtr == nullptr){
		return false;
	}

	if ((*m_pathToQmlAttrPtr).isEmpty()){
		return false;
	}

	if (m_quickItemPtr != nullptr){
		Q_ASSERT(false);

		return false;
	}

	QQmlEngine* engine = qmlEngine(parentPtr);
	if (engine != nullptr){
		SetBaseUrl(*engine);
		m_quickItemPtr = CreateItem(engine);
		if (m_quickItemPtr != nullptr){
			m_quickItemPtr->setParentItem(parentPtr);

			m_itemCreated.SetQuickItemCreated(true);

			OnItemCreated();

			return true;
		}
	}

	return false;
}


bool CQuickObjectCompBase::DestroyQuickItem()
{
	if (m_quickItemPtr != nullptr){
		OnItemDestroyed();

		m_quickItemPtr->deleteLater();

		m_quickItemPtr = nullptr;

		return true;
	}

	return false;
}


QQuickItem* CQuickObjectCompBase::GetQuickItem() const
{
	return m_quickItemPtr;
}


void CQuickObjectCompBase::SetBaseUrl(QQmlEngine& qmlEngine) const
{
	QString baseUrl;
	if (m_urlParamPtr.IsValid()){
		baseUrl = m_urlParamPtr->GetUrl().toString();
	}
	else if (m_baseUrlAttrPtr.IsValid()){
		baseUrl = *m_baseUrlAttrPtr;
	}

	QString appId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID);
	if (!appId.isEmpty() && !baseUrl.isEmpty()){
		baseUrl += "/" + appId;
	}

	qmlEngine.setBaseUrl(baseUrl);
}


void CQuickObjectCompBase::OnTryClose(bool* /*ignoredPtr*/)
{
}


// protected methods

QQuickItem* CQuickObjectCompBase::CreateItem(QQmlEngine* enginePtr) const
{
	if (enginePtr != nullptr){
		enginePtr->addImportPath("qrc:/qml");

		QUrl componentUrl("qrc" + m_pathToQmlAttrPtr->GetValue());
		QQmlComponent component(enginePtr, componentUrl);

		QQmlContext* contextPtr = enginePtr->rootContext();
		QObject* createdComponentPtr = component.create(contextPtr);
		QQuickItem* quickItemPtr = qobject_cast<QQuickItem*>(createdComponentPtr);

		if (component.isError()) {
			qWarning() << "UNABLE TO CREATE QML COMPONENT " << componentUrl;
			qWarning() << component.errors();
			qWarning() << "----------------";
		}

		return quickItemPtr;
	}

	return nullptr;
}


QQuickItem* CQuickObjectCompBase::CreateItem(QQmlEngine* enginePtr, const QVariantMap& initialProperties) const
{
	Q_UNUSED(initialProperties);

	if ((*m_pathToQmlAttrPtr).isEmpty()){
		return nullptr;
	}

	QQmlComponent component(enginePtr, QUrl("qrc" + m_pathToQmlAttrPtr->GetValue()));
#if QT_VERSION > QT_VERSION_CHECK(5, 14, 0)
	return qobject_cast<QQuickItem*>(component.createWithInitialProperties(initialProperties, enginePtr->rootContext()));
#endif
	return nullptr;
}


void CQuickObjectCompBase::OnItemCreated()
{
	for (int i = 0; i < m_modelIdsAttrPtr.GetCount(); ++i){
		QByteArray modelId = m_modelIdsAttrPtr[i];

		imtbase::CTreeItemModel* modelPtr = new imtbase::CTreeItemModel(this);

		connect(modelPtr, SIGNAL(needsReload()), this, SLOT(OnModelNeedsReload()));

		m_models.append(modelPtr);

		m_quickItemPtr->setProperty(modelId, QVariant::fromValue(modelPtr));
	}

	if (m_urlParamPtr.IsValid()){
		OnUrlParamChanged(istd::IChangeable::GetAnyChange(), m_urlParamPtr.GetPtr());
	}
}


void CQuickObjectCompBase::OnItemDestroyed()
{
	m_itemCreated.SetQuickItemCreated(false);
}


// reimplemented (icomp::CComponentBase)

void CQuickObjectCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_urlParamPtr.IsValid()){
		m_urlParamObserver.RegisterObject(m_urlParamPtr.GetPtr(), &CQuickObjectCompBase::OnUrlParamChanged);
	}
}


void CQuickObjectCompBase::OnComponentDestroyed()
{
	m_urlParamObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


void CQuickObjectCompBase::OnModelNeedsReload(imtbase::CTreeItemModel* /*itemModelPtr*/)
{
}


// private methods of the embedded class QuickItemCreated

CQuickObjectCompBase::QuickItemCreated::QuickItemCreated()
	:m_isCreated(false)
{
}


void CQuickObjectCompBase::QuickItemCreated::SetQuickItemCreated(bool isCreated)
{
	if (m_isCreated != isCreated){
		istd::CChangeNotifier changePtr(this);

		m_isCreated = isCreated;
	}
}


// reimplemented (iprm::IEnableableParam)

bool CQuickObjectCompBase::QuickItemCreated::IsEnabled() const
{
	return m_isCreated;
}


bool CQuickObjectCompBase::QuickItemCreated::IsEnablingAllowed() const
{
	return false;
}


bool CQuickObjectCompBase::QuickItemCreated::SetEnabled(bool /*isEnabled*/)
{
	return false;
}


// reimplemented (iser::ISerializable)

bool CQuickObjectCompBase::QuickItemCreated::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


} // namespace imtqml



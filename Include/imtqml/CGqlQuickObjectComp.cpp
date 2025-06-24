#include <imtqml/CGqlQuickObjectComp.h>


// Qt includes
#include <QtQml/QQmlEngine>


namespace imtqml
{


// public methods

CGqlQuickObjectComp::CGqlQuickObjectComp()
	:m_serverConnectionObserver(*this)
{
}


// reimplemented (CQuickObjectCompBase)

bool CGqlQuickObjectComp::InitializeEngine(QQmlEngine& qmlEngine) const
{
	SetBaseUrl(qmlEngine);

	return true;
}


void CGqlQuickObjectComp::OnItemCreated()
{
	m_itemCreated.SetQuickItemCreated(true);

	if (m_serverConnectionParamPtr.IsValid()){
		OnServerConnectionParamChanged(istd::IChangeable::GetAnyChange(), m_serverConnectionParamPtr.GetPtr());
	}
}


void CGqlQuickObjectComp::OnItemDestroyed()
{
	m_itemCreated.SetQuickItemCreated(false);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CGqlQuickObjectComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_serverConnectionParamPtr.IsValid()){
		m_serverConnectionObserver.RegisterObject(m_serverConnectionParamPtr.GetPtr(), &CGqlQuickObjectComp::OnServerConnectionParamChanged);
	}
}


void CGqlQuickObjectComp::OnComponentDestroyed()
{
	m_serverConnectionObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CGqlQuickObjectComp::OnServerConnectionParamChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtcom::IServerConnectionInterface* serverConnectionParamPtr)
{
	if (!m_applicationInfoCompPtr.IsValid()){
		return;
	}

	if (serverConnectionParamPtr != nullptr){
		if (m_quickItemPtr != nullptr){
			QQmlEngine* qmlEnginePtr = qmlEngine(m_quickItemPtr);
			if (qmlEnginePtr != nullptr){
				SetBaseUrl(*qmlEnginePtr);
			}
		}
	}
}

void CGqlQuickObjectComp::SetBaseUrl(QQmlEngine& qmlEngine) const
{
	QString baseUrl;
	if (m_serverConnectionParamPtr.IsValid()){
		QUrl url;
		if (m_serverConnectionParamPtr->GetUrl(imtcom::IServerConnectionInterface::PT_HTTP, url)){
			baseUrl = url.toString();
		}
	}

	if (baseUrl.isEmpty() && m_baseUrlAttrPtr.IsValid()){
		baseUrl = *m_baseUrlAttrPtr;
	}

	QString appId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID);
	if (!appId.isEmpty() && !baseUrl.isEmpty()){
		baseUrl += "/" + appId;
	}

	qmlEngine.setBaseUrl(baseUrl);
}


// private methods of the embedded class QuickItemCreated

CGqlQuickObjectComp::QuickItemCreated::QuickItemCreated()
	:m_isCreated(false)
{
}


void CGqlQuickObjectComp::QuickItemCreated::SetQuickItemCreated(bool isCreated)
{
	if (m_isCreated != isCreated){
		istd::CChangeNotifier changePtr(this);

		m_isCreated = isCreated;
	}
}


// reimplemented (iprm::IEnableableParam)

bool CGqlQuickObjectComp::QuickItemCreated::IsEnabled() const
{
	return m_isCreated;
}


bool CGqlQuickObjectComp::QuickItemCreated::IsEnablingAllowed() const
{
	return false;
}


bool CGqlQuickObjectComp::QuickItemCreated::SetEnabled(bool /*isEnabled*/)
{
	return false;
}


// reimplemented (iser::ISerializable)

bool CGqlQuickObjectComp::QuickItemCreated::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


} // namespace imtqml



#include <imtqml/CGqlQuickObjectComp.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>


namespace imtqml
{


// public methods

CGqlQuickObjectComp::CGqlQuickObjectComp()
	:m_urlParamObserver(*this)
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

	if (m_urlParamPtr.IsValid()){
		OnUrlParamChanged(istd::IChangeable::GetAnyChange(), m_urlParamPtr.GetPtr());
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

	if (m_urlParamPtr.IsValid()){
		m_urlParamObserver.RegisterObject(m_urlParamPtr.GetPtr(), &CGqlQuickObjectComp::OnUrlParamChanged);
	}
}


void CGqlQuickObjectComp::OnComponentDestroyed()
{
	m_urlParamObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CGqlQuickObjectComp::OnUrlParamChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtbase::IUrlParam* urlParamPtr)
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

void CGqlQuickObjectComp::SetBaseUrl(QQmlEngine& qmlEngine) const
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



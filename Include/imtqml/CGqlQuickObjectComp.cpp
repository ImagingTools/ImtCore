// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlQuickObjectComp.h>


// Qt includes
#include <QtQml/QQmlEngine>


namespace imtqml
{


// public methods

CGqlQuickObjectComp::CGqlQuickObjectComp()
	:m_serverConnectionObserver(*this),
	m_applicationInfoObserver(*this)
{
}


// reimplemented (CQuickObjectCompBase)

bool CGqlQuickObjectComp::InitializeEngine(QQmlEngine& /*qmlEngine*/) const
{
	SetBaseUrl();

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

	if (m_applicationInfoCompPtr.IsValid()){
		m_applicationInfoObserver.RegisterObject(m_applicationInfoCompPtr.GetPtr(), &CGqlQuickObjectComp::OnApplicationInfoChanged);
	}
}


void CGqlQuickObjectComp::OnComponentDestroyed()
{
	m_serverConnectionObserver.UnregisterAllObjects();
	m_applicationInfoObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CGqlQuickObjectComp::OnServerConnectionParamChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtcom::IServerConnectionInterface* /*serverConnectionParamPtr*/)
{
	SetBaseUrl();
}


void CGqlQuickObjectComp::OnApplicationInfoChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtbase::IApplicationInfoController* /*applicationInfoControllerPtr*/)
{
	SetBaseUrl();
}


void CGqlQuickObjectComp::SetBaseUrl() const 
{
	if (m_quickItemPtr == nullptr){
		return;
	}

	QQmlEngine* qmlEnginePtr = qmlEngine(m_quickItemPtr);
	if (qmlEnginePtr == nullptr){
		return;
	}

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

	QByteArray suffix;
	if (m_applicationInfoCompPtr.IsValid()){
		suffix = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	if (!suffix.isEmpty() && !baseUrl.isEmpty()){
		baseUrl += "/" + suffix;
	}

	qmlEnginePtr->setBaseUrl(baseUrl);
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



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgui/CAdministrationObserverQmlComp.h>


namespace imtauthgui
{


CAdministrationObserverQmlComp::CAdministrationObserverQmlComp()
	:m_loginObserver(*this),
	m_quickItemCreatedObserver(*this),
	m_serverConnectionParamObserver(*this)
{
}


// reimpemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CAdministrationObserverQmlComp::GetCommands() const
{
	return &m_commandsList;
}


// reimplemented (icomp::CComponentBase)

void CAdministrationObserverQmlComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_quickItemCreatedModelCompPtr.EnsureInitialized();

	if (m_loginCompPtr.IsValid()){
		m_loginObserver.RegisterObject(m_loginCompPtr.GetPtr(), &CAdministrationObserverQmlComp::OnLoginUpdate);
	}

	if (m_quickItemCreatedCompPtr.IsValid()){
		m_quickItemCreatedObserver.RegisterObject(m_quickItemCreatedCompPtr.GetPtr(), &CAdministrationObserverQmlComp::OnQuickItemCreatedUpdate);
	}

	if (m_serverConnectionParamCompPtr.IsValid()){
		m_serverConnectionParamObserver.RegisterObject(m_serverConnectionParamCompPtr.GetPtr(), &CAdministrationObserverQmlComp::OnServerConnectionParamUpdate);
	}
}


void CAdministrationObserverQmlComp::OnComponentDestroyed()
{
	m_loginObserver.UnregisterAllObjects();
	m_quickItemCreatedObserver.UnregisterAllObjects();
	m_serverConnectionParamObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private slots

void CAdministrationObserverQmlComp::OnCommandsModelChanged(QVariant value)
{
	imtbase::CTreeItemModel* commandsModelPtr = nullptr;
	if (value.isValid()){
		commandsModelPtr = value.value<imtbase::CTreeItemModel*>();
	}

	if (commandsModelPtr == nullptr){
		return;
	}

	istd::CChangeNotifier notifier(this);

	m_commandsList.ResetChilds();

	for (int i = 0; i < commandsModelPtr->GetItemsCount(); i++){
		auto commandPtr = new iqtgui::CHierarchicalCommand("", 100, ibase::ICommand::CF_TOOLBAR);

		if (commandsModelPtr->ContainsKey("Id", i)){
			QByteArray commandId = commandsModelPtr->GetData("Id", i).toByteArray();

			commandPtr->SetCommandId(commandId);
		}

		if (commandsModelPtr->ContainsKey("Name", i)){
			QString commandName = commandsModelPtr->GetData("Name", i).toString();

			commandPtr->SetName(commandName);
		}

		if (commandsModelPtr->ContainsKey("IsEnabled", i)){
			bool isEnabled = commandsModelPtr->GetData("IsEnabled", i).toBool();

			commandPtr->SetEnabled(isEnabled);
		}

		if (commandsModelPtr->ContainsKey("Visible", i)){
			bool isVisible = commandsModelPtr->GetData("Visible", i).toBool();

			commandPtr->setVisible(isVisible);
		}

		if (commandsModelPtr->ContainsKey("Icon", i)){
			QByteArray iconPath = commandsModelPtr->GetData("Icon", i).toByteArray();

			commandPtr->setIcon(QIcon(GetIconPath(":/" + iconPath)));
		}

		connect(commandPtr, SIGNAL(triggered()), SLOT(OnTriggered()), Qt::QueuedConnection);

		m_commandsList.InsertChild(commandPtr);
	}
}


void CAdministrationObserverQmlComp::OnTriggered()
{
	auto commandPtr = dynamic_cast<iqtgui::CHierarchicalCommand*>(sender());
	if (commandPtr != nullptr){
		QByteArray commandId = commandPtr->GetCommandId();

		if (m_quickObjectCompPtr.IsValid()){
			QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
			if (quickItem != nullptr){
				QMetaObject::invokeMethod(quickItem, "onCommandActivated", Q_ARG(QVariant, QVariant::fromValue(commandId)));
			}
		}
	}
}

// private methods

void CAdministrationObserverQmlComp::OnLoginUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/, const iauth::ILogin* objectPtr)
{
	if (objectPtr != nullptr){
		if (m_quickObjectCompPtr.IsValid()){
			QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
			if (quickItem != nullptr){
				iauth::CUser* loggedUserPtr = objectPtr->GetLoggedUser();
				if (loggedUserPtr == nullptr){
					QMetaObject::invokeMethod(quickItem, "logout");
				}
				else{
					QString username = loggedUserPtr->GetUserName();
					QString password = loggedUserPtr->GetPassword();

					QMetaObject::invokeMethod(	quickItem,
												"login",
												Q_ARG(QVariant, QVariant::fromValue(username)),
												Q_ARG(QVariant, QVariant::fromValue(password)));
				}
			}
		}
	}
}


void CAdministrationObserverQmlComp::OnQuickItemCreatedUpdate(const istd::IChangeable::ChangeSet& changeSet, const iprm::IEnableableParam* /*objectPtr*/)
{
	if (m_quickObjectCompPtr.IsValid()){
		QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
		if (quickItem != nullptr){
			OnLoginUpdate(changeSet, m_loginCompPtr.GetPtr());

			QByteArray productId;
			if (m_productIdAttrPtr.IsValid()){
				productId = *m_productIdAttrPtr;
			}

			if (!productId.isEmpty()){
				quickItem->setProperty("productId", *m_productIdAttrPtr);
			}

			bool isConnected = connect(quickItem, SIGNAL(commandsModelChanged(QVariant)), this, SLOT(OnCommandsModelChanged(QVariant)));
			Q_ASSERT(isConnected);

			if (m_serverConnectionParamCompPtr.IsValid()){
				QUrl url;
				if (m_serverConnectionParamCompPtr->GetUrl(imtcom::IServerConnectionInterface::PT_WEBSOCKET, url)){
					quickItem->setProperty("webSocketUrl", url.toString());
				}
			}
		}
	}
}


void CAdministrationObserverQmlComp::OnServerConnectionParamUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtcom::IServerConnectionInterface* objectPtr)
{
	if (objectPtr == nullptr){
		return;
	}

	if (!m_quickObjectCompPtr.IsValid()){
		return;
	}

	QQuickItem* quickItemPtr = m_quickObjectCompPtr->GetQuickItem();
	if (quickItemPtr == nullptr){
		return;
	}

	QUrl url;
	if (objectPtr->GetUrl(imtcom::IServerConnectionInterface::PT_WEBSOCKET, url)){
		quickItemPtr->setProperty("webSocketUrl", url.toString());
	}
}


} // namespace imtauthgui



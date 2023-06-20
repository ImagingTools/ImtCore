#include <imtqml/CCommandsObserverQmlComp.h>


namespace imtqml
{


// public methods

CCommandsObserverQmlComp::CCommandsObserverQmlComp()
{
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CCommandsObserverQmlComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_quickItemCreatedModelCompPtr.IsValid()){
		m_quickItemCreatedModelCompPtr->AttachObserver(this);
	}
}


// reimpemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CCommandsObserverQmlComp::GetCommands() const
{
	return &m_commandsList;
}


// reimplemented (iser::ISerializable)

bool CCommandsObserverQmlComp::Serialize(iser::IArchive& archive)
{
	return true;
}


// reimplemented (imod::TSingleModelObserverBase)

void CCommandsObserverQmlComp::OnUpdate(const ChangeSet& /*changeSet*/)
{
	if (m_quickObjectCompPtr.IsValid()){
		QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
		if (quickItem != nullptr){
			bool isConnected = connect(quickItem, SIGNAL(commandsModelChanged(QVariant)), this, SLOT(OnCommandsModelChanged(QVariant)));
			Q_ASSERT(isConnected);
		}
	}
}


// private slots

void CCommandsObserverQmlComp::OnCommandsModelChanged(QVariant value)
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
		iqtgui::CHierarchicalCommand* commandPtr = new iqtgui::CHierarchicalCommand("", 100, ibase::ICommand::CF_TOOLBAR);

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

			commandPtr->setIcon(QIcon(":/Icons/" + iconPath));
		}

		connect(commandPtr, SIGNAL(triggered()), SLOT(OnTriggered()), Qt::QueuedConnection);

		m_commandsList.InsertChild(commandPtr);
	}
}

void CCommandsObserverQmlComp::OnTriggered()
{
	iqtgui::CHierarchicalCommand* commandPtr = dynamic_cast<iqtgui::CHierarchicalCommand*>(sender());
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


} // namespace imtqml



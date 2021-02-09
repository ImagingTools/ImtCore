#include <imtgui/CPluginStatusViewerComp.h>


namespace imtgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CPluginStatusViewerComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	Table->clearContents();

	imtbase::IPluginMonitor* pluginMonitorPtr = GetObservedObject();
	Q_ASSERT(pluginMonitorPtr != nullptr);

	const imtbase::ICollectionInfo& collectionInfo = pluginMonitorPtr->GetPluginList();

	imtbase::ICollectionInfo::Ids ids = collectionInfo.GetElementIds();
	for (QByteArray id : ids){
		const istd::IInformationProvider* statusPtr = pluginMonitorPtr->GetPluginStatus(id);
		if (statusPtr != nullptr){
			QString name = collectionInfo.GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();
			QString description = collectionInfo.GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

			Table->insertRow(Table->rowCount());
			int rowIndex = Table->rowCount() - 1;

			// Timestamp
			Table->setItem(rowIndex, 0, new QTableWidgetItem(statusPtr->GetInformationTimeStamp().toString("dd.MM.yyyy hh:mm:ss")));

			// Status
			switch (statusPtr->GetInformationCategory()){
			case istd::IInformationProvider::IC_NONE:
				Table->setItem(rowIndex, 1, new QTableWidgetItem(QIcon(":/Icons/StateUnknown"), ""));
				break;
			case istd::IInformationProvider::IC_INFO:
				Table->setItem(rowIndex, 1, new QTableWidgetItem(QIcon(":/Icons/StateOk"), ""));
				break;
			case istd::IInformationProvider::IC_WARNING:
				Table->setItem(rowIndex, 1, new QTableWidgetItem(QIcon(":/Icons/StateWarning"), ""));
				break;
			case istd::IInformationProvider::IC_ERROR:
				Table->setItem(rowIndex, 1, new QTableWidgetItem(QIcon(":/Icons/Error"), ""));
				break;
			case istd::IInformationProvider::IC_CRITICAL:
				Table->setItem(rowIndex, 1, new QTableWidgetItem(QIcon(":/Icons/StateInvalid"), ""));
				break;
			}

			// Name
			Table->setItem(rowIndex, 2, new QTableWidgetItem(name));

			// Path
			Table->setItem(rowIndex, 3, new QTableWidgetItem(statusPtr->GetInformationSource()));

			// Error
			Table->setItem(rowIndex, 4, new QTableWidgetItem(description));
		}
	}
}


} // namespace imtgui



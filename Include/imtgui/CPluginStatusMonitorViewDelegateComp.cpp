#include <imtgui/CPluginStatusMonitorViewDelegateComp.h>


// ACF includes
#include <istd/IInformationProvider.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/IPluginStatusMonitor.h>


namespace imtgui
{


CPluginStatusMonitorViewDelegateComp::CPluginStatusMonitorViewDelegateComp()
	:m_collectionPtr(nullptr)
{
}


// reimplemented (ICollectionViewDelegate)

bool CPluginStatusMonitorViewDelegateComp::InitializeDelegate(imtbase::IObjectCollection* collectionPtr, iqtgui::IGuiObject* parentGuiPtr)
{
	m_collectionPtr = collectionPtr;
	m_parentGuiPtr = parentGuiPtr;

	if (m_collectionPtr != nullptr){
		OnLanguageChanged();

		return true;
	}

	return false;
}


const iqtgui::IVisualStatus& CPluginStatusMonitorViewDelegateComp::GetDocumentTypeStatus() const
{
	return m_visualStatus;
}


QByteArray CPluginStatusMonitorViewDelegateComp::GetSupportedTypeId() const
{
	static QByteArray retVal("PluginStatus");

	return retVal;
}


QAbstractItemDelegate* CPluginStatusMonitorViewDelegateComp::GetColumnItemDelegate(const QByteArray& /*columnId*/) const
{
	return nullptr;
}


void CPluginStatusMonitorViewDelegateComp::OnColumnItemDelegateEditFinished(const QByteArray &/*objectId*/, const QByteArray &/*columnId*/, const QVariant &/*newValue*/) const
{

}


bool CPluginStatusMonitorViewDelegateComp::IsEditorEnabled(const QByteArray &/*columnId*/) const
{
	return false;
}


void CPluginStatusMonitorViewDelegateComp::UpdateItemSelection(
			const imtbase::ICollectionInfo::Ids& /*selectedItems*/,
			const QByteArray& /*selectedTypeId*/)
{
}


QByteArray CPluginStatusMonitorViewDelegateComp::CreateNewObject(
			const QByteArray& /*typeId*/,
			const QString& /*objectName*/,
			const QString& /*description*/,
			const istd::IChangeable* /*defaultDataPtr*/) const
{
	return QByteArray();
}


QByteArray CPluginStatusMonitorViewDelegateComp::ImportObject(const QByteArray& /*typeId*/, const QString& /*sourcePath*/) const
{
	return QByteArray();
}


bool CPluginStatusMonitorViewDelegateComp::ExportObject(const QByteArray& /*objectId*/, const QString& /*targetPath*/) const
{
	return false;
}


void CPluginStatusMonitorViewDelegateComp::RemoveObjects(const imtbase::ICollectionInfo::Ids& /*objectIds*/)  const
{
}


bool CPluginStatusMonitorViewDelegateComp::UpdateObject(const QByteArray& /*objectId*/, const istd::IChangeable& /*contents*/) const
{
	return false;
}


bool CPluginStatusMonitorViewDelegateComp::RenameObject(const QByteArray& /*objectId*/, const QString& /*newName*/) const
{
	return false;
}


const imtbase::ICollectionInfo& CPluginStatusMonitorViewDelegateComp::GetSummaryInformationTypes() const
{
	return m_summaryInformationTypes;
}


ICollectionViewDelegate::SummaryInformation CPluginStatusMonitorViewDelegateComp::GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const
{
	SummaryInformation result;

	static QIcon none(":/Icons/StateUnknown");
	static QIcon ok(":/Icons/StateOk");
	static QIcon warning(":/Icons/StateWarning");
	static QIcon error(":/Icons/Error");
	static QIcon critical(":/Icons/StateInvalid");

	if (m_collectionPtr != nullptr){
		istd::IInformationProvider* informationProviderPtr = dynamic_cast<istd::IInformationProvider*>(
			const_cast<istd::IChangeable*>(m_collectionPtr->GetObjectPtr(objectId)));

		imtbase::IPluginStatusMonitor* pluginStatusMonitorPtr = dynamic_cast<imtbase::IPluginStatusMonitor*>(m_collectionPtr);

		if (informationProviderPtr != nullptr && pluginStatusMonitorPtr != nullptr){
			if (informationId == QByteArray("LoadedAt")){
				result.text = informationProviderPtr->GetInformationTimeStamp().toString("dd.MM.yyyy hh:mm:ss");
				result.sortValue = informationProviderPtr->GetInformationTimeStamp();
			}
			else if (informationId == QByteArray("Name")){
				result.text = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
				result.sortValue = result.text;
				switch (informationProviderPtr->GetInformationCategory()){
				case istd::IInformationProvider::IC_NONE:
					result.icon = none;
					break;
				case istd::IInformationProvider::IC_INFO:
					result.icon = ok;
					break;
				case istd::IInformationProvider::IC_WARNING:
					result.icon = warning;
					break;
				case istd::IInformationProvider::IC_ERROR:
					result.icon = error;
					break;
				case istd::IInformationProvider::IC_CRITICAL:
					result.icon = critical;
					break;
				}
			}
			else if (informationId == QByteArray("TypeId")){
				result.text = pluginStatusMonitorPtr->GetPluginTypeId(objectId);
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("Path")){
				result.text = informationProviderPtr->GetInformationSource();
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("StatusMessage")){
				result.text = informationProviderPtr->GetInformationDescription();
				result.sortValue = result.text;
			}
		}
	}

	return result;
}


ICollectionViewDelegate::HeaderInfo CPluginStatusMonitorViewDelegateComp::GetSummaryInformationHeaderInfo(const QByteArray& informationId) const
{
	if (m_collectionPtr != nullptr){
		if (m_summaryInformationHeaders.contains(informationId)){
			return m_summaryInformationHeaders[informationId];
		}
	}

	return ICollectionViewDelegate::HeaderInfo(false);
}


bool CPluginStatusMonitorViewDelegateComp::OpenDocumentEditor(const QByteArray& /*objectId*/, const QByteArray& /*viewTypeId*/) const
{
	return false;
}


iqtgui::IGuiObject* CPluginStatusMonitorViewDelegateComp::GetInformationView() const
{
	return nullptr;
}


bool CPluginStatusMonitorViewDelegateComp::IsCommandSupported(int /*commandId*/) const
{
	return false;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CPluginStatusMonitorViewDelegateComp::GetCommands() const
{
	return nullptr;
}


// protected methods

void CPluginStatusMonitorViewDelegateComp::SetupSummaryInformation()
{
	m_summaryInformationTypes.ResetData();
	m_summaryInformationHeaders.clear();

	if (*m_enabledNameAttrPtr){
		m_summaryInformationTypes.InsertItem("Name", tr("Name"), "");
		m_summaryInformationHeaders["Name"] = HeaderInfo(false);
	}

	if (*m_enabledPathAttrPtr){
		m_summaryInformationTypes.InsertItem("Path", tr("Path"), "");
		m_summaryInformationHeaders["Path"] = HeaderInfo(false);
	}

	if (*m_enabledLoadedAtAttrPtr){
		m_summaryInformationTypes.InsertItem("LoadedAt", tr("Loaded at"), "");
		m_summaryInformationHeaders["LoadedAt"] = HeaderInfo(false);
	}

	if (*m_enabledMessageAttrPtr){
		m_summaryInformationTypes.InsertItem("StatusMessage", tr("Message"), "");
		m_summaryInformationHeaders["StatusMessage"] = HeaderInfo(false);
	}

	if (*m_enabledTypeIdAttrPtr){
		m_summaryInformationTypes.InsertItem("TypeId", tr("Type"), "");
		m_summaryInformationHeaders["TypeId"] = HeaderInfo(false);
	}
}


// reimplemented (ibase::TLocalizableWrap)

void CPluginStatusMonitorViewDelegateComp::OnLanguageChanged()
{
	SetupSummaryInformation();
}


// reimplemented (icomp::CComponentBase)

void CPluginStatusMonitorViewDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetupSummaryInformation();
}


// private methods of the embedded class VisualStatus

// reimplemented (IVisualStatus)

void CPluginStatusMonitorViewDelegateComp::VisualStatus::SetStatusIcon(const QIcon &statusIcon)
{
	istd::CChangeNotifier changeNotifier(this);

	m_statusIcon = statusIcon;
}


void CPluginStatusMonitorViewDelegateComp::VisualStatus::SetStatusText(const QString &statusText)
{
	if (m_statusText != statusText){
		istd::CChangeNotifier changeNotifier(this);

		m_statusText = statusText;
	}
}


QIcon CPluginStatusMonitorViewDelegateComp::VisualStatus::GetStatusIcon() const
{
	return m_statusIcon;
}


QString CPluginStatusMonitorViewDelegateComp::VisualStatus::GetStatusText() const
{
	return m_statusText;
}


} // namespace imtgui



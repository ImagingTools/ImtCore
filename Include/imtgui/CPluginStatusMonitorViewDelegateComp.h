#pragma once


// ACF includes
#include <iqtgui/IGuiObject.h>
#include <ibase/TLocalizableWrap.h>
#include <imod/TModelWrap.h>
#include <icomp/CComponentBase.h>
#include <imtbase/CCollectionInfo.h>

// ImtCore includes
#include <imtgui/ICollectionViewDelegate.h>


namespace imtgui
{


/**
	Standard view delegate for an object collection view.
	\ingroup Collection
*/
class CPluginStatusMonitorViewDelegateComp:
			public QObject,
			public icomp::CComponentBase,
			public ibase::TLocalizableWrap<ICollectionViewDelegate>
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;
	typedef ibase::TLocalizableWrap<ICollectionViewDelegate> BaseClass2;

	I_BEGIN_COMPONENT(CPluginStatusMonitorViewDelegateComp);
		I_REGISTER_INTERFACE(imtgui::ICollectionViewDelegate);
	I_END_COMPONENT;

	CPluginStatusMonitorViewDelegateComp();

	// reimplemented (ICollectionViewDelegate)
	virtual bool InitializeDelegate(imtbase::IObjectCollection* collectionPtr, iqtgui::IGuiObject* parentGuiPtr) override;
	virtual const iqtgui::IVisualStatus& GetDocumentTypeStatus() const override;
	virtual QByteArray GetSupportedTypeId() const override;
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;
	virtual QByteArray CreateNewObject(const QByteArray& typeId, const istd::IChangeable* defaultDataPtr = nullptr) const override;
	virtual QByteArray ImportObject(const QByteArray& typeId, const QString& sourcePath = QString()) const override;
	virtual bool ExportObject(const QByteArray& objectId, const QString& targetPath = QString()) const override;
	virtual void RemoveObjects(const imtbase::ICollectionInfo::Ids& objectIds)  const override;
	virtual bool UpdateObject(const QByteArray& objectId, const istd::IChangeable& contents) const override;
	virtual bool RenameObject(const QByteArray& objectId, const QString& newName) const override;
	virtual const imtbase::ICollectionInfo& GetSummaryInformationTypes() const override;
	virtual SummaryInformation GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const override;
	virtual HeaderInfo GetSummaryInformationHeaderInfo(const QByteArray& informationId) const override;
	virtual bool OpenDocumentEditor(const QByteArray& objectId, const QByteArray& viewTypeId = QByteArray()) const override;
	virtual iqtgui::IGuiObject* GetInformationView() const override;

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

	// reimplemented (imtgui::ICollectionViewDelegate)
	virtual bool IsCommandSupported(int commandId) const override;

protected:
	class VisualStatus: virtual public iqtgui::IVisualStatus
	{
	public:
		void SetStatusIcon(const QIcon &statusIcon);
		void SetStatusText(const QString &statusText);

	private:
		QIcon m_statusIcon;
		QString m_statusText;

		// reimplemented (IVisualStatus)
		virtual QIcon GetStatusIcon() const override;
		virtual QString GetStatusText() const override;
	};

protected:
	imtbase::IObjectCollection* m_collectionPtr;
	iqtgui::IGuiObject* m_parentGuiPtr;

	imtbase::ICollectionInfo::Ids m_selectedItemIds;
	QByteArray m_selectedTypeId;

	imtbase::CCollectionInfo m_summaryInformationTypes;
	QMap<QByteArray, HeaderInfo> m_summaryInformationHeaders;

	imod::TModelWrap<VisualStatus> m_visualStatus;
};


} // namespace imtgui



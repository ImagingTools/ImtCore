#pragma once


// Qt includes
#include <QtWidgets/QMenu>

// ACF includes
#include <istd/TDelPtr.h>
#include <imod/TModelWrap.h>
#include <ibase/TLocalizableWrap.h>
#include <imtbase/CCollectionInfo.h>
#include <iqtgui/IGuiObject.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtgui/ICollectionViewDelegate.h>


namespace imtgui
{


/**
	Standard view delegate for an object collection view.
	\ingroup Collection
*/
class CObjectCollectionViewDelegate: public QObject, public ibase::TLocalizableWrap<ICollectionViewDelegate>
{
	Q_OBJECT
public:
	typedef ibase::TLocalizableWrap<ICollectionViewDelegate> BaseClass;

	enum CommandGroup
	{
		CG_EDIT = 2050
	};

	enum CommandIdentifier
	{
		CI_EDIT = BaseClass::CI_USER,
		CI_INSERT,
		CI_DUPLICATE,
		CI_REMOVE,
		CI_RENAME,
		CI_EDIT_DESCRIPTION,

		CI_USER = BaseClass::CI_USER + 100
	};

	CObjectCollectionViewDelegate();

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
	virtual bool IsCommandSupported(int commandId) const override;

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	virtual void SetupSummaryInformation();
	virtual void SetupCommands();
	virtual void SetupInsertCommand();
	virtual bool IsNameUnique(const QString& name);
	virtual void OnDuplicateObject(const QByteArray& sourceObjectId, const QByteArray& destinationObjectId);

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

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

protected Q_SLOTS:
	virtual void OnInsert();
	virtual void OnDuplicate();
	virtual void OnRemove();
	virtual void OnAddMenuOptionClicked(QAction* action);

protected:
	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;

	iqtgui::CHierarchicalCommand m_editCommands;
	iqtgui::CHierarchicalCommand m_insertCommand;
	iqtgui::CHierarchicalCommand m_duplicateCommand;
	iqtgui::CHierarchicalCommand m_removeCommand;

	istd::TDelPtr<QMenu> m_insertNewDocumentMenuPtr;

	imtbase::IObjectCollection* m_collectionPtr;
	iqtgui::IGuiObject* m_parentGuiPtr;

	imtbase::ICollectionInfo::Ids m_selectedItemIds;
	QByteArray m_selectedTypeId;

	imtbase::CCollectionInfo m_summaryInformationTypes;
	QMap<QByteArray, HeaderInfo> m_summaryInformationHeaders;

	imod::TModelWrap<VisualStatus> m_visualStatus;
};


} // namespace imtgui



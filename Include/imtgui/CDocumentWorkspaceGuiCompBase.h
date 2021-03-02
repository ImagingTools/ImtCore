#pragma once


// Qt includes
#include <QtWidgets/QShortcut>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>
#include <idoc/CMultiDocumentManagerBase.h>
#include <idoc/IDocumentTemplate.h>
#include <ibase/ICommandsProvider.h>
#include <iqtgui/IVisualStatus.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtdoc/TQtDocumentManagerWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>
#include <imtgui/IDocumentViewDecorator.h>
#include <GeneratedFiles/imtgui/ui_CDocumentWorkspaceGuiCompBase.h>


namespace imtgui
{



class CDocumentWorkspaceGuiCompBase:
			public iqtdoc::TQtDocumentManagerWrap<
						idoc::CMultiDocumentManagerBase,
						iqtgui::TRestorableGuiWrap< iqtgui::TDesignerGuiCompBase<Ui::CDocumentWorkspaceGuiCompBase> > >,
			protected imod::CMultiModelDispatcherBase
{
	Q_OBJECT

public:
	typedef iqtdoc::TQtDocumentManagerWrap<
				idoc::CMultiDocumentManagerBase,
				iqtgui::TRestorableGuiWrap< iqtgui::TDesignerGuiCompBase<Ui::CDocumentWorkspaceGuiCompBase> > > BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_BASE_COMPONENT(CDocumentWorkspaceGuiCompBase);
		I_REGISTER_INTERFACE(idoc::IDocumentManager);
		I_REGISTER_INTERFACE(idoc::IDocumentTypesInfo);
		I_REGISTER_SUBELEMENT(DocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, iprm::ISelectionParam, ExtractDocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, imod::IModel, ExtractDocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, istd::IChangeable, ExtractDocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, iprm::IOptionsList, ExtractDocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, imtbase::ICollectionInfo, ExtractDocumentList);
		I_REGISTER_SUBELEMENT(CurrentDocumentName);
		I_REGISTER_SUBELEMENT_INTERFACE(CurrentDocumentName, iprm::INameParam, ExtractCurrentDocumentName);
		I_REGISTER_SUBELEMENT_INTERFACE(CurrentDocumentName, imod::IModel, ExtractCurrentDocumentName);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_ASSIGN(m_documentTemplateCompPtr, "DocumentTemplate", "Document template", true, "DocumentTemplate");
		I_ASSIGN(m_tabsPositionAttrPtr, "TabsPosition", "Tabs position inside workspace (N, E, S, W)", true, "N");
		I_ASSIGN_MULTI_0(m_fixedTabsCompPtr, "FixedViews", "List of fixed views", false);
		I_ASSIGN_MULTI_0(m_fixedTabsNamesAttrPtr, "FixedViewNames", "List of names for the fixed views", false);
		I_ASSIGN_MULTI_0(m_fixedVisualInfosCompPtr, "FixedVisualInfos", "List of additional visual infos related to the fixed views", false);
	I_END_COMPONENT;

	enum ModelIds
	{
		MI_VISUAL_STATUS_BASE_INDEX = 1000,
		MI_DOCUMENT_COMMANDS_BASE_INDEX = 10000
	};

	CDocumentWorkspaceGuiCompBase();

	// reimplemented (iqtgui::IGuiObject)
	virtual void OnTryClose(bool* ignoredPtr = nullptr) override;

	// reimplemented (idoc::IDocumentManager)
	virtual void SetActiveView(istd::IPolymorphic* viewPtr) override;

protected:
	int GetFixedWindowsCount() const;
	void UpdateAllTitles();
	int GetDocumentIndexFromWidget(const QWidget& widget) const;
	virtual void InitializeDocumentView(IDocumentViewDecorator* pageViewPtr, const SingleDocumentData& documentData);

protected:
	virtual bool AddTab(const QString& name, iqtgui::IGuiObject* guiPtr, const QIcon& icon = QIcon());
	virtual void OnDragEnterEvent(QDragEnterEvent* dragEnterEventPtr);
	virtual void OnDropEvent(QDropEvent* dropEventPtr);

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (idoc::CMultiDocumentManagerBase)
	virtual istd::IChangeable* OpenSingleDocument(
				const QString& filePath,
				bool createView,
				const QByteArray& viewTypeId,
				QByteArray& documentTypeId,
				bool beQuiet,
				bool* ignoredPtr,
				ibase::IProgressManager* progressManagerPtr) override;

	// reimplemented (idoc::IDocumentManager)
	virtual bool InsertNewDocument(
				const QByteArray& documentTypeId,
				bool createView = true,
				const QByteArray& viewTypeId = "",
				istd::IChangeable** newDocumentPtr = NULL,
				bool beQuiet = false,
				bool* ignoredPtr = NULL) override;

	// reimplemented (idoc::CMultiDocumentManagerBase)
	virtual void CloseAllDocuments() override;
	virtual QStringList GetOpenFilePaths(const QByteArray* documentTypeIdPtr = NULL) const override;
	virtual void OnViewRegistered(istd::IPolymorphic* viewPtr, const SingleDocumentData& documentData) override;
	virtual void OnViewRemoved(istd::IPolymorphic* viewPtr) override;
	virtual bool QueryDocumentSave(const SingleDocumentData& info, bool* ignoredPtr) override;

	// reimplemented (iqt:CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnRetranslate() override;
	virtual void OnGuiRetranslate() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (istd:IChangeable)
	virtual void OnEndChanges(const ChangeSet& changeSet) override;

	// reimplemented (QObject)
	virtual bool eventFilter(QObject* sourcePtr, QEvent* eventPtr) override;

	// abstract methods
	virtual IDocumentViewDecorator* CreateDocumentViewDecorator(
				istd::IPolymorphic* viewPtr,
				QWidget* parentWidgetPtr,
				const SingleDocumentData& documentData,
				const ifile::IFilePersistence* persistencePtr) = 0;

protected Q_SLOTS:
	virtual void OnCloseDocument();
	virtual void OnUndoDocument();
	virtual void OnRedoDocument();
	virtual void OnWindowActivated(int index);
	virtual void OnCloseAllViews();
	virtual void OnNewDocument(const QByteArray& documentTypeId);
	virtual void OnOpenDocument(const QByteArray& documentTypeId);
	void OnTabCloseRequested(int index);
	void OnCloseCurrentTabShortcut();
	virtual void UpdateCommands();

Q_SIGNALS:
	void PostUpdateCommands();

private:
	class DocumentList:
				virtual public iprm::ISelectionParam,
				virtual public iprm::IOptionsList,
				virtual public imtbase::IObjectCollectionInfo
	{
	public:
		DocumentList();

		void SetParent(CDocumentWorkspaceGuiCompBase& parent);

		// reimplemented (iprm::ISelectionParam)
		virtual const iprm::IOptionsList* GetSelectionConstraints() const;
		virtual int GetSelectedOptionIndex() const;
		virtual bool SetSelectedOptionIndex(int index);
		virtual iprm::ISelectionParam* GetSubselection(int index) const;

		// reimplemented (iprm::IOptionsList)
		virtual int GetOptionsFlags() const;
		virtual int GetOptionsCount() const;
		virtual QString GetOptionName(int index) const;
		virtual QString GetOptionDescription(int index) const;
		virtual QByteArray GetOptionId(int index) const;
		virtual bool IsOptionEnabled(int index) const;

		// reimplemented (imtbase::IObjectCollectionInfo)
		virtual bool GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const override;
		virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
		virtual Id GetObjectTypeId(const QByteArray& objectId) const override;

		// reimplemented (imtbase::ICollectionInfo)
		virtual Ids GetElementIds() const override;
		virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

		// reimplemented (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive);

	private:
		int m_selectedDocumentIndex;
		CDocumentWorkspaceGuiCompBase* m_parent;
	};

	class Commands: virtual public ibase::ICommandsProvider
	{
	public:
		Commands();

		void SetParent(CDocumentWorkspaceGuiCompBase* parentPtr);

	protected:
		// reimplemented (ibase::ICommandsProvider)
		virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	private:
		CDocumentWorkspaceGuiCompBase* m_parentPtr;
	};


	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractDocumentList(CDocumentWorkspaceGuiCompBase& component)
	{
		return &component.m_documentList;
	}

	template <class InterfaceType>
	static InterfaceType* ExtractCurrentDocumentName(CDocumentWorkspaceGuiCompBase& component)
	{
		return &component.m_currentDocumentName;
	}

	template <typename InterfaceType>
	static InterfaceType* ExtractCommands(CDocumentWorkspaceGuiCompBase& component)
	{
		return &component.m_commands;
	}

private:
	imod::TModelWrap<DocumentList> m_documentList;
	imod::TModelWrap<iprm::CNameParam> m_currentDocumentName;
	imod::TModelWrap<Commands> m_commands;

	bool m_forceQuietClose;
	bool m_isUpdateBlocked;

	QShortcut *m_closeCurrentTabShortcutPtr;

	typedef QList<iqtgui::IGuiObject*> TabList;
	TabList m_fixedTabs;
	int m_previousTabIndex;

	I_REF(idoc::IDocumentTemplate, m_documentTemplateCompPtr);
	I_ATTR(QString, m_tabsPositionAttrPtr);
	I_MULTIREF(iqtgui::IGuiObject, m_fixedTabsCompPtr);
	I_MULTIREF(iqtgui::IVisualStatus, m_fixedVisualInfosCompPtr);
	I_MULTITEXTATTR(m_fixedTabsNamesAttrPtr);
};


} // namespace imtgui



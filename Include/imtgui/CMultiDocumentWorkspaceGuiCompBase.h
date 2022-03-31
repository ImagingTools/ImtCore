#pragma once


// Qt includes
#include <QtWidgets/QShortcut>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <ibase/ICommandsProvider.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtbase/CMultiDocumentManager.h>
#include <imtgui/TDocumentWorkspaceGuiCompBase.h>
#include <GeneratedFiles/imtgui/ui_CMultiDocumentWorkspaceGuiCompBase.h>


namespace imtgui
{



class CMultiDocumentWorkspaceGuiCompBase:
			public imtgui::TDocumentWorkspaceGuiCompBase<
						imtbase::CMultiDocumentManager,
						iqtgui::TRestorableGuiWrap<iqtgui::TDesignerGuiCompBase<Ui::CMultiDocumentWorkspaceGuiCompBase>>>,
			protected imod::CMultiModelDispatcherBase
{
	Q_OBJECT

public:
	typedef imtgui::TDocumentWorkspaceGuiCompBase<
				imtbase::CMultiDocumentManager,
				iqtgui::TRestorableGuiWrap<iqtgui::TDesignerGuiCompBase<Ui::CMultiDocumentWorkspaceGuiCompBase>>> BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_BASE_COMPONENT(CMultiDocumentWorkspaceGuiCompBase);
		I_REGISTER_SUBELEMENT(DocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, iprm::ISelectionParam, ExtractDocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, imod::IModel, ExtractDocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, istd::IChangeable, ExtractDocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, iprm::IOptionsList, ExtractDocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, imtbase::ICollectionInfo, ExtractDocumentList);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_ASSIGN(m_tabsPositionAttrPtr, "TabsPosition", "Tabs position inside workspace (0 - North, 1 - South, 2 - West, 3 - East)", true, 0);
		I_ASSIGN_MULTI_0(m_fixedTabsCompPtr, "FixedViews", "List of fixed views", false);
		I_ASSIGN_MULTI_0(m_fixedTabsNamesAttrPtr, "FixedViewNames", "List of names for the fixed views", false);
		I_ASSIGN_MULTI_0(m_fixedVisualInfosCompPtr, "FixedVisualInfos", "List of additional visual infos related to the fixed views", false);
	I_END_COMPONENT;

	enum ModelIds
	{
		MI_VISUAL_STATUS_BASE_INDEX = 1000,
		MI_DOCUMENT_COMMANDS_BASE_INDEX = 10000
	};

	CMultiDocumentWorkspaceGuiCompBase();

	// reimplemented (idoc::IDocumentManager)
	virtual void SetActiveView(istd::IPolymorphic* viewPtr) override;

protected:
	int GetFixedWindowsCount() const;

	// reimplemented (TDocumentWorkspaceGuiCompBase)
	virtual void UpdateAllTitles() override;
	virtual void InitializeDocumentView(IDocumentViewDecorator* pageViewPtr, const SingleDocumentData& documentData) override;

protected:
	virtual bool AddTab(const QString& name, iqtgui::IGuiObject* guiPtr, const QIcon& icon = QIcon());

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (idoc::CMultiDocumentManagerBase)
	virtual void CloseAllDocuments() override;
	virtual void OnViewRegistered(istd::IPolymorphic* viewPtr, const SingleDocumentData& documentData) override;
	virtual void OnViewRemoved(istd::IPolymorphic* viewPtr) override;

	// reimplemented (iqt:CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;

	// reimplemented (istd:IChangeable)
	virtual void OnEndChanges(const ChangeSet& changeSet) override;

protected Q_SLOTS:
	virtual void UpdateCommands();
	void OnNew();
	void OnOpen();
	void OnSaveDocumentAs();
	virtual void OnCloseDocument();
	virtual void OnUndo();
	virtual void OnRedo();
	virtual void OnViewActivated(int index);
	virtual void OnNewDocument(const QByteArray& documentTypeId);
	virtual void OnOpenDocument(const QByteArray& documentTypeId);
	void OnViewCloseTriggered(int index);
	void OnCurrentViewCloseTriggered();

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

		void SetParent(CMultiDocumentWorkspaceGuiCompBase& parent);

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
		virtual int GetElementsCount(const iprm::IParamsSet* selectionParamPtr = nullptr) const override;
		virtual Ids GetElementIds(
					int offset = 0,
					int count = -1,
					const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
		virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

		// reimplemented (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive);

	private:
		int m_selectedIndex;
		CMultiDocumentWorkspaceGuiCompBase* m_parentPtr;
	};

	class Commands: virtual public ibase::ICommandsProvider
	{
	public:
		Commands();

		void SetParent(CMultiDocumentWorkspaceGuiCompBase* parentPtr);

	protected:
		// reimplemented (ibase::ICommandsProvider)
		virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	private:
		CMultiDocumentWorkspaceGuiCompBase* m_parentPtr;
	};


	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractDocumentList(CMultiDocumentWorkspaceGuiCompBase& component)
	{
		return &component.m_documentList;
	}

	template <typename InterfaceType>
	static InterfaceType* ExtractCommands(CMultiDocumentWorkspaceGuiCompBase& component)
	{
		return &component.m_commands;
	}

private:
	imod::TModelWrap<DocumentList> m_documentList;
	imod::TModelWrap<Commands> m_commands;

	bool m_forceQuietClose;
	bool m_isUpdateBlocked;

	QShortcut *m_closeCurrentTabShortcutPtr;

	typedef QList<iqtgui::IGuiObject*> TabList;
	TabList m_fixedTabs;
	int m_previousTabIndex;

	I_REF(idoc::IDocumentTemplate, m_documentTemplateCompPtr);
	I_ATTR(int, m_tabsPositionAttrPtr);
	I_MULTIREF(iqtgui::IGuiObject, m_fixedTabsCompPtr);
	I_MULTIREF(iqtgui::IVisualStatus, m_fixedVisualInfosCompPtr);
	I_MULTITEXTATTR(m_fixedTabsNamesAttrPtr);
};


} // namespace imtgui



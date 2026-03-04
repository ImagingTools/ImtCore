// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QSortFilterProxyModel>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QMenu>
#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
#include <QtWidgets/QShortcut>
#else
#include <QtGui/QShortcut>
#endif

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <ibase/ICommandsProvider.h>
#include <iprm/IOptionsList.h>
#include <iprm/ISelectionParam.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iser/IArchive.h>
#include <iwidgets/CFocusDecorator.h>

// ImtCore includes
#include <imtbase/ISelection.h>
#include <imtbase/CCollectionFilter.h>
#include <imtbase/IComplexCollectionFilter.h>
#include <imtbase/CComplexCollectionFilterHelper.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtcol/CObjectTypeIdFilter.h>
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <imtgui/CCommandToolBar.h>
#include <GeneratedFiles/imtgui/ui_CObjectCollectionViewComp.h>


namespace imtgui
{


class CObjectCollectionViewAttr: public iqtgui::TRestorableGuiWrap<
			iqtgui::TDesignerGuiObserverCompBase<Ui::CObjectCollectionViewComp, imtbase::IObjectCollection>>
{
public:
	typedef iqtgui::TRestorableGuiWrap<
				iqtgui::TDesignerGuiObserverCompBase<Ui::CObjectCollectionViewComp, imtbase::IObjectCollection>> BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectCollectionViewAttr);
		I_ASSIGN(m_columnSettingsKeyAttrPtr, "ColumnSettingsKey", "Registry/INI file key for storing column settings", true, "");
		I_ASSIGN(m_useSearchWidgetAttrPtr, "UseSearchWidget", "Use internal search widget", true, true);
		I_ASSIGN(m_useCloseButtonAttrPtr, "UseCloseButton", "Add close button to the right", true, false);
		I_ASSIGN(m_viewRightPanelAttrPtr, "ViewRightPanel", "View right panel", true, true);
		I_ASSIGN(m_viewFilterAttrPtr, "ViewFilter", "View filter for default", true, false);
		I_ASSIGN(m_updateOnChangesAttrPtr, "UpdateOnChanges", "Update the view on changes of the observed collection", true, false);
	I_END_COMPONENT;

protected:
	I_ATTR(QByteArray, m_columnSettingsKeyAttrPtr);
	I_ATTR(bool, m_useSearchWidgetAttrPtr);
	I_ATTR(bool, m_viewRightPanelAttrPtr);
	I_ATTR(bool, m_viewFilterAttrPtr);
	I_ATTR(bool, m_useCloseButtonAttrPtr);
	I_ATTR(bool, m_updateOnChangesAttrPtr);

	QList<bool> m_enabledStates;
};


class CObjectCollectionViewComp:
		public CObjectCollectionViewAttr,
		public imod::CMultiModelDispatcherBase,
		virtual public imtbase::ISelection
{
	Q_OBJECT
public:
	typedef CObjectCollectionViewAttr BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CObjectCollectionViewComp);
		I_REGISTER_INTERFACE(imtbase::ISelection);
		I_REGISTER_SUBELEMENT(PageSelection);
		I_REGISTER_SUBELEMENT_INTERFACE(PageSelection, iprm::ISelectionParam, ExtractPageSelection);
		I_REGISTER_SUBELEMENT_INTERFACE(PageSelection, imod::IModel, ExtractPageSelection);
		I_REGISTER_SUBELEMENT_INTERFACE(PageSelection, istd::IChangeable, ExtractPageSelection);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_ASSIGN(m_leftToolBarCommandGroupIdAttrPtr, "LeftToolBarCommandGroupId", 
			R"(Commands with the specified group ID will be placed in the left part of the toolbar

NOTE
(*) This attribute belongs to a group of logically related attributes(LeftToolBarCommandGroupId, CenterToolBarCommandGroupId, RightToolBarCommandGroupId)
(*) If all attributes are less than 0, then the entire set of commands is placed in the left part of the toolbar
(*) If at least one attribute is greater than or equal to 0, then commands with the specified group id are placed in the corresponding part of the toolbar
    If any of the attributes has a group id value less than 0, the corresponding part of the toolbar will remain empty)"
			, true, -1);
		I_ASSIGN(m_centerToolBarCommandGroupIdAttrPtr, "CenterToolBarCommandGroupId",
			R"(Commands with the specified group ID will be placed in the center of the toolbar

NOTE
(*) This attribute belongs to a group of logically related attributes(LeftToolBarCommandGroupId, CenterToolBarCommandGroupId, RightToolBarCommandGroupId)
(*) If all attributes are less than 0, then the entire set of commands is placed in the left part of the toolbar
(*) If at least one attribute is greater than or equal to 0, then commands with the specified group id are placed in the corresponding part of the toolbar
	If any of the attributes has a group id value less than 0, the corresponding part of the toolbar will remain empty)"
			, true, -1);
		I_ASSIGN(m_rightToolBarCommandGroupIdAttrPtr, "RightToolBarCommandGroupId",
			R"(Commands with the specified group ID will be placed in the right part of the toolbar

NOTE
(*) This attribute belongs to a group of logically related attributes(LeftToolBarCommandGroupId, CenterToolBarCommandGroupId, RightToolBarCommandGroupId)
(*) If all attributes are less than 0, then the entire set of commands is placed in the left part of the toolbar
(*) If at least one attribute is greater than or equal to 0, then commands with the specified group id are placed in the corresponding part of the toolbar
	If any of the attributes has a group id value less than 0, the corresponding part of the toolbar will remain empty)"
			, true, -1);
		I_ASSIGN_MULTI_0(m_viewDelegatesCompPtr, "ViewDelegates", "List of view delegates (corresponding with the object type) used for the collection", false);
		I_ASSIGN(m_filterParamsGuiCompPtr, "FilterParamsGui", "UI for the additional filtering parameters for the collection", false, "FilterParamsGui");
		I_ASSIGN_TO(m_filterParamsObserverCompPtr, m_filterParamsGuiCompPtr, false);
		I_ASSIGN(m_filterSelectionCompPtr, "FilterSelection", "Filter selection", false, "FilterSelection");
		I_ASSIGN(m_paginationGuiCompPtr, "PaginationGui", "Pagination gui", false, "PaginationGui");
		I_ASSIGN_TO(m_paginationGuiObserverCompPtr, m_paginationGuiCompPtr, false);
		I_ASSIGN(m_filterParamsCompPtr, "FilteringParams", "Parameter using for the filterering the table", false, "FilteringParams");
		I_ASSIGN_TO(m_filterParamsModelCompPtr, m_filterParamsCompPtr, false);
		I_ASSIGN(m_filterEditPlaceholderTextAttrPtr, "FilterEditPlaceholderText", "Filter text box placeholder text", false, "");
	I_END_COMPONENT;

	enum ModelId
	{
		MI_DOCUMENT_TYPE_VISUAL_STATUS_BASE = 0,
		MI_LAST = MI_DOCUMENT_TYPE_VISUAL_STATUS_BASE + 1000
	};

	CObjectCollectionViewComp();

	void SetFilterString(const QString& text);

	// reimplemented (imtbase::ISelection)
	virtual const imtbase::ICollectionInfo* GetSelectionConstraints() const override;
	virtual SelectionMode GetSelectionMode() const override;
	virtual bool SetSelectionMode(SelectionMode mode) override;
	virtual Ids GetSelectedIds() const override;
	virtual bool SetSelectedIds(const Ids& selectedIds) override;

	// reimplemented (iser::ISerialize)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	ICollectionViewDelegate* GetViewDelegatePtr(const QByteArray& typeId);
	const ICollectionViewDelegate* GetViewDelegate(const QByteArray& typeId) const;

	virtual void OnPageSelectionUpdated();

	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings) override;
	virtual void OnSaveSettings(QSettings& settings) const override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;
	virtual void OnGuiDesignChanged() override;
	virtual void OnGuiModelShown() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	class PageSelection: virtual public iprm::ISelectionParam, virtual public iprm::IOptionsList
	{
	public:
		PageSelection();

		void SetParent(CObjectCollectionViewComp* parentPtr);

		void SetPageCount(int pageCount);

		// reimplemented (iprm::ISelectionParam)
		virtual const IOptionsList* GetSelectionConstraints() const override;
		virtual int GetSelectedOptionIndex() const override;
		virtual bool SetSelectedOptionIndex(int index) override;
		virtual ISelectionParam* GetSubselection(int index) const override;

		// reimplemented (iprm::IOptionsList)
		virtual int GetOptionsFlags() const override;
		virtual int GetOptionsCount() const override;
		virtual QString GetOptionName(int index) const override;
		virtual QString GetOptionDescription(int index) const override;
		virtual QByteArray GetOptionId(int index) const override;
		virtual bool IsOptionEnabled(int index) const override;

		// reimplemented (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive) override;

	private:
		CObjectCollectionViewComp* m_parentPtr;

		int m_pageCount;
		int m_selectedPageIndex;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractPageSelection(CObjectCollectionViewComp& component)
	{
		return &component.m_pageSelection;
	}


	class Commands: virtual public ibase::ICommandsProvider
	{
	public:
		Commands();

		void SetParent(CObjectCollectionViewComp* parentPtr);

	protected:
		// reimplemented (ibase::ICommandsProvider)
		virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	private:
		CObjectCollectionViewComp* m_parentPtr;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractCommands(CObjectCollectionViewComp& component)
	{
		return &component.m_commands;
	}

	class FocusDecorationFactory: public iwidgets::CFocusDecorator::IGraphicsEffectFactory
	{
	public:
		// reimplemented (iGraphicsEffectFactory)
		virtual std::unique_ptr<QGraphicsEffect> CreateEffect() const override;
	};

private:
	void UpdateCommands();

	QVector<QByteArray> GetMetaInfoIds(const QByteArray& typeId) const;
	QStringList GetMetaInfoHeaders(const QByteArray& typeId) const;
	ICollectionViewDelegate::ObjectMetaInfo GetMetaInfo(const QByteArray& itemId, const QByteArray& typeId) const;

	void EnsureColumnsSettingsSynchronized() const;
	void RestoreColumnsSettings();
	void ValidateSectionSize(int logicalIndex, int newSize);
	void UpdateTypeStatus();
	void OnFilterUpdate(const istd::IChangeable::ChangeSet&, const iprm::IParamsSet* filterPtr);

	// reimplemented (QObject)
	virtual bool eventFilter(QObject* object, QEvent* event) override;

private Q_SLOTS:
	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnItemDoubleClick(const QModelIndex& item);
	void OnCustomContextMenuRequested(const QPoint& point);
	void OnSectionResized(int logicalIndex, int oldSize, int newSize);
	void OnSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
	void OnSortingChanged(int logicalIndex, Qt::SortOrder order);
	void OnTypeChanged();
	void OnTextFilterTimer();

	void OnFilterChanged(const QString& text);
	void OnSearchShortCut();
	void OnEscShortCut();
	void OnDelShortCut();
	void OnRenameShortCut();
	void DoUpdateGui(const istd::IChangeable::ChangeSet& changeSet);

Q_SIGNALS:
	void EmitUpdateGui(const istd::IChangeable::ChangeSet& changeSet);

private:
	class SignalSemaphore
	{
	public:
		SignalSemaphore(int& counter)
		{
			m_counter = &counter;
			(*m_counter)++;
		}

		~SignalSemaphore()
		{
			(*m_counter)--;
		}

	private:
		int* m_counter;
		SignalSemaphore()
		{
		}
	};

	int m_semaphoreCounter;

	QShortcut* m_searchShortCutPtr;
	QShortcut* m_escShortCutPtr;
	QShortcut* m_delShortCutPtr;
	QShortcut* m_renameShortCutPtr;
	iwidgets::CFocusDecorator* m_focusDecoratorPtr;
	FocusDecorationFactory m_graphicsEffectFactory;
	QPropertyAnimation* m_filterPanelAnimationPtr;

	class TableModel: public QAbstractTableModel
	{
	public:
		TableModel(CObjectCollectionViewComp& parent);
		void UpdateFromData(const imtbase::IObjectCollection& collection, const istd::IChangeable::ChangeSet& changes);
		void AddItem(const imtbase::IObjectCollectionInfo::Id& objectId);
		void UpdateItem(const imtbase::IObjectCollectionInfo::Id& objectId);
		void RemoveItem(const imtbase::IObjectCollectionInfo::Id& objectId);
		void SetTextFilter(const QString& textFilter);
		void SetSorting(int logicalIndex, Qt::SortOrder order);
		void SetCurrentTypeId(const QByteArray& typeId);
		const imtbase::ICollectionFilter& GetFilter() const;

		// reimplemented (QAbstractTableModel)
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
		virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
		virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

	protected:
		// reimplemented (QAbstractTableModel)
		virtual bool canFetchMore(const QModelIndex &parent) const override;
		virtual void fetchMore(const QModelIndex &parent) override;
	
	private:
		int m_batchSize;
		qsizetype m_fetchedRowCount;
		mutable imtbase::ICollectionInfo::Ids m_ids;
		int m_totalRowCount;
		ICollectionViewDelegate::ObjectMetaInfo m_metaInfo;
		QStringList m_columns;
		bool m_isPageMode;

		CObjectCollectionViewComp& m_parent;
		mutable QMap<QByteArray, ICollectionViewDelegate::ObjectMetaInfo> m_metaInfoMap;
		imtbase::CCollectionFilter m_filter;
		imtcol::CObjectTypeIdFilter m_objectTypeIdFilter;
	};

	QStandardItemModel m_typeModel;
	TableModel m_tableModel;

	imod::TModelWrap<CObjectCollectionViewDelegate> m_defaultViewDelegate;

	typedef QMap<QByteArray, ICollectionViewDelegate*> ViewDelegateMap;
	ViewDelegateMap m_viewDelegateMap;

	QByteArray m_currentTypeId;
	imod::TModelWrap<PageSelection> m_pageSelection;
	imod::TModelWrap<Commands> m_commands;

	typedef QMap<QString, QVariant> ColumnSettings;
	typedef QVector<ColumnSettings> ColumnsList;
	typedef QMap<QString, ColumnsList> TypeIdColumnsSettings;
	mutable TypeIdColumnsSettings m_typeIdColumnsSettings;

	iqtgui::IGuiObject* m_currentInformationViewPtr;

	imtbase::TModelUpdateBinder<iprm::IParamsSet, CObjectCollectionViewComp> m_filterObserver;

	QTimer m_textFilterTimer;

private:
	/**
		List of collection view delegates.
	*/
	I_ATTR(int, m_leftToolBarCommandGroupIdAttrPtr);
	I_ATTR(int, m_centerToolBarCommandGroupIdAttrPtr);
	I_ATTR(int, m_rightToolBarCommandGroupIdAttrPtr);
	I_MULTIREF(ICollectionViewDelegate, m_viewDelegatesCompPtr);
	I_REF(iqtgui::IGuiObject, m_filterParamsGuiCompPtr);
	I_REF(imod::IObserver, m_filterParamsObserverCompPtr);
	I_REF(iprm::ISelectionParam, m_filterSelectionCompPtr);
	I_REF(iqtgui::IGuiObject, m_paginationGuiCompPtr);
	I_REF(imod::IObserver, m_paginationGuiObserverCompPtr);
	I_REF(iprm::IParamsSet, m_filterParamsCompPtr);
	I_REF(imod::IModel, m_filterParamsModelCompPtr);
	I_TEXTATTR(m_filterEditPlaceholderTextAttrPtr);

	imtgui::CCommandToolBar m_collectionCommandsLeftToolBar;
	imtgui::CCommandToolBar m_collectionCommandsCenterToolBar;
	imtgui::CCommandToolBar m_collectionCommandsRightToolBar;
};


} // namespace imtgui



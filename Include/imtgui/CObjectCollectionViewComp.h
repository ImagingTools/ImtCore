#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QSortFilterProxyModel>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QShortcut>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <ibase/ICommandsProvider.h>
#include <ibase/IProgressManager.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iser/IArchive.h>
#include <iwidgets/CFocusDecorator.h>

// ImtCore includes
#include <imtbase/IMultiSelection.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/TObjectCollectionEventHandlerCompWrap.h>
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <GeneratedFiles/imtgui/ui_CObjectCollectionViewComp.h>


namespace imtgui
{


class CObjectCollectionViewComp:
			public imtbase::TObjectCollectionEventHandlerCompWrap<
						iqtgui::TRestorableGuiWrap<
									iqtgui::TDesignerGuiObserverCompBase<
												Ui::CObjectCollectionViewComp, imtbase::IObjectCollection>>>,
			public imod::CMultiModelDispatcherBase,
			virtual public ibase::IProgressManager,
			virtual public imtbase::IMultiSelection
{
	Q_OBJECT
public:
	typedef imtbase::TObjectCollectionEventHandlerCompWrap<
				iqtgui::TRestorableGuiWrap<
							iqtgui::TDesignerGuiObserverCompBase<
										Ui::CObjectCollectionViewComp, imtbase::IObjectCollection>>> BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CObjectCollectionViewComp);
		I_REGISTER_INTERFACE(ibase::IProgressManager);
		I_REGISTER_INTERFACE(imtbase::IMultiSelection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionEventHandler);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_ASSIGN_MULTI_0(m_viewDelegatesCompPtr, "ViewDelegates", "List of view delegates (corresponding with the object type) used for the collection", false);
		I_ASSIGN(m_columnSettingsKeyAttrPtr, "ColumnSettingsKey", "Registry/INI file key for storing column settings", true, "");
		I_ASSIGN(m_useAsyncReadAttrPtr, "UseAsyncRead", "Useasync collectionRead", true, false);
	I_END_COMPONENT;

	enum DataRole
	{
		DR_TYPE_ID = Qt::UserRole,
		DR_OBJECT_ID,
		DR_SORT_VALUE
	};

	enum ModelId
	{
		MI_DOCUMENT_TYPE_VISUAL_STATUS_BASE = 0,
		MI_LAST = MI_DOCUMENT_TYPE_VISUAL_STATUS_BASE + 1000
	};

	typedef QVector<ICollectionViewDelegate::SummaryInformation> ObjectMetaInfo;

	CObjectCollectionViewComp();

	// reimplemented (imtbase::IObjectCollectionEventHandler)
	virtual void OnCollectionConnected(const imtbase::IObjectCollection* objectCollectionPtr) override;

	// reimplemented (imtbase::IMultiSelection)
	virtual const iprm::IOptionsList* GetSelectionConstraints() const override;
	virtual Ids GetSelectedIds() const override;
	virtual bool SetSelectedIds(const Ids& selectedIds) override;

	// reimplemented (ibase::IProgressManager)
	virtual int BeginProgressSession(
				const QByteArray& progressId,
				const QString& description,
				bool isCancelable = false) override;
	virtual void EndProgressSession(int sessionId) override;
	virtual void OnProgress(int sessionId, double currentProgress) override;
	virtual bool IsCanceled(int sessionId) const override;

	// reimplemented (iser::ISerialize)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	ICollectionViewDelegate& GetViewDelegateRef(const QByteArray& typeId);
	const ICollectionViewDelegate& GetViewDelegate(const QByteArray& typeId) const;

	// reimplemented (imtbase::TObjectCollectionEventHandlerCompWrap)
	virtual void ProcessObjectCollectionEvent(
				const imtbase::IObjectCollection* objectCollectionPtr,
				const imtbase::IObjectCollectionEvent* eventPtr) override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings) override;
	virtual void OnSaveSettings(QSettings& settings) const override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;
 
protected:
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

	class ItemProxyModel: public QSortFilterProxyModel
	{
		QString m_filter;

	public:
		ItemProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent)
		{
		}

		void setFilter(const QString &filter)
		{
			m_filter = filter;
		}

	protected:
		// reimplemented (QSortFilterProxyModel)
		virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
	};

	class FocusDecorationFactory: public iwidgets::CFocusDecorator::GraphicsEffectFactory
	{
	public:
		// reimplemented (iGraphicsEffectFactory)
		virtual QGraphicsEffect* CreateInstance(const QByteArray& keyId = "") const;
		virtual KeyList GetFactoryKeys() const;
	};

	class ReadCollectionThread: public QThread
	{
	public:
		explicit ReadCollectionThread(CObjectCollectionViewComp* parentPtr);
		void SetModels(QStandardItemModel* typeModelPtr, QStandardItemModel* itemModelPtr);

	private:
		virtual void run() override;

	private:
		CObjectCollectionViewComp* m_parentPtr;
		QStandardItemModel* m_typeModelPtr;
		QStandardItemModel* m_itemModelPtr;
	};

private:
	enum ReadCollectionThreadState
	{
		/*
			Read collection thread is idle
		*/
		RCTS_IDLE = 0,

		/*
			Read collection thread is running
		*/
		RCTS_RUNNING,

		/*
			Read collection thread is running and next run is pending
		*/
		RCTS_PENDING
	};

	void UpdateCommands();

	QVector<QByteArray> GetMetaInfoIds(const QByteArray& typeId) const;
	QStringList GetMetaInfoHeaders(const QByteArray& typeId) const;
	ObjectMetaInfo GetMetaInfo(const QByteArray& itemId, const QByteArray& typeId) const;

	void UpdateItem(const imtbase::IObjectCollectionInfo::Id& objectId, QStandardItemModel* modelPtr);
	void RemoveItem(const imtbase::IObjectCollectionInfo::Id& objectId);

	void EnsureColumnsSettingsSynchronized() const;
	void RestoreColumnsSettings();
	void ValidateSectionSize(int logicalIndex, int newSize);

	void SaveItemsSelection();
	void RestoreItemsSelection();

	void UpdateTypeStatus();

	void ReadCollection(QStandardItemModel* typeModelPtr, QStandardItemModel* itemModelPtr);
	void StartCollectionRead();
	Q_INVOKABLE void OnUpdateProgress(int progress);
	Q_INVOKABLE void OnCollectionReadFinished();

	Q_INVOKABLE void ProcessObjectCollectionEventSync(
				ObjectCollectionPtr objectCollectionPtr,
				ObjectCollectionEventPtr eventPtr);

	bool eventFilter(QObject *object, QEvent *event);

private Q_SLOTS:
	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnItemDoubleClick(const QModelIndex &item);
	void OnCustomContextMenuRequested(const QPoint &point);
	void OnSectionResized(int logicalIndex, int oldSize, int newSize);
	void OnSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
	void OnTypeChanged();

	void OnContextMenuRename(bool checked);
	void OnContextMenuEditDescription(bool checked);
	void OnContextMenuEditDocument(bool checked);
	void OnContextMenuRemove(bool checked);

	void OnFilterChanged(const QString &text);
	void OnSearchShortCut();
	void OnEscShortCut();
	void OnDelShortCut();
	void OnRenameShortCut();

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

	ReadCollectionThread m_readCollectionThread;
	ReadCollectionThreadState m_readCollectionThreadState;

	QShortcut* m_searchShortCutPtr;
	QShortcut* m_escShortCutPtr;
	QShortcut* m_delShortCutPtr;
	QShortcut* m_renameShortCutPtr;
	iwidgets::CFocusDecorator* m_focusDecoratorPtr;
	FocusDecorationFactory m_graphicsEffectFactory;
	QPropertyAnimation* m_filterPanelAnimationPtr;

	QStandardItemModel *m_itemModelPtr;
	QStandardItemModel m_itemModel1;
	QStandardItemModel m_itemModel2;
	QStandardItemModel m_typeModel;

	imod::TModelWrap<CObjectCollectionViewDelegate> m_defaultViewDelegate;

	typedef QMap<QByteArray, ICollectionViewDelegate*> ViewDelegateMap;
	ViewDelegateMap m_viewDelegateMap;

	QByteArray m_currentTypeId;
	imod::TModelWrap<Commands> m_commands;

	ItemProxyModel* m_proxyModelPtr;

	typedef QMap<QString, QVariant> ColumnSettings;
	typedef QVector<ColumnSettings> ColumnsList;
	typedef QMap<QString, ColumnsList> TypeIdColumnsSettings;
	mutable TypeIdColumnsSettings m_typeIdColumnsSettings;

	QMap<QByteArray, QByteArrayList> m_itemsSelection;

	iqtgui::IGuiObject* m_currentInformationViewPtr;

private:
	/**
		List of collection view delegates.
	*/
	I_MULTIREF(ICollectionViewDelegate, m_viewDelegatesCompPtr);
	I_ATTR(QByteArray, m_columnSettingsKeyAttrPtr);
	I_ATTR(bool, m_useAsyncReadAttrPtr);

	bool m_eventBasedUpdateEnabled;
};


} // namespace imtgui



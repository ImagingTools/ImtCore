#pragma once


// Qt includes
#include <QtCore/QSortFilterProxyModel>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QShortCut>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iwidgets/CFocusDecorator.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <GeneratedFiles/imtgui/ui_CObjectCollectionViewComp.h>


namespace imtgui
{


class CObjectCollectionViewComp:
			public iqtgui::TRestorableGuiWrap<
						iqtgui::TDesignerGuiObserverCompBase<
									Ui::CObjectCollectionViewComp, imtbase::IObjectCollection>>,
	private imod::CMultiModelDispatcherBase
			
{
	Q_OBJECT
public:
	typedef iqtgui::TRestorableGuiWrap<
				iqtgui::TDesignerGuiObserverCompBase<
							Ui::CObjectCollectionViewComp, imtbase::IObjectCollection>> BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CObjectCollectionViewComp);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_ASSIGN_MULTI_0(m_viewDelegatesCompPtr, "ViewDelegates", "List of view delegates (corresponding with the object type) used for the collection", false);
		I_ASSIGN(m_showCommandsToolBarAttrPtr, "ShowToolBar", "If enabled the command tool bar will be shown", true, true);
	I_END_COMPONENT;

	enum DataRole
	{
		DR_TYPE_ID = Qt::UserRole,
		DR_OBJECT_ID
	};

	enum ModelId
	{
		MI_DOCUMENT_TYPE_VISUAL_STATUS = 0
	};

	CObjectCollectionViewComp();

protected:
	ICollectionViewDelegate& GetViewDelegateRef(const QByteArray& typeId);
	const ICollectionViewDelegate& GetViewDelegate(const QByteArray& typeId) const;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings);
	virtual void OnSaveSettings(QSettings& settings) const;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
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

private:
	void UpdateCommands();

	QVector<QByteArray> GetMetaInfoIds(const QByteArray &typeId) const;
	QStringList GetMetaInfoHeaders(const QByteArray &typeId) const;
	QStringList GetItemMetaInfo(const QByteArray &itemId, const QByteArray &typeId) const;

	void EnsureColumnsSettingsSynchronized() const;
	void RestoreColumnsSettings();

	void SaveItemsSelection();
	void RestoreItemsSelection();

private Q_SLOTS:
	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnItemChanged(QStandardItem *item);
	void OnItemDoubleClick(const QModelIndex &item);
	void OnCustomContextMenuRequested(const QPoint &point);
	void on_TypeList_itemSelectionChanged();

	void OnContextMenuRename(bool checked);
	void OnContextMenuEditDescription(bool checked);
	void OnContextMenuEditDocument(bool checked);
	void OnContextMenuRemove(bool checked);

	void OnFilterChanged(const QString &text);
	void OnSearchShortCut();
	void OnEscShortCut();
	void OnDelShortCut();

private:
	QShortcut* m_searchShortCutPtr;
	QShortcut* m_escShortCutPtr;
	QShortcut* m_delShortCutPtr;
	iwidgets::CFocusDecorator* m_focusDecoratorPtr;
	FocusDecorationFactory m_graphicsEffectFactory;
	QPropertyAnimation* m_filterPanelAnimationPtr;

	QStandardItemModel m_itemModel;

	CObjectCollectionViewDelegate m_defaultViewDelegate;

	typedef QMap<QByteArray, ICollectionViewDelegate*> ViewDelegateMap;
	ViewDelegateMap m_viewDelegateMap;

	I_ATTR(bool, m_showCommandsToolBarAttrPtr);

	/**
		List of collection view delegates.
	*/
	I_MULTIREF(ICollectionViewDelegate, m_viewDelegatesCompPtr);

	QByteArray m_currentTypeId;

	imod::TModelWrap<Commands> m_commands;
	
	ItemProxyModel* m_proxyModelPtr;

	bool m_blockColumnsSettingsSynchronize;
	bool m_blockSaveItemsSelection;

	typedef QMap<QString, QVariant> ColumnSettings;
	typedef QVector<ColumnSettings> ColumnsList;
	typedef QMap<QString, ColumnsList> TypeIdColumnsSettings;
	mutable TypeIdColumnsSettings m_typeIdColumnsSettings;

	QMap<QByteArray, QByteArrayList> m_itemsSelection;
};


} // namespace imtgui



#pragma once


// Qt includes
#include <QtCore/QSortFilterProxyModel>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QMenu>

// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtgui/ui_CObjectCollectionViewComp.h>
#include <imtgui/CObjectCollectionViewDelegate.h>


namespace imtgui
{


class CObjectCollectionViewComp:
			public iqtgui::TRestorableGuiWrap<iqtgui::TDesignerGuiObserverCompBase<Ui::CObjectCollectionViewComp, imtbase::IObjectCollection>>
			
{
	Q_OBJECT
public:
	typedef iqtgui::TRestorableGuiWrap<iqtgui::TDesignerGuiObserverCompBase<Ui::CObjectCollectionViewComp, imtbase::IObjectCollection>> BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionViewComp);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_ASSIGN_MULTI_0(m_objectTypeIdsAttrPtr, "ObjectTypeIds", "List of type-IDs supported by this view", false);
		I_ASSIGN_MULTI_0(m_viewDelegatesCompPtr, "ViewDelegates", "List of view delegates (corresponding with the object type) used for the collection", false);
		I_ASSIGN(m_showCommandsToolBarAttrPtr, "ShowToolBar", "If enabled the command tool bar will be shown", true, true);
	I_END_COMPONENT;

	enum DataRole
	{
		DR_TYPE_ID = Qt::UserRole,
		DR_OBJECT_ID
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

	class QCustomSortFilterProxyModel: public QSortFilterProxyModel
	{
		QString m_filter;

	public:
		QCustomSortFilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent)
		{
		}

		void setFilter(const QString &filter)
		{
			m_filter = filter;
		}

	protected:
		virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
	};

private:
	void UpdateCommands();

	QVector<QByteArray> GetMetaInfoIds(const QByteArray &typeId) const;
	QStringList GetMetaInfoHeaders(const QByteArray &typeId) const;
	QStringList GetObjectMetaInfo(const QByteArray &itemId, const QByteArray &typeId) const;

	void EnsureColumnsSettingsSynchronized() const;
	void RestoreColumnsSettings();

private Q_SLOTS:
	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnItemChanged(QStandardItem *item);
	void OnItemDoubleClick(const QModelIndex &item);
	void OnCustomContextMenuRequested(const QPoint &point);
	void on_TypeList_itemSelectionChanged();

	void OnContextMenuRename(bool checked);
	void OnContextMenuEdit(bool checked);
	void OnContextMenuRemove(bool checked);

	void OnFilterChanged(const QString &text);

private:
	QStandardItemModel m_itemModel;

	CObjectCollectionViewDelegate m_defaultViewDelegate;

	typedef QMap<QByteArray, ICollectionViewDelegate*> ViewDelegateMap;
	ViewDelegateMap m_viewDelegateMap;

	I_ATTR(bool, m_showCommandsToolBarAttrPtr);

	/**
		List of supported type object's type-IDs.
	*/
	I_MULTIATTR(QByteArray, m_objectTypeIdsAttrPtr);
	I_MULTIREF(ICollectionViewDelegate, m_viewDelegatesCompPtr);

	QByteArray m_currentTypeId;

	imod::TModelWrap<Commands> m_commands;
	
	QSortFilterProxyModel* m_proxyModelPtr;
	QCustomSortFilterProxyModel* m_customProxyModelPtr;

	bool m_blockColumnsSettingsSynchronize;

	typedef QMap<QString, QVariant> ColumnSettings;
	typedef QVector<ColumnSettings> ColumnsList;
	typedef QMap<QString, ColumnsList> TypeIdColumnsSettings;
	mutable TypeIdColumnsSettings m_typeIdColumnsSettings;
};


} // namespace imtgui



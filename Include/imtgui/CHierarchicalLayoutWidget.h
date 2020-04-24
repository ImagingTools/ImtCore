#pragma once


// Qt includes
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtWidgets/QWidget>
#include <GeneratedFiles/imtgui/ui_CCustomLayoutWidgetForm.h>

// ACF includes
#include <iser/ISerializable.h>
#include <iser/CXmlStringWriteArchive.h>
#include <iser/CXmlStringReadArchive.h>


namespace imtgui
{


class CCustomLayoutWidget;


/**
	Class defines hierarchical layouting as a widget.
*/
class CHierarchicalLayoutWidget: public QWidget
{
	Q_OBJECT

public:
	typedef QList<QByteArray> IdsList;
	typedef QList<int> SizeList;

	enum LayoutType
	{
		LT_NONE = 0,
		LT_VERTICAL_SPLITTER,
		LT_HORIZONTAL_SPLITTER,
		LT_OBJECT,
		LT_MERGE
	};

	I_DECLARE_ENUM(LayoutType, LT_NONE, LT_VERTICAL_SPLITTER, LT_HORIZONTAL_SPLITTER, LT_OBJECT, LT_MERGE);

	enum AlignType
	{
		AT_LEFT = 0,
		AT_RIGHT,
		AT_H_CENTER
	};

	I_DECLARE_ENUM(AlignType, AT_LEFT, AT_RIGHT, AT_H_CENTER);

	enum ViewMode
	{
		VM_UNDEFINED,
		VM_NORMAL,
		VM_EDIT
	};

	CHierarchicalLayoutWidget(QWidget* parentPtr = Q_NULLPTR);

	void SetViewMode(ViewMode viewMode);
	bool SetRootItemId(const QByteArray& id);
	bool SetLayoutToItem(const QByteArray& id, LayoutType type, int count, IdsList* idsListPtr = NULL);
	bool SetItemSizes(const QByteArray& id, const SizeList& sizeList);
	bool SetWidgetToItem(const QByteArray& id, const QByteArray& viewId, QWidget* widgetPtr);

	QByteArray GetRootItemId();
	IdsList GetItemChildIdList(const QByteArray& id);
	LayoutType GetItemLayoutType(const QByteArray& id);
	SizeList GetItemSizes(const QByteArray& id);
	void ClearAll();
	void ResetWidget();
	void SetSplitterLayout(const QByteArray& id, Qt::Orientation orientation, int count, IdsList* idsListPtr = NULL);
	void MergeLayout(const QByteArray& id);
	void RemoveLayout(const QByteArray& id);
	void SetName(const QByteArray& id, QString &name);
	QString GetName(const QByteArray& id);
	void SetAdditionalNames(QStringList& additionalNames);
	bool Serialize(iser::IArchive& archive); // callbackFunc for get Widget

protected:
	friend class CCustomLayoutWidget;
	void OnDropEvent(const QByteArray& id, QDropEvent* eventPtr);
	void OnMouseReleaseEvent(const QByteArray& id, QMouseEvent* eventPtr);
	void OnSplitterMoved(const QByteArray& id, SizeList sizeList);

private:
	struct InternalItemData{
		InternalItemData(QByteArray idIn, IdsList childItemsIn = IdsList(), LayoutType layoutTypeIn = LayoutType::LT_NONE, SizeList sizeListIn = SizeList())
			:id(idIn),
			childItems(childItemsIn),
			layoutType(layoutTypeIn),
			sizeList(sizeListIn)
		{
		}

		QByteArray id;
		IdsList childItems;
		LayoutType layoutType;
		SizeList sizeList;

		bool operator<(const InternalItemData& item){
			return id < item.id;
		}
		bool operator==(const InternalItemData& item){
			return id == item.id;
		}
	};

private:
	bool SerializeRecursive(iser::IArchive& archive, QWidget** widget);
	void CleanLayoutRecursive(QLayout* layoutPtr);
	void IdsListCollectChildIdsRecursive(const QByteArray& id, IdsList& idList);
	InternalItemData* GetInternalItem(const QByteArray& id);

Q_SIGNALS:
	void EmitLayoutChanged(QByteArray id, LayoutType type, int count);
	void EmitDropEvent(QByteArray id, QDropEvent* eventPtr);
	void EmitOpenMenuEvent(QByteArray id, QMouseEvent* eventPtr);
	void EmitClearEvent(const QByteArray& id);
	void EmitAddWidget(const QByteArray& id, int index);
	void EmitAddWidgetByViewId(const QByteArray& id, const QByteArray& viewId);
	void EmitDeleteWidget(const QByteArray& id);
	void EmitSplitVertical(const QByteArray& id);
	void EmitSplitHorizontal(const QByteArray& id);

private:
	typedef QMap<QByteArray, CCustomLayoutWidget*> CustomWidgetMap;
	CustomWidgetMap m_customWidgetMap;
	QList<InternalItemData> m_internalItemList;
	ViewMode m_viewMode;
	QStringList m_additionalNames;
	QByteArray m_rootId;
};


/**
	Helper class for one item in layout
*/
class CCustomLayoutWidget: public QWidget, Ui::CCustomLayoutWidgetForm
{
	Q_OBJECT
public:
	CCustomLayoutWidget(const QByteArray& id, CHierarchicalLayoutWidget& hierarchicalLayoutWidget, CCustomLayoutWidget* parentCustomWidgetPtr = Q_NULLPTR, QWidget* parentPtr = Q_NULLPTR);

	void SetDefaultPalette();
	void SetHighLightPalette();
	bool SetWidget(QWidget* widgetPtr);
	QWidget* GetWidget();
	void SetIsHaveChilds(bool source);
	void SetName(QString name);
	QString GetName();
	void SetEditMode(bool isEditMode);
	QByteArray GetId();
	void SetId(QByteArray &id);
	QPixmap GetIcon();
	void SetIcon(QPixmap &icon);
	QByteArray GetViewId();
	void SetViewId(const QByteArray &viewId);
	void SetTitleAlign(const CHierarchicalLayoutWidget::AlignType &align);
	CHierarchicalLayoutWidget::AlignType GetTitleAlign();

protected:
	// reimplemented (QWidget)
	virtual void paintEvent(QPaintEvent* eventPtr) Q_DECL_OVERRIDE;
	virtual void dragEnterEvent(QDragEnterEvent* eventPtr) Q_DECL_OVERRIDE;
	virtual void dragLeaveEvent(QDragLeaveEvent* eventPtr) Q_DECL_OVERRIDE;
	virtual void dropEvent(QDropEvent* eventPtr) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent *eventPtr) Q_DECL_OVERRIDE;

protected Q_SLOTS:
	void OnSplitterMoved(int pos, int index);
	void OnNamePosition();
	void OnAddWidget();
	void OnDeleteWidget();
	void OnSplitVertical();
	void OnSplitHorizontal();
	void OnAlignLeft();
	void OnAlignCenter();
	void OnAlignRight();
	void OnChangeName();
	void OnChangeIcon();

private:
	CHierarchicalLayoutWidget& m_hierarchicalLayoutWidget;
	CCustomLayoutWidget* m_parentCustomWidgetPtr;
	QByteArray m_id;
	QByteArray m_viewId;
	QWidget* m_editPanelPtr;
	QWidget* m_externalWidgetPtr;
	QString m_name;
	int m_titleSize;
	bool m_isHaveChilds;
	CHierarchicalLayoutWidget::AlignType m_titleAlign;
};


} //namespace imtgui



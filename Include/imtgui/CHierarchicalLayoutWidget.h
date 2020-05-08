#pragma once

// Qt includes
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtWidgets/QWidget>
#include <GeneratedFiles/imtgui/ui_CCustomLayoutWidgetForm.h>

// ACF includes
#include <iser/ISerializable.h>
#include <iser/CXmlStringWriteArchive.h>
#include <iser/CXmlStringReadArchive.h>
#include <imtgui/ILayout.h>
#include <imtgui/CLayoutSettingsDialog.h>


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


	enum ViewMode
	{
		VM_UNDEFINED,
		VM_NORMAL,
		VM_EDIT
	};

	CHierarchicalLayoutWidget(QWidget* parentPtr = Q_NULLPTR);

	void SetViewMode(ViewMode viewMode);
	CHierarchicalLayoutWidget::ViewMode GetViewMode();
	void ClearAll();
	void SetAdditionalNames(QStringList& additionalNames);
	void CleanLayoutRecursive(QLayout* layoutPtr);
	CCustomLayoutWidget* createCustomWidget();
	void SetBorderColor(const QColor &color);
	QColor GetBorderColor() const;
	void SetIsShowBox(bool isShowBox);
	bool GetIsShowBox() const;

protected:
	friend class CCustomLayoutWidget;

private:

Q_SIGNALS:
	void EmitLayoutChanged(QByteArray id, ILayout::LayoutType type, int count);
	void EmitDropEvent(QByteArray id, QDropEvent* eventPtr);
	void EmitOpenMenuEvent(QByteArray id, QMouseEvent* eventPtr);
	void EmitClearEvent(const QByteArray& id);
	void EmitAddWidget(const QByteArray& id, int index);
	void EmitAddWidgetByViewId(const QByteArray& id, const QByteArray& viewId);
	void EmitDeleteWidget(const QByteArray& id);
	void EmitSplitVertical(const QByteArray& id);
	void EmitSplitHorizontal(const QByteArray& id);
	void EmitChangeIcon(const QByteArray& id);
	void EmitRemoveIcon(const QByteArray& id);
	void EmitChangeAlignTitle(const QByteArray& id, const ILayout::AlignType& align);
	void EmitChangeTitle(const QByteArray& id, const QString& title);
	void EmitChangeSizes(const QByteArray& id, const SizeList& sizeList);
	void EmitChangeProperties(const QByteArray& id, const ILayout::LayoutProperties& properties);

private:
	typedef QMap<QByteArray, CCustomLayoutWidget*> CustomWidgetMap;
	CustomWidgetMap m_customWidgetMap;
	ViewMode m_viewMode;
	QStringList m_additionalNames;
	QColor m_borderColor;
	bool m_isShowBox;
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
	void SetName(QString name);
	QString GetName();
	void SetEditMode(bool isEditMode);
	QByteArray GetId();
	void SetId(const QByteArray &id);
	QPixmap GetIcon();
	void SetIcon(const QPixmap &icon);
	QByteArray GetViewId();
	void SetViewId(const QByteArray &viewId);
	void SetTitleAlign(const ILayout::AlignType &align);
	ILayout::AlignType GetTitleAlign();
    void SetLayoutProperties(const ILayout::LayoutProperties &properties);

protected:
	// reimplemented (QWidget)
	virtual void paintEvent(QPaintEvent* eventPtr) Q_DECL_OVERRIDE;

protected Q_SLOTS:
	void OnAddWidget();
	void OnChangeSettings();
	void OnDeleteWidget();
	void OnSplitVertical();
	void OnSplitHorizontal();
	void OnAlignLeft();
	void OnAlignCenter();
	void OnAlignRight();
	void OnChangeIcon();
	void OnTitleChanged();
	void OnRemoveIcon();

private:
	CHierarchicalLayoutWidget& m_hierarchicalLayoutWidget;
	CCustomLayoutWidget* m_parentCustomWidgetPtr;
	QByteArray m_id;
	QByteArray m_viewId;
	QWidget* m_externalWidgetPtr;
	ILayout::AlignType m_titleAlign;
	ILayout::LayoutProperties m_properties;
};


} //namespace imtgui



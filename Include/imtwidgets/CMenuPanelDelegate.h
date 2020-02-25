#pragma once


// Qt includes
#include <QtWidgets/QStyledItemDelegate>

// Qt includes
#include <QtWidgets/QTreeView>


namespace imtwidgets
{


class CMenuPanelDelegate: public QStyledItemDelegate
{
public:
	explicit CMenuPanelDelegate(QTreeView* menuPanelPtr = nullptr);
	
	int GetMinimumWidth();

	void SetFontMetrics(const QFontMetrics& fontMetrics);

	void SetMaxIndent(int indent);
	void SetItemHeight(int height);
	void SetIconSizeRatio(double ratio);

	void SetTopPadding(int padding);
	void SetLeftPadding(int padding);
	void SetRightPadding(int padding);
	void SetIconToTextPadding(int padding);

	void SetSelectedColor(const QColor& color);
	void SetMouserOverColor(const QColor& color);
	void SetMouserOverSelectedColor(const QColor& color);
	void SetSelectedContourColor(const QColor& color);
	void SetTextColor(const QColor& color);

protected:
	// reimplemented (QItemDelegate)
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	int m_maxIndent;
	int m_height;

	double m_iconSizeRatio;
	int m_iconHeight;

	int m_topPadding;
	int m_leftPadding;
	int m_rightPadding;
	int m_iconToTextPadding;

	QColor m_textColor;
	QColor m_selectedContourColor;
	QColor m_selectedColor;
	QColor m_mouseOverColor;
	QColor m_mouseOverSelectedColor;

	QFontMetrics m_fontMetrics;

};			 


} // namespace imtwidgets



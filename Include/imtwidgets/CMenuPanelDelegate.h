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

	void SetIndent(int indent);
	void SetItemHeight(int height);
	void SetIconSizeRatio(double ratio);
	void SetIconSizeHoverRatio(double ratio);

	void SetTopPadding(int padding);
	void SetLeftPadding(int padding);
	void SetRightPadding(int padding);
	void SetIconToTextPadding(int padding);

protected:
	// reimplemented (QItemDelegate)
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	int m_indent;
	int m_height;

	double m_iconSizeRatio;
	double m_iconSizeHoverRatio;
	int m_iconHeight;
	int m_iconHeightHover;

	int m_topPadding;
	int m_leftPadding;
	int m_rightPadding;
	int m_iconToTextPadding;

	QFontMetrics m_fontMetrics;
};


} // namespace imtwidgets



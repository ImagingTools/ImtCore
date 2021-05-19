#pragma once


// ImtCore includes
#include <imtwidgets/IMenuPanelDelegate.h>

namespace imtwidgets
{


class CMenuPanelDelegate: public IMenuPanelDelegate
{
public:
	explicit CMenuPanelDelegate(QTreeView* menuPanelPtr = nullptr);
	
	// reimplemented (IMenuPanelDelegate)
	int GetMinimumWidth() override;

	void SetFontMetrics(const QFontMetrics& fontMetrics) override;

	void SetIndent(int indent) override;
	void SetItemHeight(int height) override;
	void SetIconSizeRatio(double ratio) override;
	void SetIconSizeHoverRatio(double ratio) override;

	void SetTopPadding(int padding) override;
	void SetLeftPadding(int padding) override;
	void SetRightPadding(int padding) override;
	void SetIconToTextPadding(int padding) override;

protected:
	// reimplemented (QItemDelegate)
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

protected:
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



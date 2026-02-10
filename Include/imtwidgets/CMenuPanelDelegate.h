// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QTreeView>

// ImtCore includes
#include <imtwidgets/IMenuPanelDelegate.h>


namespace imtwidgets
{


class CMenuPanelDelegate: public IMenuPanelDelegate
{
public:
	explicit CMenuPanelDelegate(QTreeView* menuPanelPtr = nullptr);
	
	// reimplemented (IMenuPanelDelegate)
	virtual int GetMinimumWidth() override;

	virtual void SetFont(const QFont& font) override;

	virtual void SetIndent(int indent) override;
	virtual void SetItemHeight(int height) override;
	virtual void SetIconSizeRatio(double ratio) override;
	virtual void SetIconSizeHoverRatio(double ratio) override;

	virtual void SetTopPadding(int padding) override;
	virtual void SetLeftPadding(int padding) override;
	virtual void SetRightPadding(int padding) override;
	virtual void SetIconToTextPadding(int padding) override;

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

	QFont m_font;
};


} // namespace imtwidgets



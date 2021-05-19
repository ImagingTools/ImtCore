#pragma once


// Qt includes
#include <QtWidgets/QStyledItemDelegate>

// Qt includes
#include <QtWidgets/QTreeView>


namespace imtwidgets
{


class IMenuPanelDelegate: virtual public QStyledItemDelegate
{
public:	
	virtual int GetMinimumWidth() = 0;

	virtual void SetFontMetrics(const QFontMetrics& fontMetrics) = 0;

	virtual void SetIndent(int indent) = 0;
	virtual void SetItemHeight(int height) = 0;
	virtual void SetIconSizeRatio(double ratio) = 0;
	virtual void SetIconSizeHoverRatio(double ratio) = 0;

	virtual void SetTopPadding(int padding) = 0;
	virtual void SetLeftPadding(int padding) = 0;
	virtual void SetRightPadding(int padding) = 0;
	virtual void SetIconToTextPadding(int padding) = 0;
};


} // namespace imtwidgets



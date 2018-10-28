#pragma once


// Qt includes
#include <QtWidgets/QStyledItemDelegate>
#include <QtGui/QStandardItemModel>

// ACF includes
#include <iprm/ISelectionParam.h>
#include <ilog/IMessageContainer.h>
#include <iimg/IBitmap.h>


namespace imtgui
{


class CThumbpageItemGuiDelegate : public QStyledItemDelegate
{
public:
	typedef QStyledItemDelegate BaseClass;

	enum DataRole
	{
		DR_TYPE_ID = Qt::UserRole,
		DR_PAGE_ID
	};

	CThumbpageItemGuiDelegate(const QStandardItemModel& itemModel, const int horizontalSpacing = 6, const int verticalSpacing = 6, QObject* parent = NULL);
	
	/**
		Margins setter function. If input parameter equals -1, corresponding margin is left untouched
	*/
	void SetMargins(int horizontal, int vertical);

protected:
	// reimplemented (QItemDelegate)
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const override;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;



	protected:

private:
	const QStandardItemModel& m_itemModel;
	/**
		draw margin, equals half the spacing
	*/
	int m_verticalMargin;
	int m_horizontalMargin;
};


} // namespace imtgui




#pragma once


// Qt includes
#include <QtWidgets/QStyledItemDelegate>
#include <QtGui/QStandardItemModel>

// ACF includes
#include <iprm/ISelectionParam.h>


namespace imtgui
{


/**
	Item delegate used for drawing page' thumbnails.
*/
class CThumbpageItemGuiDelegate: public QStyledItemDelegate
{
public:
	typedef QStyledItemDelegate BaseClass;

	enum DataRole
	{
		DR_TYPE_ID = Qt::UserRole,
		DR_PAGE_ID
	};

	CThumbpageItemGuiDelegate(
				const QStandardItemModel& itemModel,
				int horizontalSpacing = 6,
				int verticalSpacing = 6,
				QObject* parent = NULL);
	
	/**
		Set margin size for the main frame. If any input parameter is -1, corresponding margin is left untouched.
	*/
	void SetMargins(int horizontal, int vertical);

	// reimplemented (QItemDelegate)
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const override;

protected:
	// reimplemented (QItemDelegate)
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	const QStandardItemModel& m_itemModel;

	int m_verticalMargin;
	int m_horizontalMargin;
};


} // namespace imtgui



#pragma once


// Qt includes
#include <QtWidgets/QItemDelegate>
#include <QtGui/QStandardItemModel>

// ACF includes
#include <iprm/ISelectionParam.h>
#include <ilog/IMessageContainer.h>
#include <iimg/IBitmap.h>


namespace imtgui
{


	class CThumbpageItemGuiDelegate : public QItemDelegate
	{
	public:
		typedef QItemDelegate BaseClass;

		enum DataRole
		{
			DR_TYPE_ID = Qt::UserRole,
			DR_PAGE_ID
		};

		CThumbpageItemGuiDelegate(const QStandardItemModel& itemModel, const int horizontalSpacing = 6, const int verticalSpacing = 6, QObject* parent = NULL);

	protected:

		// reimplemented (QItemDelegate)
		virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const override;
		virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	private:
		const QStandardItemModel& m_itemModel;
		/**
			draw margin, equals half the spacing
		*/
		int m_verticalMargin, m_horizontalMargin;
	};


} // namespace imtgui




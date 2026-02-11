// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QItemDelegate>


namespace imthypegui
{


class CTaskItemDelegate: public QItemDelegate
{
public:
	typedef QItemDelegate BaseClass;

	enum DataRole
	{
		DR_TASK_UUID = Qt::UserRole,
		DR_TYPE_ID,
		DR_TYPE_NAME,
		DR_TASK_NAME,
		DR_TASK_USERID,
		DR_TASK_INPUTID,
		DR_TASK_PREVIEW_OBJECT,
		DR_TASK_PROCESSING_STATE,
		DR_TASK_PROCESSING_STATE_ICON,
		DR_TASK_ENABLED,
		DR_LAST = DR_TASK_ENABLED
	};

	CTaskItemDelegate(QObject* parent = nullptr);

protected:
	// reimplemented (QItemDelegate)
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


class CMicroTaskItemDelegate : public CTaskItemDelegate
{
public:
	typedef CTaskItemDelegate BaseClass;

	CMicroTaskItemDelegate(QObject* parent = nullptr);

protected:
	// reimplemented (QItemDelegate)
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


} // namespace imthypegui



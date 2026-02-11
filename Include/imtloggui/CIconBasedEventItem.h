// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtGui/QPixmap>

// ImtCore includes
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


class CIconBasedEventItem: public CEventItemBase
{
public:
	typedef CEventItemBase BaseClass;

	void SetParams(
				const QPixmap& icon,
				const ilog::IMessageConsumer::MessagePtr& messagePtr,
				QGraphicsItem* parentPtr = nullptr);

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

protected:
	QPixmap m_icon;
	mutable QRectF m_boundingRect;
};


} // namespace imtloggui



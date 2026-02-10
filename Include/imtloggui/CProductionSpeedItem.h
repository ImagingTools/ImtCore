// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QGraphicsRectItem>

// ACF includes
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtloggui/IEventScenePositionProvider.h>
//#include <imtloggui/CRepresentationProductionModel.h>
#include <istd/IChangeable.h>


namespace imtloggui
{


class CProductionSpeedItem: public QGraphicsRectItem
{
public:
	typedef QGraphicsRectItem BaseClass;

	CProductionSpeedItem(QGraphicsItem* parent = nullptr);
	istd::TSmartPtr<istd::IChangeable> GetModel();
	void SetModel(istd::TSmartPtr<istd::IChangeable> modelPtr);
	void SetScenePositionProvider(IEventScenePositionProvider* providerPtr);

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

private:
	istd::TSmartPtr<istd::IChangeable> m_modelPtr;
	IEventScenePositionProvider* m_positionProviderPtr;
};


} // namespace imtloggui



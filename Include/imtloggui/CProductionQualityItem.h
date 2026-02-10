// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QGraphicsRectItem>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CProductionQualityItem: public QGraphicsRectItem
{
public:
	typedef QGraphicsRectItem BaseClass;

	enum DrawStyle
	{
		DS_BAR = 0,
		DS_POLYLINE
	};

	CProductionQualityItem(QGraphicsItem* parent = nullptr);
	istd::IChangeable* GetModel();
	void SetModel(istd::IChangeable* modelPtr);
	void SetScenePositionProvider(IEventScenePositionProvider* providerPtr);
	void SetStyle(DrawStyle style);

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

private:
	bool IsEmptyInterval(int i);
	void PreparePointArrays();
	void DrawBars(QPainter* painter);
	void DrawPolylines(QPainter* painter);
	void DrawLegend(QPainter* painter);

private:
	istd::IChangeable* m_modelPtr;
	IEventScenePositionProvider* m_positionProviderPtr;

	QVector<QPointF> m_okPoints;
	QVector<QPointF> m_warningPoints;
	QVector<QPointF> m_nokPoints;
	QVector<QPointF> m_errorsPoints;

	DrawStyle m_style;
};


} // namespace imtloggui



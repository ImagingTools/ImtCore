#pragma once


// Qt includes
#include <QtWidgets/QGraphicsRectItem>

// Acf includes
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtloggui/IEventScenePositionProvider.h>
#include <imtloggui/CRepresentationProductionSpeedModel.h>


namespace imtloggui
{


class CProductionQualityItem: public QGraphicsRectItem
{
public:
	typedef QGraphicsRectItem BaseClass;

	enum Style
	{
		S_Bar = 0,
		S_Polyline
	};

	CProductionQualityItem(QGraphicsItem* parent = nullptr);
	istd::TSmartPtr<istd::IChangeable> GetModel();
	void SetModel(istd::TSmartPtr<istd::IChangeable> modelPtr);
	void SetScenePositionProvider(IEventScenePositionProvider* providerPtr);
	void SetStyle(Style style);

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

private:
	bool IsEmptyInterval(int i);

private:
	istd::TSmartPtr<istd::IChangeable> m_modelPtr;
	IEventScenePositionProvider* m_positionProviderPtr;

	QVector<QPointF> m_okPoints;
	QVector<QPointF> m_warningPoints;
	QVector<QPointF> m_nokPoints;
	QVector<QPointF> m_errorsPoints;

	Style m_style;
};


} // namespace imtloggui



#pragma once


// Qt includes
#include <QtWidgets/QGraphicsRectItem>

// ImtCore includes
#include <imtloggui/CEventGroupLabelItem.h>


namespace imtloggui
{


class CEventGroupItem: public QGraphicsRectItem
{
public:
	typedef QGraphicsRectItem BaseClass;

	CEventGroupItem(QGraphicsItem* parent = nullptr);
	~CEventGroupItem();

	void SetBackgroundColor(const QColor& color);
	void SetGroupName(const QString& name);
	void OnViewPortChanged();

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	double GetCurrentScaleX() const;
	double GetCurrentScaleY() const;
	QRectF GetSceneVisibleRect() const;
	QRectF GetItemVisibleRect() const;

private:
	QColor m_backgroundColor;
	QString m_name;

	CEventGroupLabelItem* m_labelPtr;
};


} // namespace imtloggui



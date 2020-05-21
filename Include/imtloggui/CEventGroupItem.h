#pragma once


// Qt includes
#include <QtWidgets/QGraphicsRectItem>

// ImtCore includes
//#include <imtloggui/CGroupLabelItem.h>


namespace imtloggui
{


class CEventGroupItem: public QGraphicsRectItem
{
public:
	typedef QGraphicsRectItem BaseClass;

	CEventGroupItem(QGraphicsItem* parent = nullptr);
	~CEventGroupItem();

	void SetBackgroundColor(const QColor& color);
	void SetGroupName(QString name);
	void ViewPortChanged();

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	double GetCurrentScaleX() const;
	double GetCurrentScaleY() const;
	QRectF GetSceneVisibleRect() const;
	QRectF GetItemVisibleRect() const;

private:
	QColor m_bgColor;
	QString m_name;
	//CGroupLabelItem* m_labelPtr;
};


} // namespace imtloggui



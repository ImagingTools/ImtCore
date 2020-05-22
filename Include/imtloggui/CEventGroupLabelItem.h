#pragma once


// Qt includes
#include <QtGui/QFont>
#include <QtWidgets/QGraphicsItem>


namespace imtloggui
{


class CEventGroupLabelItem: public QGraphicsItem
{
public:
	typedef QGraphicsItem BaseClass;

	CEventGroupLabelItem(QGraphicsItem* parent = nullptr);

	void SetBackgroundColor(const QColor& color);
	void SetGroupName(const QString& name);
	void SetHeight(double height);

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	QFont m_font;
	QColor m_backgroundColor;
	QString m_name;
	double m_height;
};


} // namespace imtloggui



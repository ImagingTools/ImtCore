// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CCanvasItem.h>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QPen>


namespace imtqml
{

CCanvasItem::CCanvasItem(QQuickItem *parent):QQuickPaintedItem(parent),
	m_lineWidth(1), m_image(100,100,QImage::Format_ARGB32), m_pixmap(100,100), m_painterPath()
{
	//QObject::connect(this, SIGNAL(widthChanged()), this, SLOT(onWidthChanged()));
	//QObject::connect(this, SIGNAL(heightChanged()), this, SLOT(onHeightChanged()));
}


void CCanvasItem::paint(QPainter *painter)
{
	painter->drawPixmap(0, 0, m_pixmap);
	//qDebug() << "PAINT____________";
}

void CCanvasItem::stroke()
{
	if(!m_pixmap){
		//qDebug() << "stroke_____returned!";
		return;
	}
	QPainter painter(&m_pixmap);
	painter.setFont(m_qfont);

	// painter.setPen(QPen(QColor(m_strokeStyle), m_lineWidth, Qt::SolidLine,
	// 					Qt::RoundCap, Qt::RoundJoin));

	painter.setPen(QPen(QColor(m_strokeStyle), m_lineWidth, Qt::SolidLine,
						Qt::FlatCap, Qt::MiterJoin));

	painter.drawPath(m_painterPath);
	//qDebug() << "stroke_____" << m_lineWidth << m_strokeStyle << m_pixmap.width() << m_pixmap.height();
}

void CCanvasItem::fill()
{
	if(!m_pixmap){
		return;
	}
	QPainter painter(&m_pixmap);
	painter.setFont(m_qfont);
	painter.setBrush(QBrush(QColor(m_fillStyle)));
	painter.drawPath(m_painterPath);

}

void CCanvasItem::beginPath()
{
	m_painterPath.clear();
}

void CCanvasItem::save()
{

}

void CCanvasItem::restore()
{

}

void CCanvasItem::clip()
{

}

void CCanvasItem::requestPaint()
{
	emit paintRequired();
}

void CCanvasItem::closePath()
{
	//m_painterPath.clear();
}

QSize CCanvasItem::measureText(QString string)
{
	QFontMetrics fontMetrix(m_qfont);
	int length = fontMetrix.horizontalAdvance(string);
	return QSize(length, 0);
}

void CCanvasItem::arc(double x, double y, double radius, double startAngle, double endAngle, bool /*anticlockwise*/)
{
	if(!m_pixmap){
		return;
	}

	double size = 2*radius;

	m_painterPath.moveTo(x + radius, y);
	m_painterPath.arcTo(x - radius, y - radius, size , size, 180 * startAngle / M_PI, 180 * endAngle / M_PI);
}

void CCanvasItem::fillRect(double x, double y, double width, double height)
{
	if(!m_pixmap){
		return;
	}
	beginPath();
	m_painterPath.addRect(x, y, width, height);
	QPainter painter(&m_pixmap);

	painter.setBrush(QBrush(QColor(m_fillStyle)));
	painter.drawPath(m_painterPath);
	closePath();

}


void CCanvasItem::moveTo(double x, double y)
{
	m_startPoint = QPointF(x,y);
	m_painterPath.moveTo(x, y);
	//qDebug() << "MOVE_TO:: " << x << y;
}

void CCanvasItem::lineTo(double x, double y)
{
	if(!m_pixmap){
		return;
	}
	//qDebug()<< "LINE_TO:: " << x << " " << y;
	m_painterPath.lineTo(x, y);
}

void CCanvasItem::fillText(QString text, double x, double y)
{
	if(!m_pixmap){
		return;
	}
	QPainter painter(&m_pixmap);

	// beginPath();
	// m_painterPath.addText(x, y, font, text);
	painter.setFont(m_qfont);
	//painter.setBrush(QBrush(QColor(m_fillStyle)));

	// painter.setPen(QPen(QColor(m_strokeStyle), m_lineWidth, Qt::SolidLine,
	// 					Qt::RoundCap, Qt::RoundJoin));
	painter.setPen(QPen(QColor(m_strokeStyle), m_lineWidth, Qt::SolidLine,
						Qt::FlatCap, Qt::MiterJoin));
	//painter.drawPath(m_painterPath);
	// closePath();
	painter.drawText(x, y, text);
}

void CCanvasItem::text(QString text, double x, double y)
{
	if(!m_pixmap){
		return;
	}

	m_painterPath.addText(x, y, m_qfont, text);
}



void CCanvasItem::setPixmapSize(double width, double height)
{
	m_pixmap = QPixmap(width, height);
}

double CCanvasItem::lineWidth() const
{
	return m_lineWidth;
}

void CCanvasItem::setLineWidth(double newLineWidth)
{
	if (qFuzzyCompare(m_lineWidth, newLineWidth))
		return;
	m_lineWidth = newLineWidth;
	emit lineWidthChanged();
}

void CCanvasItem::onWidthChanged()
{
	//qDebug() << "WIDTH_CHANGED____" << width();
	//m_pixmap = QPixmap(width(), height());
	//draw();
	//drawRect();
}

void CCanvasItem::onHeightChanged()
{
	//qDebug() << "HEIGHT_CHANGED____" << height();
	//m_pixmap = QPixmap(width(), height());
	//draw();
	//drawRect();
}

QString CCanvasItem::fillStyle() const
{
	return m_fillStyle;
}

void CCanvasItem::setFillStyle(const QString &newFillStyle)
{
	if (m_fillStyle == newFillStyle)
		return;
	m_fillStyle = newFillStyle;
	emit fillStyleChanged();
}

QString CCanvasItem::strokeStyle() const
{
	return m_strokeStyle;
}

void CCanvasItem::setStrokeStyle(const QString &newStrokeStyle)
{
	if (m_strokeStyle == newStrokeStyle)
		return;
	m_strokeStyle = newStrokeStyle;
	emit strokeStyleChanged();
}

void CCanvasItem::draw(/*QPainter *painter*/)
{
	//qDebug() << "DRAW____";
}



QString CCanvasItem::font() const
{
	return m_font;
}

void CCanvasItem::setFont(const QString &newFont)
{
	if (m_font == newFont)
		return;
	m_font = newFont;
	emit fontChanged();

	int pixelSize = m_font.section("px ",0, 0).toInt();
	QString fontFamily = m_font.section("px ",1, 1);
	m_qfont.setPixelSize(pixelSize);
	m_qfont.setFamily(fontFamily);
}

QString CCanvasItem::lineCap() const
{
	return m_lineCap;
}

void CCanvasItem::setLineCap(const QString &newLineCap)
{
	if (m_lineCap == newLineCap)
		return;
	m_lineCap = newLineCap;
	emit lineCapChanged();
}

QString CCanvasItem::lineJoin() const
{
	return m_lineJoin;
}

void CCanvasItem::setLineJoin(const QString &newLineJoin)
{
	if (m_lineJoin == newLineJoin)
		return;
	m_lineJoin = newLineJoin;
	emit lineJoinChanged();
}

double CCanvasItem::globalAlpha() const
{
	return m_globalAlpha;
}

void CCanvasItem::setGlobalAlpha(double newGlobalAlpha)
{
	if (qFuzzyCompare(m_globalAlpha, newGlobalAlpha))
		return;
	m_globalAlpha = newGlobalAlpha;
	emit globalAlphaChanged();
}




} // namespace imtqml



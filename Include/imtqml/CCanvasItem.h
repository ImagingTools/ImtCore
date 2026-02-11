// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtQuick/QQuickPaintedItem>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>

#include <istd/TDelPtr.h>


namespace imtqml
{


class CCanvasItem : public QQuickPaintedItem
{
	Q_OBJECT

	Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
	Q_PROPERTY(QString fillStyle READ fillStyle WRITE setFillStyle NOTIFY fillStyleChanged)
	Q_PROPERTY(QString strokeStyle READ strokeStyle WRITE setStrokeStyle NOTIFY strokeStyleChanged)
	Q_PROPERTY(QString font READ font WRITE setFont NOTIFY fontChanged)
	Q_PROPERTY(QString lineCap READ lineCap WRITE setLineCap NOTIFY lineCapChanged)
	Q_PROPERTY(QString lineJoin READ lineJoin WRITE setLineJoin NOTIFY lineJoinChanged)
	Q_PROPERTY(double globalAlpha READ globalAlpha WRITE setGlobalAlpha NOTIFY globalAlphaChanged)

public:
	CCanvasItem(QQuickItem *parent = nullptr);
	Q_INVOKABLE void paint(QPainter *painter);
	Q_INVOKABLE void draw(/*QPainter *painter*/);
	Q_INVOKABLE void fillRect(double x, double y, double width, double height);
	Q_INVOKABLE void moveTo(double x, double y);
	Q_INVOKABLE void lineTo(double x, double y);
	Q_INVOKABLE void fillText(QString text,double x, double y);
	Q_INVOKABLE void text(QString text,double x, double y);
	Q_INVOKABLE void arc(double x, double y, double radius, double startAngle, double endAngle, bool anticlockwise);
	Q_INVOKABLE void setPixmapSize(double width, double height);
	Q_INVOKABLE QSize measureText(QString string);
	Q_INVOKABLE void stroke();
	Q_INVOKABLE void fill();
	Q_INVOKABLE void beginPath();
	Q_INVOKABLE void closePath();
	Q_INVOKABLE void save();
	Q_INVOKABLE void restore();
	Q_INVOKABLE void clip();
	Q_INVOKABLE void requestPaint();
;

	double lineWidth() const;
	void setLineWidth(double newLineWidth);

	QString fillStyle() const;
	void setFillStyle(const QString &newFillStyle);

	QString strokeStyle() const;
	void setStrokeStyle(const QString &newStrokeStyle);

	QString font() const;
	void setFont(const QString &newFont);

	QString lineCap() const;
	void setLineCap(const QString &newLineCap);

	QString lineJoin() const;
	void setLineJoin(const QString &newLineJoin);

	double globalAlpha() const;
	void setGlobalAlpha(double newGlobalAlpha);

Q_SIGNALS:
	void paintRequired();

	void lineWidthChanged();

	void fillStyleChanged();

	void strokeStyleChanged();

	void fontChanged();

	void lineCapChanged();

	void lineJoinChanged();

	void globalAlphaChanged();

public slots:
	void onWidthChanged();
	void onHeightChanged();

private:
	QImage m_image;
	QPixmap m_pixmap;
	double m_lineWidth;
	QPointF m_startPoint;
	QString m_fillStyle;
	QPainterPath m_painterPath;
	QString m_strokeStyle;
	QString m_font;
	QFont m_qfont;
	QString m_lineCap;
	QString m_lineJoin;
	double m_globalAlpha;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CCanvasItem*)



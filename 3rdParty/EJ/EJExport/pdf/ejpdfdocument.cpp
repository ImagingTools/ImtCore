#include "ejpdfdocument.h"
#include "export_global.h"

EjPdfDocument::EjPdfDocument(){}

EjPdfDocument::~EjPdfDocument(){}

void EjPdfDocument::setFont(QPainter* painter, QColor color, QFont font)
{
    font.setPixelSize(int(font.pixelSize() * 1.3));
    painter->setFont(font);
    pen.setColor(color);
}

void EjPdfDocument::setColorBackground(QColor color)
{
    m_backgroundColor = color;
}

void EjPdfDocument::addText(QPainter* painter, QString text, int x, int y)
{
    if (!m_fontStyleExist)
    {
        QFont font;
        font.setPixelSize(int(12 * 1.3));
        painter->setFont(font);
    }
    painter->setPen(pen);
    painter->drawText(x, y, text);
}

void EjPdfDocument::drawFillRect(QPainter *painter, int x, int y, int width, int height)
{
    QRect rect(QPoint(x, y), QPoint(x + width/* - 2*/, y + height /*- 2*/));
    painter->fillRect(rect, m_backgroundColor);
}

void EjPdfDocument::drawBorder(QPainter* painter, int x, int y, int widthCell, int heightCell, int widthLeftBorder, int widthRightBorder,
                             int widthTopBorder, int widthBottomBorder)
{
    QPoint point1(x, y), point2(x + widthCell, y), point3(x, y + heightCell/* - 1*/);
    QPen pen;
    if(widthTopBorder != -1)
    {
        pen.setWidth(widthTopBorder / 100);
        painter->setPen(pen);
        painter->drawLine(point1, point2);
    }

    if(widthLeftBorder != -1)
    {
        pen.setWidth(widthLeftBorder / 100);
        painter->setPen(pen);
        painter->drawLine(point1, point3);
    }

    QPoint point(x + widthCell, y + heightCell /*- 1*/);
    if(widthBottomBorder != -1)
    {
        pen.setWidth(widthBottomBorder / 100);
        painter->setPen(pen);
        painter->drawLine(point3, point);
    }

    if(widthRightBorder != -1)
    {
        pen.setWidth(widthRightBorder / 100);
        painter->setPen(pen);
        painter->drawLine(point2, point);
    }
}


void EjPdfDocument::drawImage(QPainter *painter, QImage image, int x, int y)
{
    painter->drawImage(QRect(x, y, int(image.width()*1.3), int(image.height()*1.3)), image);
}

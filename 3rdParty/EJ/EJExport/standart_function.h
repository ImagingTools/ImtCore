#ifndef STANDART_FUNCTION_H
#define STANDART_FUNCTION_H

#include <QString>
#include <QColor>

QString convertColor(QColor const& color);
const QString& convertColorName(QColor const& color, int number_colors, const QColor *colors, const QString *color_names);

#endif // STANDART_FUNCTION_H

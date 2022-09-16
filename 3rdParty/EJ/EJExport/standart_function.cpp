#include "standart_function.h"

QString convertColor(QColor const& color)
{
    QString red_string = QString("%1").arg(color.red(), 0, 16).toUpper();
    red_string.resize(2,'0');
    QString green_string = QString("%1").arg(color.green(), 0, 16).toUpper();
    green_string.resize(2,'0');
    QString blue_string = QString("%1").arg(color.blue(), 0, 16).toUpper();
    blue_string.resize(2,'0');
    return red_string + green_string + blue_string;
}

const QString& convertColorName(QColor const& color, int number_colors, const QColor *colors, const QString *color_names)
{
    int index = 0;
    int min = std::numeric_limits<int>::max();
    for (int i = 0; i < number_colors; ++i)
    {
        QColor const& current_color = colors[i];
        int delta_red = color.red() - current_color.red();
        int delta_green = color.green() - current_color.green();
        int delta_blue = color.blue() - current_color.blue();
        int f = 30*delta_red*delta_red + 59*delta_green*delta_green + 11*delta_blue*delta_blue;
        if (f < min) {
            min = f;
            index = i;
        }
    }
    return color_names[index];
}

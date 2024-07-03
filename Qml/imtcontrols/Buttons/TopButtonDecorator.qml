import QtQuick 2.12
import Acf 1.0
import QtGraphicalEffects 1.0
import Qt5Compat.GraphicalEffects 6.0


ButtonDecorator {
    property bool isEmpty: baseElement.text === "";

    width: isEmpty ? 50 : contentWidth + 2 * Style.size_mainMargin;

    maxTextWidth: 200;
    color: Style.backgroundColor2;
}



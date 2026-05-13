import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

ButtonDecorator {
    property bool isEmpty: baseElement.text === "";
    width: isEmpty ? 50 : contentWidth + 2 * Style.marginM;
	maxTextWidth: Style.sizeHintXXS;
    color: baseElement && baseElement.mouseArea.containsMouse ? Style.baseColor : Style.backgroundColor2;
    icon.width: icon.isEmpty ? 0 : Style.iconSizeM
    border.width: baseElement && baseElement.mouseArea.containsMouse ? 1 : 0;
    contentSpacing: Style.marginXS;
}



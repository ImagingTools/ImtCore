import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ButtonDecorator {
    id: root;

    // A list row, not a button: transparent at rest, list hover and list selection colors otherwise.
    color: !root.baseElement ? "transparent" :
                               root.baseElement.highlighted || root.baseElement.down || root.baseElement.checked ? Style.selectedColor :
                               root.baseElement.hovered ? Style.hover : "transparent"

	maxTextWidth: root.baseElement ? root.baseElement.width - Style.marginM : Style.sizeHintBXS;
	backgroundItem.radius: Style.buttonRadius
	border.width: 0
}



import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ButtonDecorator{
    id: root;

    color: baseElement && baseElement.hovered  ? Style.toolButtonHoverColor : "transparent";
    border.width: 0;

	icon.width: icon.visible ? Style.iconSizeM : 0;
}


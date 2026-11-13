import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ItemDelegateDecorator {
	id: root;

	contentCentered: false;
	contentLeftMargin: !root.baseElement ? Style.marginM : root.baseElement.contentLeftMargin;

	// VS Code menus: the row under the pointer or keyboard takes the accent with light text; the current value keeps the list selection.
	readonly property bool active: !root.baseElement ? false : (root.selected || root.baseElement.down) && root.baseElement.enabled && !root.isSeparator;

	color: !baseElement ? "transparent" :
						  root.active ? Style.buttonPrimaryColor :
						  baseElement.highlighted || baseElement.checked ? Style.selectedColor : "transparent";
	textColor: !baseElement ? Style.textColor :
							  !baseElement.enabled ? Style.inactiveTextColor :
							  root.active ? Style.buttonPrimaryTextColor : Style.textColor;
	border.width: 0;

	property bool isSeparator: !root.baseElement ? false : root.baseElement.isSeparator;
	property bool selected: !root.baseElement ? false : root.baseElement.selected;

	Rectangle {
		id: separator;
		anchors.verticalCenter: root.verticalCenter;
		width: root.width;
		height: 1;
		color: Style.borderColor;
		visible: root.isSeparator;
	}

}

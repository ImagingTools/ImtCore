import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ItemDelegateDecorator {
	id: root;

	contentCentered: false;
	contentLeftMargin: !root.baseElement ? Style.sizeMainMargin : root.baseElement.contentLeftMargin;

	color: !baseElement ? "transparent" :
						  baseElement.highlighted ? Style.selectedColor :
													baseElement.down || baseElement.checked ?
														Style.buttonPressedColor : root.selected ?
															Style.buttonHoverColor : "transparent";
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

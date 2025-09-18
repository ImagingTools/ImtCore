import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

MetaInfoViewDelegateBase {
	id: root

	height: valueText.height

	Text {
		id: valueText
		width: parent.width
		font.family: Style.fontFamily
		font.pixelSize: Style.fontSizeS
		wrapMode: Text.WordWrap
		color: Style.textColor
		elide: Text.ElideRight
		text: root.viewData
	}
}


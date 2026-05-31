import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

MetaInfoViewDelegateBase {
	id: root

	height: valueText.height

	BaseText {
		id: valueText;
		width: parent.width
		font.pixelSize: Style.fontSizeS
		wrapMode: Text.WordWrap

		property string dateStr: root.viewData
		onDateStrChanged: {
			text = new Date(dateStr).toLocaleString()
		}
	}
}


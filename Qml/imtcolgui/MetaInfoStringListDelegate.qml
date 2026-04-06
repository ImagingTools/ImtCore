import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

MetaInfoViewDelegateBase {
	id: root

	height: column.height


	onViewDataChanged: {
		let model_ = []
		let viewData_ = viewData.replace("[", "").replace("]", "").replace(/"/g, "")
		model_ = viewData_.split(",")
		repeater.model = model_
	}

	Column{
		id: column
		width: parent.width
		spacing: Style.spacingM

		Repeater{
			id: repeater

			delegate: Text {
				id: valueText
				width: column.width
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeS
				wrapMode: Text.WordWrap
				color: Style.textColor
				elide: Text.ElideRight
				text: modelData
			}
		}
	}

}


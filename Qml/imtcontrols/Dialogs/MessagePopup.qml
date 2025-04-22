import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Dialog {
	id: message;

	title: qsTr("Message");
	closeAfterPause: true;
	centered: false;

	property bool hiddenBackground: true;
	property bool noMouseArea: true;

	x: ModalDialogManager.activeView.width - contentWidth - Style.sizeMainMargin;
	y: ModalDialogManager.activeView.height - height - Style.sizeMainMargin;

	property string text: "";
	property int contentWidth: 300;

	topPanelComp: Component{TopPanelDialog{canClose: false}}

	contentComp: Component{ Rectangle{
			width: message.contentWidth;
			height: messageText.height + 20;

			color: "transparent";

			radius: 4;
			//border.color: Style.backgroundColor2;

			Text {
				id: messageText;

				anchors.horizontalCenter: parent.horizontalCenter;
				width: parent.width - 40;

				text: message.text;

				font.pixelSize: Style.fontSizeLarge;
				font.family: Style.fontFamily;
				color: Style.textColor;
				wrapMode: Text.WordWrap;

			}

		}}

	backgroundColor: Style.baseColor;
	radius: 2;

}

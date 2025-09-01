import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Dialog {
	id: message;

	title: qsTr("Message");
	closeAfterPause: true;
	centered: false;
	pinned: true;

	property bool hiddenBackground: true;
	property bool noMouseArea: true;

	x: ModalDialogManager.activeView.width - contentWidth - Style.marginM;
	y: ModalDialogManager.activeView.height - height - Style.marginM;

	property string text: "";
	property int contentWidth: Style.sizeHintXS;

	topPanelComp: Component{TopPanelDialog{canClose: false}}

	contentComp: Component{ Rectangle{
			width: message.contentWidth;
			height: messageText.height + Style.marginXL;

			color: "transparent";

			radius: Style.radiusM;
			//border.color: Style.backgroundColor2;

			Text {
				id: messageText;

				anchors.horizontalCenter: parent.horizontalCenter;
				width: parent.width - 2*Style.marginL;

				text: message.text;

				font.pixelSize: Style.fontSizeL;
				font.family: Style.fontFamily;
				color: Style.textColor;
				wrapMode: Text.WordWrap;

			}

		}}

	backgroundColor: Style.baseColor;
	radius: 2;

}

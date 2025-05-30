import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

Rectangle {
    id: inputDialogBodyContainer;

	height: Style.size_minMessageHeight == undefined ? columnBody.height + 2*Style.marginL :
													  Math.max(Style.size_minMessageHeight, columnBody.height + 2*Style.marginL);
    color: "transparent";

    property string message;

    Column {
        id: columnBody;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.left: parent.left;
		anchors.rightMargin: Style.marginXL;
		anchors.leftMargin: Style.marginXL;

        width: inputDialogBodyContainer.width;

		spacing: Style.spacingXS;

        TextEdit {
            id: messageText;

            width: columnBody.width;
            height: contentHeight

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeM;
            wrapMode: Text.WordWrap;
            readOnly: true

            text: inputDialogBodyContainer.message;
        }
    }
}


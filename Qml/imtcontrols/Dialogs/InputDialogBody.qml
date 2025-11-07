import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
    id: inputDialogBodyContainer;

	height: columnBody.height + 2*Style.marginXL;

    property string message;
    property string inputValue: "";
    property Item rootItem: null;
    property alias placeHolderText: inputField.placeHolderText;

	property bool selectTextOnStart: false;

    Component.onCompleted: {
        inputField.forceActiveFocus();
    }

    Column {
        id: columnBody;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.rightMargin: Style.marginM;
        anchors.left: parent.left;
        anchors.leftMargin: Style.marginM;

        spacing: 5;

        Text {
            id: message;

            width: columnBody.width;

            text: inputDialogBodyContainer.message;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeM;
            elide: Text.ElideRight;

            wrapMode: Text.WordWrap;
        }

        CustomTextField {
            id: inputField;

            anchors.horizontalCenter: parent.horizontalCenter;

            width: columnBody.width;
            height: 30;

            text: inputDialogBodyContainer.inputValue;

			property bool wasSelectedOnStart: false;

            onTextChanged: {
                inputDialogBodyContainer.inputValue = inputField.text;
				if(inputDialogBodyContainer.selectTextOnStart && !wasSelectedOnStart){
					selectAll()
					wasSelectedOnStart = true;
				}
            }

			Component.onCompleted: {

			}

            onAccepted: {
                inputDialogBodyContainer.rootItem.buttons.buttonClicked(Enums.ok);
            }
        }
    }
}


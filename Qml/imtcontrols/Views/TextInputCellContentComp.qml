import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Component {
    id: textInputComp;

    Item {
        id: item;

        property Item tableCellDelegate: null;

        z: parent.z + 1;

        width: parent.width;
        height: 25;

        onTableCellDelegateChanged: {
            if (tableCellDelegate){
                textInput.text = tableCellDelegate.getValue();
                textLabel.text = tableCellDelegate.getValue();

            }
        }

        Text {
            id: textLabel;

            anchors.verticalCenter: parent.verticalCenter;

            width: parent.width;

            elide: Text.ElideRight;
            wrapMode: Text.NoWrap;

            clip: true;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: textInput;

            anchors.verticalCenter: parent.verticalCenter;

            width: parent.width;
            height: 25;

            visible: false;

            onEditingFinished: {
                console.log("onEditingFinished")

                textInput.visible = false;

                if (item.tableCellDelegate){
                    textLabel.text = textInput.text;
                    item.tableCellDelegate.setValue(textInput.text);
                }
            }
        }

        MouseArea {
            id: ma;

            z: textInput.z + 1;

            anchors.fill: parent;

            onClicked: {
                mouse.accepted = true;
            }

            onDoubleClicked: {
                console.log("ma onDoubleClicked");
                mouse.accepted = true;

                textInput.visible = true;

                textInput.forceActiveFocus();
            }
        }
    }
}


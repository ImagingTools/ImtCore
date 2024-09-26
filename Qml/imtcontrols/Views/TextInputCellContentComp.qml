import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Component {
    id: textInputComp;

    TableCellDelegateBase {
        id: item;

        onRowIndexChanged: {
            if (rowIndex >= 0){
                let valueModel = getValue();
                textInput.text = valueModel
                textLabel.text = valueModel
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
                let data = textInput.text
                console.log("onEditingFinished", data)
                textInput.visible = false;
                textLabel.text = textInput.text;
                item.setValue(textInput.text);
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


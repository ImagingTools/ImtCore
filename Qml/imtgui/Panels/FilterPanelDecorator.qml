import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: filterPanelDecorator;

    height: content.height;

    Row {
        id: content;

        anchors.right: filterPanelDecorator.right;
        height: tfc.height;
        spacing: Style.size_mainMargin;

        CustomTextField {
            id: tfc;

            anchors.verticalCenter: content.verticalCenter;

            width: 270;
            height: 30;

            placeHolderText: qsTr("Enter some text to filter the item list");

            onTextChanged: {
                timer.restart();
            }

            Timer {
                id: timer;

                interval: 200;

                onTriggered: {
                    filterPanelDecorator.baseElement.filterChanged("TextFilter", tfc.text);
                }
            }

            ToolButton {
                id: iconClear;

                z: tfc.z + 1;

                anchors.right: tfc.right;
                anchors.rightMargin: Style.margin;
                anchors.verticalCenter: tfc.verticalCenter;

                width: Style.buttonWidthSmall;
                height: width;

                visible: tfc.text != "";

                iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

                onClicked: {
                    console.log("iconClear onClicked");

                    tfc.text = "";
                }
            }
        }

        ToolButton {
            id: closeButton;

            anchors.verticalCenter: content.verticalCenter;

            width: Style.buttonWidthMedium;
            height: width;

            iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

            onClicked: {
                tfc.text = "";
                filterPanelDecorator.baseElement.close();
            }
        }
    }
}


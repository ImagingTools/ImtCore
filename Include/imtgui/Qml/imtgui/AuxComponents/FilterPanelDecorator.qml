import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Item {
    id: filterPanelDecorator;

    height: 30;

    onVisibleChanged: {
        console.log("filterPanelDecorator", filterPanelDecorator.visible);

        animRect.start();
    }

    NumberAnimation {
        id: animRect;
        target: filterPanelDecorator;
        property: "height";
        from: 0;
        to: filterPanelDecorator.height;
        duration: 500;
    }

    Row {
        id: rowFilter;

        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter;

        height: 25;

        spacing: 5;

        CustomTextField {
            id: tfc;
            anchors.verticalCenter: parent.verticalCenter;

            width: 400;
            height: 25;

            placeHolderText: qsTr("Enter some text to filter the item list");

            onTextChanged: {
                loaderDecorator.textChanged(model.index, tfc.text);
            }

            AuxButton {
                id: iconClear;

                anchors.right: parent.right;
                anchors.rightMargin: 5;
                anchors.verticalCenter: parent.verticalCenter;

                height: 15;
                width: height;

                visible: tfc.text != "";

                iconSource: "../../../" + "Icons/" + Style.theme + "/Close_On_Normal.svg";

                onClicked: {
                    tfc.text = "";
                }
            }
        }

        AuxButton {
            id: closeButton;

            anchors.verticalCenter: parent.verticalCenter;

            height: 20;
            width: height;

            iconSource: "../../../" + "Icons/" + Style.theme + "/Close_On_Normal.svg";

            onClicked: {
                tfc.text = "";
                loaderDecorator.onClosed();
            }
        }
    }
}


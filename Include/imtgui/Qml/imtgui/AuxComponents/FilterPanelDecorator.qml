import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: filterPanelDecorator;

    height: 30;

    property Item rootLoader: null;

    onVisibleChanged: {
        console.log("filterPanelDecorator", filterPanelDecorator.visible);

        if (filterPanelDecorator.visible){
            animRect.start();
        }
    }

    onFocusChanged: {
        if (filterPanelDecorator.focus){
            console.log("filterPanelDecorator.focus", filterPanelDecorator.focus);
            console.log("tfc.focus1", tfc.focus);
          //  tfc.focus = filterPanelDecorator.focus;

            tfc.forceActiveFocus();
            console.log("tfc.focus2", tfc.focus);
        }
    }

    NumberAnimation {
        id: animRect;
        target: tfc;
        property: "width";
        from: 0;
        to: tfc.width;
        duration: 300;
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

            width: 300;
            height: 25;

            placeHolderText: qsTr("Enter some text to filter the item list");

            onTextChanged: {
                filterPanelDecorator.rootLoader.textChanged(model.index, tfc.text);
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
                filterPanelDecorator.rootLoader.onClosed();
            }
        }
    }
}

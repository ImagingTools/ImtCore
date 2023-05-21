import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: filterPanelDecorator;

    height: 30;
    width: 325;

    property Item rootLoader: null;

    function calcWidth(){
        console.log("calcWidth");

        if (filterPanelDecorator.rootLoader){
            console.log("parent.width", filterPanelDecorator.rootLoader.parent.width);
            let width = filterPanelDecorator.rootLoader.parent.width - 30;
            if (width <= tfc.width){
                tfc.width = width;
            }
            else{
                tfc.width = 270;
            }

            console.log("tfc.width", tfc.width);
        }
    }

    onVisibleChanged: {
        console.log("onVisibleChanged", tfc.width);

        if (filterPanelDecorator.visible){
            animRect.to = tfc.width;
            animRect.start();
        }
    }

    onFocusChanged: {
        if (filterPanelDecorator.focus){
            tfc.forceActiveFocus();
        }
    }

    NumberAnimation {
        id: animRect;
        target: tfc;
        property: "width";
        from: 0;
//        to: tfc.width;
        duration: 200;
    }

    CustomTextField {
        id: tfc;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: closeButton.left;
        anchors.rightMargin: 5;

        width: 270;
        height: filterPanelDecorator.height;

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
        anchors.right: parent.right;

        height: 20;
        width: height;

        iconSource: "../../../" + "Icons/" + Style.theme + "/Close_On_Normal.svg";

        onClicked: {
            tfc.text = "";
            filterPanelDecorator.rootLoader.onClosed();
        }
    }

//    Row {
//        id: rowFilter;

//        anchors.right: parent.right;
//        anchors.verticalCenter: parent.verticalCenter;

//        height: filterPanelDecorator.height;

//        spacing: 5;

//        CustomTextField {
//            id: tfc;

//            width: 300;
//            height: rowFilter.height;

//            placeHolderText: qsTr("Enter some text to filter the item list");

//            onTextChanged: {
//                filterPanelDecorator.rootLoader.textChanged(model.index, tfc.text);
//            }

//            AuxButton {
//                id: iconClear;

//                anchors.right: parent.right;
//                anchors.rightMargin: 5;
//                anchors.verticalCenter: parent.verticalCenter;

//                height: 15;
//                width: height;

//                visible: tfc.text != "";

//                iconSource: "../../../" + "Icons/" + Style.theme + "/Close_On_Normal.svg";

//                onClicked: {
//                    tfc.text = "";
//                }
//            }
//        }

//        AuxButton {
//            id: closeButton;

//            anchors.verticalCenter: parent.verticalCenter;

//            height: 20;
//            width: height;

//            iconSource: "../../../" + "Icons/" + Style.theme + "/Close_On_Normal.svg";

//            onClicked: {
//                tfc.text = "";
//                filterPanelDecorator.rootLoader.onClosed();
//            }
//        }
//    }
}

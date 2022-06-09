import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Item {
    id: filterPanelDecorator;

    anchors.fill: parent;

    property int itemWidth: 150;
    property int itemHeight: 30;

//    signal textFilterChanged(int index, string text);

//    TextFieldCustom {
//        id: searchTextField;

//        anchors.left: parent.left;
//        anchors.leftMargin: 5;
//        anchors.verticalCenter: parent.verticalCenter;

//        radius: 5;
//        width: 200;
//        height: 30;

//        onTextChanged: {
////            filterPanelDecorator.textFilterChanged(model.index, tfc.text);
//            loaderDecorator.textChanged(model.index, searchTextField.text);
//        }
//    }





//LISA DECORATOR
    TextFieldCustom {
        id: tfc;

        anchors.right: parent.right;
        anchors.rightMargin: 5;
        anchors.verticalCenter: parent.verticalCenter;

        width: 200;
        height: filterPanelDecorator.height - 5;

        onTextChanged: {
//            filterPanelDecorator.textFilterChanged(model.index, tfc.text);
            loaderDecorator.textChanged(model.index, tfc.text);
        }

        Image {
            id: iconClear;

            anchors.right: parent.right;
            anchors.rightMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;

            height: 10;
            width: 10;

            sourceSize.height: height;
            sourceSize.width: width;

            source: "../../../" + "Icons/" + Style.theme + "/Close_On_Normal.svg";

            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    tfc.text = "";
                }
            }
        }
    }
}


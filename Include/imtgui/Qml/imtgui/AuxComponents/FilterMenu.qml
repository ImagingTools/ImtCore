import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Rectangle {
    id: filterContainer;

//    width: 500;
//    height: 30;

    color: Style.backgroundColor;

    property int countFilters: 4;

    property int itemWidth: filterContainer.width / filterContainer.countFilters;
    property int itemHeight: 30;

    signal textFilterChanged(int index, string text);
    property string decoratorSource;

    onDecoratorSourceChanged: {
        console.log("loaderDecorator.source", filterContainer.decoratorSource);
        loaderDecorator.source = filterContainer.decoratorSource;
    }

    Loader {
        id: loaderDecorator;

        anchors.fill: parent;

        function textChanged(index, text){
            filterContainer.textFilterChanged(index, text);
        }

        onItemChanged: {
            console.log("onItemChanged");
            if (loaderDecorator.item){
                loaderDecorator.item.height = filterContainer.height;
                loaderDecorator.item.width = filterContainer.width;
                console.log("FilterMenu");
                console.log("filterContainer.heightt", filterContainer.height);
                console.log("filterContainer.width", filterContainer.width);
                console.log("loaderDecorator.item.height", loaderDecorator.item.height);
                console.log("loaderDecorator.item.width", loaderDecorator.item.width);
            }
        }

    }
//    TextFieldCustom {
//        id: tfc;

//        anchors.right: parent.right;
//        anchors.rightMargin: 5;
//        anchors.verticalCenter: parent.verticalCenter;

//        width: 200;
//        height: filterContainer.height - 5;

//        onTextChanged: {
//            filterContainer.textFilterChanged(model.index, tfc.text);
//        }

//        Image {
//            id: iconClear;

//            anchors.right: parent.right;
//            anchors.rightMargin: 5;
//            anchors.verticalCenter: parent.verticalCenter;

//            height: 10;
//            width: 10;

//            sourceSize.height: height;
//            sourceSize.width: width;

//            source: "../../../" + "Icons/" + Style.theme + "/Close_On_Normal.svg";

//            MouseArea {
//                anchors.fill: parent;

//                onClicked: {
//                    tfc.text = "";
//                }
//            }
//        }
//    }
}

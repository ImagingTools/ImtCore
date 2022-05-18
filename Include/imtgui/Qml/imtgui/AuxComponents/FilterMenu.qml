import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Rectangle {
    id: filterContainer;

    width: 500;
    height: 30;

    color: Style.baseColor;

    property int countFilters: 4;

    property int itemWidth: filterContainer.width / filterContainer.countFilters;
    property int itemHeight: 30;

    signal textFilterChanged(int index, string text);

    Row {
        anchors.fill: parent;

        Repeater {
            id: filterRepeater;

            model: 4;
            delegate: Rectangle {
                id: filterDelegate;

                width: filterContainer.itemWidth;
                height: filterContainer.height;

                TextFieldCustom {
                    id: tfc;
                    width: filterDelegate.width - 20;
                    height: filterDelegate.height - 6;

                    onTextChanged: {
                        filterContainer.textFilterChanged(model.index, tfc.text);
                    }
                }

//                Rectangle {
//                    id: close;

//                    anchors.right: filterDelegate.right;
//                    anchors.verticalCenter: parent.verticalCenter;

//                    width: 5;
//                    height: 5;

//                    color: "red";
//                }
            }
        }
    }

//    ListView {
//        id: filtersList;

//        anchors.fill: parent;

//        clip: true;
//        model: 4;
//        orientation: ListView.Horizontal;
//        boundsBehavior: Flickable.StopAtBounds;
//        spacing: 0;

//        delegate: Rectangle {
//            id: filterDelegate;

//            width: filterContainer.itemWidth;
//            height: filterContainer.height;

//            TextFieldCustom {
//                width: filterDelegate.width - 5;
//                height: filterDelegate.height - 2;
//            }

//            Rectangle {
//                id: close;

//                anchors.right: filterDelegate.right;
//                anchors.verticalCenter: parent.verticalCenter;

//                width: 5;
//                height: 5;

//                color: "red";
//            }

//        }
//    }
}

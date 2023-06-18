import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import QtQuick.Controls 2.15
import QtWebView 1.1

Item{

//     WebView{
//     id: webView;
//     anchors.fill: parent;
//     // url: '/html/map.html';
//    // url: "/html/map.html";
//    url: 'https://www.openstreetmap.org/';

//     // z: 1000;
//     }

CustomScrollbar{
        id: scrollBar;

        anchors.left: testList.right;
//        anchors.rightMargin: - secondSize -8;
        anchors.bottom: parent.bottom;
        targetItem: testList;
        secondSize: 8;
        radius: secondSize;
        indicatorRadius: secondSize;
        indicatorMargin: 0;
        indicatorColor: "gray";
}



ListView {
            id: testList;
            model: 10000;
            width: parent.width - 10;
            height: parent.height;
            anchors.horizontalCenter: parent.horizontalCenter;
            cacheBuffer: 0
            // ScrollBar: ScrollBar{}
        //     ScrollBar.vertical: ScrollBar {
        //         parent: testList.parent
        //  anchors.top: testList.top
        //  anchors.right: testList.left
        //  anchors.bottom: testList.bottom
        //         }

            delegate: Rectangle {
                id: rectDelegate
                width: testList.width;
                height: 50 + model.index * 0.0;
                color: model.index % 2 === 0 ? 'red' : 'yellow';

                onYChanged: {
                    console.log('Rectangle ', model.index, 'y =', y)
                }

//                Text {
//                    text: 'model.id = ' + model.index;
//                    anchors.centerIn: parent;
//                }

                Row {
                    height: parent.height;

                    spacing: 2;
                    Repeater {
                        model: 10;
                        delegate: Rectangle {
                            width: rectDelegate.width / 10 - 2;
                            height: rectDelegate.height - 10;
                            color: 'blue';
                            Text {
                                text: 'model.id = ' + model.index;
                                anchors.centerIn: parent;
                            }
                        }
                    }
                }

                MouseArea {
                    width: parent.width / 2
                    height: parent.height
                    property bool isOpen: false
                    onClicked: {
                       //testList.contentY += 1000
                       if (!isOpen){
                         rectDelegate.height = rectDelegate.height * 2
                       }
                       else{
                         rectDelegate.height = rectDelegate.height / 2
                       }
                       isOpen = !isOpen
                    }
                }

                 MouseArea {
                    width: parent.width / 2
                    height: parent.height
                    x: parent.width / 2
                    onClicked: {
                        label.text = model.index + " originY: " +  (rectDelegate.y - model.index * testList.contentHeight / testList.count)
                    }
                }
            }

            onContentYChanged: {
                console.log('contentY =', contentY, 'contentHeight =', contentHeight)
            }
    }
}

// CollectionView {
//     id: packageCollectionViewContainer;

//     Component.onCompleted: {
//         console.log("PackageCollection onCompleted", model.index);
//         packageCollectionViewContainer.commandUpdateGui = "PackageCollectionUpdateGui";
//         packageCollectionViewContainer.commandsDelegatePath = "../../imtlicgui/PackageCollectionViewCommandsDelegate.qml";
//     }
// }

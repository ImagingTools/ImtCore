import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: testMultiDocViewContainer;

    anchors.fill: parent;

    property int number: 0;

    Calendar {
        id: calendar;
    }

//    AuxButton{
//        id: add;

//        width: 50;
//        height: 25;

//        textButton: "Add";

//        hasText: true;

//        borderColor: "#000000";

//        onClicked: {

//            for (var i = 0; i < 5000; i++)
//            {
//                modelTest.append({"Text": testMultiDocViewContainer.number});
////                modelTest.InsertNewItem();
////                modelTest.SetData("Text", testMultiDocViewContainer.number, i);
//                testMultiDocViewContainer.number++;
//            }

//            //var index = modelTest.InsertNewItem();
//        }
//    }

//    AuxButton{
//        id: del;

//        anchors.left: add.right;
//        anchors.leftMargin: 10;

//        width: 50;
//        height: 25;

//        textButton: "Del";

//        hasText: true;

//        borderColor: "#000000";

//        onClicked: {
//            //modelTest.RemoveItem(testMultiDocViewContainer.number - 1);
//          //  testMultiDocViewContainer.number--;

//            for (var i = 0; i < 5000; i++)
//            {
//                //modelTest.RemoveItem(0);
//                modelTest.remove(0);
//            }

//            testMultiDocViewContainer.number = 0;
//        }
//    }

////    TreeItemModel {
////        id: modelTest;
////    }

//    ListModel {
//        id: modelTest;
//    }


//    ListView {
//        id: listview;

//        anchors.top: del.bottom;
//        anchors.topMargin: 10;
//        anchors.left: parent.left;
//        width: 200;

//        height: 500;

//        model: modelTest;

//        delegate: Rectangle{
//            width: listview.width;
//            height: 30;

//            color: "#c0c0c0";

//            Text {
//                id: text;

//                anchors.horizontalCenter: parent.horizontalCenter;
//                anchors.verticalCenter: parent.verticalCenter;

//                text: model.Text;
//            }
//        }

//    }
}

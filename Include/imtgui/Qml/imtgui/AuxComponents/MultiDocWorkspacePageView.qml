import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: multiDocPageView;

    anchors.fill: parent;

//    color: 'transparent';

//    TreeItemModel {
//        id: comboBoxModel;
//        Component.onCompleted: {
//            SetData("Id", "data0", 0)
//            SetData("Name", "data0", 0)
//            InsertNewItem()
//            SetData("Id", "data1", 1)
//            SetData("Name", "data1", 1)

//        }
//    }
//    ComboBox {
//        width: 100;
//        model: comboBoxModel;
//    }

//    ListModel {
//        id: testListModel;

//        Component.onCompleted: {
//            append({'id': 0, 'text': 'name1', 'color': 'red'})
//        }
//    }

//    ListView {
//        model: testListModel;
//        width: 100;
//        height: 500;
//        spacing: 10;
//        x: 100;

//        delegate: Rectangle {
//            width: parent.width;
//            height: 100;
//            color: model.color;
//            Text {
//                anchors.centerIn: parent;
//                text: model.text;
//            }
//        }
//    }


//    Loader {
//        id: loader;

//        anchors.top: rectSeparator.bottom;
//        anchors.topMargin: 15;

//        Component.onCompleted: {
//            loader.source = "SettingsComboBox.qml";
//            loader.item.width = 200;
//            loader.item.parameters = comboBoxModel

//        }

//    }

//    Column {
//        Repeater{
//            model: 5;
//            delegate: Text {
//                text: 'jergerhtu4 54uh654785y 437592837f45y34 87f5y238 475y34785y324857 2fy349573y9 jergerhtu4 54uh654785y 437592837f45y34 87f5y238 475y34785y324857 2fy349573y9 jergerhtu4 54uh654785y 437592837f45y34 87f5y238 475y34785y324857 2fy349573y9'
//                width: multiDocPageView.width - 100;
//                wrapMode: Text.WordWrap;
//            }
//        }



//    }

    property var startPageObj;

    property MainDocumentManager mainDocumentManager: null;

    onMainDocumentManagerChanged: {
        console.log("multiDocPageView onMainDocumentManagerChanged" , mainDocumentManager);
    }

    onStartPageObjChanged: {
        console.log("multiDocPageView onStartPageObjChanged");
        documentManager.addDocument(multiDocPageView.startPageObj)

        if (multiDocPageView.mainDocumentManager != null){
            multiDocPageView.mainDocumentManager.registerDocumentManager(multiDocPageView.startPageObj["CommandsId"], documentManager);
        }
    }

    MultiDocWorkspaceView {
        id: documentManager;

        anchors.fill: parent;

        mainDocumentManager: multiDocPageView.mainDocumentManager;
    }
}

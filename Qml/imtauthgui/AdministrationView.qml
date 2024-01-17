import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import Qt.labs.qmlmodels 1.1

Rectangle {
    id: container;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property Item documentManagerPtr: null;

    function addHeader(header){
        headerText.text += "/ " + header;
    }

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    Component.onDestruction: {
        console.log("Administration onDestruction");

        Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        console.log("Administration onLocalizationChanged", language);

        let rolesIndex = multiPageView.getIndexById("Roles");
        if (rolesIndex >= 0){
            multiPageView.pagesModel.setProperty(rolesIndex, "Name", qsTr("Roles"))
        }

        let usersIndex = multiPageView.getIndexById("Users");
        if (usersIndex >= 0){
            multiPageView.pagesModel.setProperty(usersIndex, "Name", qsTr("Users"))
        }

        let groupsIndex = multiPageView.getIndexById("Groups");
        if (groupsIndex >= 0){
            multiPageView.pagesModel.setProperty(groupsIndex, "Name", qsTr("Groups"))
        }

        if (multiPageView.currentIndex >= 0){
            headerText.text = qsTr("Administration") + " / " + multiPageView.pagesModel.get(multiPageView.currentIndex).Name;
        }
    }

    Row {
        id: header;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        height: 40

        spacing: 20;

        Text {
            id: headerText;

            anchors.verticalCenter: parent.verticalCenter;

            font.pixelSize: Style.fontSize_title;
            font.family: Style.fontFamily;

            color: Style.titleColor;
        }
    }

    Component {
        id: roleCollectionComp;

        RoleCollectionView {
            documentManagerPtr: container.documentManagerPtr;
        }
    }

    Component {
        id: userCollectionComp;

        UserCollectionView {
            documentManagerPtr: container.documentManagerPtr;
        }
    }

    Component {
        id: userGroupCollectionComp;

        UserGroupCollectionView {
            documentManagerPtr: container.documentManagerPtr;
        }
    }

//    TableView {
//      id: usersTable

//      model: TableModel {
//                   TableModelColumn { display: "id" }
//                   TableModelColumn { display: "name" }

//                   rows: [ { "id": "Harry", "name": "Harry" }, { "id": "Hedwig", "name": "Hedwig" } ]
//               }

//      anchors.top: header.bottom;
//      anchors.left: parent.left;
//      anchors.right: parent.right;
//      anchors.bottom: parent.bottom;

//      columnSpacing: 1
//               rowSpacing: 1
//               boundsBehavior: Flickable.StopAtBounds

//      delegate: DelegateChooser
//      {
//        DelegateChoice
//        {
//          delegate: Rectangle {
//            implicitWidth: 100
//            implicitHeight: 50
//            Text { text: display }

//            MouseArea {
//              anchors.fill: parent
//              onClicked:
//              {
//                // print value from clicked cell
//                var idx = usersTable.model.index(row,column)
//                console.log("Clicked cell: ", usersTable.model.data(idx))

//                // print values from all cells in a row
//                console.log("Clicked row: ")
//                for (var i = 0; i < usersTable.model.columnCount(); i++)
//                {
//                  var idx2 = usersTable.model.index(row,i)
//                  var data = usersTable.model.data(idx2)
//                  console.log(data)
//                }
//              } // onClicked
//            } // MouseArea
//          } // Rectangle
//        }// DelegateChoice
//      } // DelegateChooser
//    } // TableView

    MultiPageView {
        id: multiPageView;

        anchors.top: header.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        Component.onCompleted: {
            updateModel();
        }

        onCurrentIndexChanged: {
            console.log("onSelectedIndexChanged", currentIndex);

            if (currentIndex >= 0){
                headerText.text = qsTr("Administration") + " / " + pagesModel.get(currentIndex).Name;
            }
        }

        function updateModel(){
            multiPageView.clear();

            multiPageView.addPage("Roles", qsTr("Roles"), roleCollectionComp);
            multiPageView.addPage("Users", qsTr("Users"), userCollectionComp);
            multiPageView.addPage("Groups", qsTr("Groups"), userGroupCollectionComp);

            multiPageView.currentIndex = 0;
        }
    }
}

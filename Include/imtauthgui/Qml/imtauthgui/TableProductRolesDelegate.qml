import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

Item {
    id: productRolesDelegate;

    height: body.height + rolesColumn.height;

    property string commandsId;

    property int index: model.index;

    property int selectedIndex: -1;

    property bool newIsEnabled: true;

    property Item baseCollectionView: null;

    signal clicked(int index);
    signal doubleClicked(string id, string name, int index);

    Component.onCompleted: {
        if (model.Roles){
            rolesRepeater.model = model.Roles;
        }
    }

    onSelectedIndexChanged: {
        if (productRolesDelegate.selectedIndex != productRolesDelegate.index){
            rolesRepeater.selectedIndex = -1;
        }
    }

    function getSelectedId(){
        let elements = model.Roles;

        let roleId = elements.GetData("Id", rolesRepeater.selectedIndex);

        return roleId;
    }

    function getSelectedName(){
        let elements = model.Roles;

        let roleName = elements.GetData("Name", rolesRepeater.selectedIndex);

        return roleName;
    }

    Rectangle {
        id: body;

        width: productRolesDelegate.width - 10;
        height: 30;

        color: Style.alternateBaseColor;

        border.width: 1;
        border.color: Style.imagingToolsGradient2;

        Text {
            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 10;

            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamilyBold;
            font.bold: true;

            color: Style.textColor;
            wrapMode: Text.WordWrap;
            elide: Text.ElideRight;

            text: model.Name;
        }

        AuxButton {
            id: buttonNew;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 10;

            width: 20;
            height: width;

            visible: productRolesDelegate.newIsEnabled;

            iconSource: "../../../../" + "Icons/" + Style.theme + "/Add_Off_Normal.svg";

            onClicked: {
                console.log("productRolesDelegate.index", productRolesDelegate.index);
              //  productRolesDelegate.clicked(productRolesDelegate.index);
                Events.sendEvent(productRolesDelegate.commandsId + "CommandActivated", "New");
            }
        }
    }

    Rectangle {
        id: rolesBg;

        anchors.top: body.bottom;

        color: "transparent";

        width: body.width;
        height: rolesColumn.height + 2;

        border.color: Style.imagingToolsGradient2;
        border.width: 1;

        Column {
            id: rolesColumn;

            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;

            width: rolesBg.width - rolesBg.border.width * 2;

            Repeater {
                id: rolesRepeater;

                property int selectedIndex: -1;

                onSelectedIndexChanged: {
                    if (productRolesDelegate.selectedIndex == productRolesDelegate.index){
                        productRolesDelegate.baseCollectionView.selectedIndexChanged(rolesRepeater.selectedIndex);
                    }
                }

                delegate: Rectangle {
                    width: rolesColumn.width;
                    height: body.height;

                    property bool selected: false;

                    color: this.selected ? Style.selectedColor : "transparent";

                    Component.onCompleted: {
                        let table = productRolesDelegate.baseCollectionView.table;
                        table.tableSelection.selectionChanged.connect(this.selectionChanged);
                    }

                    Component.onDestruction: {
                        let table = productRolesDelegate.baseCollectionView.table;
                        table.tableSelection.selectionChanged.disconnect(this.selectionChanged);
                    }

                    function selectionChanged(){
                        let table = productRolesDelegate.baseCollectionView.table;
                        selected = table.tableSelection.selectedIndexes.includes(model.index);
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        anchors.leftMargin: 10;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                        color: Style.textColor;
                        wrapMode: Text.WordWrap;
                        elide: Text.ElideRight;

                        text: model.Name;
                    }

                    MouseArea {
                        id: delegMa;

                        anchors.fill: parent;

                        onClicked: {
                            rolesRepeater.selectedIndex = model.index;

                            productRolesDelegate.clicked(rolesRepeater.selectedIndex);
                        }

                        onDoubleClicked: {
                            rolesRepeater.selectedIndex = model.index;

                            productRolesDelegate.doubleClicked(model.Id, model.Name, model.index);
                        }
                    }
                }
            }
        }//Column
    }
}

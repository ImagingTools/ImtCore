import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

FocusScope {
    id: container;

    height: bodyColumn.height + 40;

    property TreeItemModel rolesModel: TreeItemModel {}
    property string activeRoleIds: ""

    property Item rootItem: null;

    Component.onDestruction: {
        container.rolesModel.modelChanged.disconnect(container.updateModel);
    }

    onFocusChanged: {
        console.log("InputBody onFocusChanged", container.focus);

        if (container.focus){
            inputName.focus = container.focus;
        }
    }

    function started(){
        let activeRoleList = container.activeRoleIds.split(';');
        for (let i = 0; i < container.rolesModel.GetItemsCount(); i++){
            let id = container.rolesModel.GetData("Id", i);
            if (activeRoleList.includes(id)){
                container.rolesModel.SetData("CheckedState", Qt.Checked, i);
            }
        }

        container.rolesModel.modelChanged.connect(container.updateModel);

        rolesTable.elements = container.rolesModel;
    }

    function updateModel(){
        let activeRoleList = [];
        for (let i = 0; i < container.rolesModel.GetItemsCount(); i++){
            let id = container.rolesModel.GetData("Id", i);
            let state = container.rolesModel.GetData("CheckedState", i);
            if (state === Qt.Checked){
                activeRoleList.push(id);
            }
        }

        container.activeRoleIds = activeRoleList.join(';')
    }

    Column {
        id: bodyColumn;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.left: parent.left;
        anchors.rightMargin: 10;
        anchors.leftMargin: 10;

        width: container.width;

        spacing: 5;

        Text {
            id: titleName;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Group Name");
        }

        CustomTextField {
            id: inputName;

            width: bodyColumn.width;
            height: 30;

            text: container.rootItem.groupName;

            KeyNavigation.tab: descriptionInput;

            onTextChanged: {
                container.rootItem.groupName = inputName.text;
            }
        }

        Text {
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Group Description");
        }

        CustomTextField {
            id: descriptionInput;

            width: bodyColumn.width;
            height: 30;

            text: container.rootItem.groupDescription;

            onTextChanged: {
                container.rootItem.groupDescription = descriptionInput.text;
            }

            KeyNavigation.tab: inputName;
        }

        Text {
            id: titleRoles;

            text: qsTr("Group Roles");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        TreeItemModel {
            id: headersModel;

            Component.onCompleted: {
                headersModel.InsertNewItem();

                headersModel.SetData("Id", "Name");
                headersModel.SetData("Name", "Role Name");

                rolesTable.headers = headersModel;
            }
        }

        AuxTable {
            id: rolesTable;

            width: parent.width;
            height: 300;

            checkable: true;
            radius: 0;
        }
    }
}

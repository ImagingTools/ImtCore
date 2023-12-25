import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Dialog {
    id: editDialogContainer;

    width: 300;

    property string groupName: "";
    property string groupDescription: "";

    property TreeItemModel rolesModel: TreeItemModel {}
    property string activeRoleIds: ""

    contentComp: Component { EditDialogBody {
        id: dialogBody;

        rolesModel: editDialogContainer.rolesModel;

        rootItem: editDialogContainer;
        activeRoleIds: editDialogContainer.activeRoleIds;
    } }

    Component.onCompleted: {
        editDialogContainer.buttons.addButton({"Id": "Ok", "Name": "OK", "Enabled": true});
        editDialogContainer.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        editDialogContainer.title = qsTr("Group Editor");
    }
}

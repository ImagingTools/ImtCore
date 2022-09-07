import QtQuick 2.12
import Acf 1.0

Item {
    id: container;

    signal checkBoxPermissionClicked(string itemId, int modelIndex, int state);

    onCheckBoxPermissionClicked: {
        console.log("RolePermissions onCheckBoxPermissionsClicked", itemId, modelIndex, state);
        let activePermissionsModel = documentModel.GetData("Permissions");
        activePermissionsModel.SetData("PermissionId", state, modelIndex);
    }
}

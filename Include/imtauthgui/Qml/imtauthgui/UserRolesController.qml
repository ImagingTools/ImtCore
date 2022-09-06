import QtQuick 2.12
import Acf 1.0

Item {
    id: container;

    signal checkBoxRoleClicked(string itemId, int modelIndex, int state);

    onCheckBoxRoleClicked: {
        console.log("UserRoles onCheckBoxRoleClicked", itemId, modelIndex, state);
        let activeRolesModel = documentModel.GetData("Roles");
        activeRolesModel.SetData("RoleId", state, modelIndex);
    }
}

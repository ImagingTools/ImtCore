import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegate {
    id: container;

    showInputIdDialog: false;

    onCommandActivated: {
        console.log("RolesCommands onCommandActivated", commandId);
    }

    onSelectedIndexChanged: {
        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";
    }

    onClosed: {
        Events.sendEvent("LicenseFeaturesUpdate");
    }

    onEdited: {
        let elementsModel = tableData.elements;
        let oldUserId = itemId;
        let newUserId = elementsModel.GetData("Id", selectedIndex);

        if (oldUserId != newUserId){
            lisensesFeaturesModel.updateLicensesDependenciesAfterLicenseEditing(oldLicenseId, newLicenseId);
        }
    }
}


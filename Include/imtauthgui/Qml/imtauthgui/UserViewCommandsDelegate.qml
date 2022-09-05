import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegate {
    id: container;

    showInputIdDialog: false;

    onCommandActivated: {
        console.log("UsersCommands onCommandActivated", commandId);

        if (commandId === "Duplicate"){
            let elements = container.tableData.elements;

            let selectedId = elements.GetData("Id", container.selectedIndex);
            let selectedName = elements.GetData("Name", container.selectedIndex);
            let selectedDescription = elements.GetData("Description", container.selectedIndex);

            let index = elements.InsertNewItem();
            elements.SetData("Name", "Copy of " + selectedName, index);
            elements.SetData("Id", selectedId, index);
            elements.SetData("Description", selectedDescription, index);
        }
    }

    onSelectedIndexChanged: {
        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";
        commandsProvider.changeCommandMode("Duplicate", mode);
    }

    onClosed: {
        Events.sendEvent("LicenseFeaturesUpdate");
    }

    onEdited: {
        let elementsModel = tableData.elements;
        let oldLicenseId = itemId;
        let newLicenseId = elementsModel.GetData("Id", selectedIndex);

        if (oldLicenseId != newLicenseId){
            lisensesFeaturesModel.updateLicensesDependenciesAfterLicenseEditing(oldLicenseId, newLicenseId);
        }
    }
}


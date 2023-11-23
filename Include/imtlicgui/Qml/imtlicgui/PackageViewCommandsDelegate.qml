import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    onCommandActivated: {
        console.log("PackageViewCommands onCommandActivated", commandId);
        if (commandId === "New"){
        }
        else if (commandId === "Remove"){
        }
    }
}


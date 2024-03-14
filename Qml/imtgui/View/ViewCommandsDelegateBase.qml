import QtQuick 2.12
import Acf 1.0

Item {
    id: container;

    property Item view: null;

    signal commandActivated(string commandId);

    function commandHandle(commandId){
        if (commandId === "Save"){
        }
        else if (commandId === "Close"){
        }
        else{
            commandActivated(commandId);
        }
    }

    Shortcut {
        sequence: "Ctrl+S";

        enabled: container.view ? container.view.visible : false;
        onActivated: {
            console.log("Ctrl+S onActivated");
            container.commandActivated("Save");
        }
    }
}

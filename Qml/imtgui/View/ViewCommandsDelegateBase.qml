import QtQuick 2.12
import Acf 1.0

Item {
    id: container;

    property Item view: null;

    signal commandActivated(string commandId);

    function commandHandle(commandId){
        commandActivated(commandId);
    }

    Shortcut {
        sequence: "Ctrl+S";

        enabled: container.view ? container.view.visible : false;

        onEnabledChanged: {
            console.log("View Ctrl+S onEnabledChanged", enabled);
        }

        onActivated: {
            console.log("View Ctrl+S onActivated");
            container.commandHandle("Save");
        }
    }
}

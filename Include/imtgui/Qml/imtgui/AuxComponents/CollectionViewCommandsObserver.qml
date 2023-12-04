import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: root;

    property var commandsProvider: null;

    Component.onDestruction: {
        if (root.orderCommandsProvider != null){
            root.commandsProvider.modelLoaded.disconnect(root.commandsModelLoaded);
        }
    }

    onCommandsProviderChanged: {
        if (root.commandsProvider != null){
//            root.commandsProvider.modelLoaded.connect(root.commandsModelLoaded);
        }
    }

    function commandsModelLoaded(){
        let removeIsExists = root.commandsProvider.commandExists("Remove");
        if (!removeIsExists){
            for (let i = 0; i < root.commandsProvider.commandsModel.GetItemsCount(); i++){
                let currentCommandId = root.commandsProvider.commandsModel.GetData("Id", i);
                if (currentCommandId === "Edit"){
                    let open = qsTr("Open");
                    root.commandsProvider.commandsModel.SetData("Name", open, i);
                }
            }
        }
    }
}


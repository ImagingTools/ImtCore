import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0

Item {
    id: root;

    property var commandsProvider: null;

    Component.onDestruction: {
        root.commandsProvider.modelLoaded.disconnect(root.commandsModelLoaded);
    }

    onCommandsProviderChanged: {
        if (root.commandsProvider != null){
//            root.commandsProvider.modelLoaded.connect(root.commandsModelLoaded);
        }
    }

    function commandsModelLoaded(){
        let removeIsExists = root.commandsProvider.commandExists("Remove");
        if (!removeIsExists){
            for (let i = 0; i < root.commandsProvider.commandsModel.getItemsCount(); i++){
                let currentCommandId = root.commandsProvider.commandsModel.getData("id", i);
                if (currentCommandId === "Edit"){
                    let open = qsTr("Open");
                    root.commandsProvider.commandsModel.setData("name", open, i);
                }
            }
        }
    }
}


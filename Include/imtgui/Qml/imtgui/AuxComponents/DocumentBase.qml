import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0

Item {
    id: documentBase;

    anchors.fill: parent;

    property string itemId;
    property string itemName;

    property string commandsId;
    property string commandsDelegatePath: "DocumentWorkspaceCommandsDelegateBase.qml";

    signal commandsDelegateLoaded();

    property alias documentModel: objectModel;

    property alias commandsDelegate: commandsDelegateBase.item;
    property alias commandsProvider: commandsProviderBase;

    Component.onCompleted: {
        console.log("documentBase onCompleted");
        itemId = documentsData.GetData("ItemId", model.index);
        itemName = documentsData.GetData("Title", model.index);
    }

    onCommandsIdChanged: {
        commandsProvider.commandsId = documentBase.commandsId;
        commandsDelegateBase.item.commandsId = documentBase.commandsId;
    }

    onVisibleChanged: {
        if (visible){
            Events.sendEvent("CommandsModelChanged", {"Model": commandsProvider.commandsModel,
                                                      "CommandsId": commandsProvider.commandsId});

            updateGui();
        }
    }

    onItemIdChanged: {
        objectModel.SetData("Id", itemId);
        documentsData.SetData("ItemId", itemId, model.index);
    }

    onItemNameChanged: {
        objectModel.SetData("Name", itemName);
        documentsData.SetData("Name", itemName, model.index);
    }

    TreeItemModel {
        id: objectModel;

        onModelChanged: {
            console.log("objectModel onModelChanged");
            console.log("DependentModel", objectModel.GetData("DependentModel"));
        }
    }

    Loader {
        id: commandsDelegateBase;

        source: commandsDelegatePath;

        onLoaded: {
            commandsDelegateBase.item.objectView = documentBase;
            commandsDelegateBase.item.objectModel = objectModel;

            commandsDelegateLoaded();
        }
    }

    CommandsProvider {
        id: commandsProviderBase;
    }

    function updateGui(){}
}

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

    property TreeItemModel documentModel;

    property bool itemLoad: true;

    property alias commandsDelegate: commandsDelegateBase.item;
    property alias commandsProvider: commandsProviderBase;

    Component.onCompleted: {
        console.log("documentBase onCompleted");
        console.log("documentsData", documentsData);

        if (itemLoad){
            itemId = documentsData.GetData("Id", model.index);
            itemName = documentsData.GetData("Title", model.index);
        }
    }

    onCommandsIdChanged: {
        console.log("documentBase onCommandsIdChanged", commandsId);

        if (itemLoad){
            commandsProvider.commandsId = documentBase.commandsId;
        }

        commandsDelegateBase.item.commandsId = documentBase.commandsId;
    }

    onDocumentModelChanged: {
        console.log("documentBase onDocumentModelChanged");

        updateGui();
    }

    onVisibleChanged: {
        if (visible){
            Events.sendEvent("CommandsModelChanged", {"Model": commandsProvider.commandsModel,
                                                      "CommandsId": commandsProvider.commandsId});

            documentBase.updateGui();
        }
    }

    onItemIdChanged: {
        if (itemLoad){
            documentsData.SetData("Id", itemId, model.index);
        }
    }

    onItemNameChanged: {
        if (itemLoad){
            documentsData.SetData("Name", itemName, model.index);
        }
    }

    Loader {
        id: commandsDelegateBase;

        source: commandsDelegatePath;

        onLoaded: {
            commandsDelegateLoaded();
        }
    }

    CommandsProvider {
        id: commandsProviderBase;
    }

    function updateGui(){
    }

    LoadingPage {
        id: loadingPage;

        z: 10;

        anchors.fill: parent;

        visible: false;
    }
}

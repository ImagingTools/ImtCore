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

    property TreeItemModel documentModel;

    property bool itemLoad: true;

    property alias commandsProvider: commandsProviderBase.item;
    property alias commandsProviderSourceCompr: commandsProviderBase.sourceComponent;

//    property Item commandsDelegate : DocumentWorkspaceCommandsDelegateBase {}

//    property string commandsDelegatePath: "DocumentWorkspaceCommandsDelegateBase.qml";
    property alias commandsDelegate: commandsDelegateBase.item;
    property alias commandsDelegateSourceComp: commandsDelegateBase.sourceComponent;

    signal commandsDelegateLoaded();

    Component.onCompleted: {
        commandsDelegate.documentBase = documentBase;

        if (itemLoad){
            itemId = documentsData.GetData("Id", model.index);
            itemName = documentsData.GetData("Title", model.index);
        }
    }

    onCommandsIdChanged: {
        console.log("documentBase onCommandsIdChanged", commandsId);

        if (itemLoad){
//            commandsProvider.commandsId = documentBase.commandsId;
            commandsProviderBase.item.commandsId = documentBase.commandsId;
        }

//        commandsDelegate.commandsId = documentBase.commandsId;
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

//    CommandsProvider {
//        id: commandsProviderBase;
//    }

    function updateGui(){}

    Loader {
        id: commandsProviderBase;

        sourceComponent: CommandsProvider {
        }

        onLoaded: {
        }
    }

    Loader {
        id: commandsDelegateBase;

        sourceComponent: DocumentWorkspaceCommandsDelegateBase {
        }
//        source: commandsDelegatePath;

        onLoaded: {
            item.documentBase = documentBase;
//            item.commandsId = commandsId;

            commandsDelegateLoaded();
        }
    }
}

import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0

FocusScope {
    id: documentBase;

    anchors.fill: parent;

    focus: true;

    property string itemId;
    property string itemName;
    property string commandsId;

    property TreeItemModel documentModel: TreeItemModel{};

    property bool itemLoad: true;

    property alias commandsDelegate: commandsDelegateBase.item;
    property alias commandsDelegateSourceComp: commandsDelegateBase.sourceComponent;

    property CommandsProvider commandsProvider: CommandsProvider {
        commandsId: documentBase.commandsId;
    }

    signal commandsDelegateLoaded();

    Shortcut {
        sequence: "Ctrl+S";

        enabled: visible;

        onActivated: {
            console.log("Ctrl+S onActivated");
            Events.sendEvent(commandsId + "CommandActivated", "Save");
        }
    }

    Shortcut {
        sequence: "Ctrl+Z";

        enabled: visible;

        onActivated: {
            console.log("Ctrl+Z onActivated");
            Events.sendEvent(commandsId + "CommandActivated", "Undo");
        }

        onActivatedAmbiguously: {
            console.log("Ctrl+Z onActivatedAmbiguously");
        }
    }

    Shortcut {
        sequence: "Ctrl+Shift+Z";

        enabled: visible;

        onActivated: {
            console.log("Ctrl+Shift+Z onActivated");
            Events.sendEvent(commandsId + "CommandActivated", "Redo");
        }
    }

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
        }

        commandsDelegate.commandsId = documentBase.commandsId;
    }

    onDocumentModelChanged: {
        console.log("documentBase onDocumentModelChanged");

//        updateGui();
    }

    onVisibleChanged: {
        if (visible){
            Events.sendEvent("CommandsModelChanged", {"Model": commandsProvider.commandsModel,
                                                      "CommandsId": commandsProvider.commandsId});

            documentBase.forceActiveFocus();
//            documentBase.updateGui();
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

    function updateGui(){}

    Loader {
        id: commandsDelegateBase;

        sourceComponent: DocumentWorkspaceCommandsDelegateBase {
        }

        onLoaded: {
            item.documentBase = documentBase;
            commandsDelegateLoaded();
        }
    }
}

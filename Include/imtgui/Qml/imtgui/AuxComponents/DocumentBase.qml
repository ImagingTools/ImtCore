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


    Component.onCompleted: {
        commandsDelegate.documentBase = documentBase;

        if (documentBase.itemLoad){
            documentBase.itemId = documentsData.GetData("Id", model.index);
            documentBase.itemName = documentsData.GetData("Title", model.index);
        }
    }

    onCommandsIdChanged: {
        console.log("documentBase onCommandsIdChanged", documentBase.commandsId);

        commandsDelegate.commandsId = documentBase.commandsId;
    }

    onVisibleChanged: {
        if (documentBase.visible){
            Events.sendEvent("CommandsModelChanged", {"Model": commandsProvider.commandsModel,
                                                      "CommandsId": commandsProvider.commandsId});

            documentBase.forceActiveFocus();
        }
    }

    onItemIdChanged: {
        if (documentBase.itemLoad){
            documentsData.SetData("Id", documentBase.itemId, model.index);
        }
    }

    onItemNameChanged: {
        if (itemLoad){
            documentsData.SetData("Name", documentBase.itemName, model.index);
        }
    }

    function updateGui(){}

    Shortcut {
        sequence: "Ctrl+S";

        enabled: documentBase.visible;

        onActivated: {
            console.log("Ctrl+S onActivated");
            Events.sendEvent(documentBase.commandsId + "CommandActivated", "Save");
        }
    }

    Shortcut {
        sequence: "Ctrl+Z";

        enabled: documentBase.visible;

        onActivated: {
            console.log("Ctrl+Z onActivated");
            Events.sendEvent(documentBase.commandsId + "CommandActivated", "Undo");
        }

        onActivatedAmbiguously: {
            console.log("Ctrl+Z onActivatedAmbiguously");
        }
    }

    Shortcut {
        sequence: "Ctrl+Shift+Z";

        enabled: documentBase.visible;

        onActivated: {
            console.log("Ctrl+Shift+Z onActivated");
            Events.sendEvent(documentBase.commandsId + "CommandActivated", "Redo");
        }
    }


    Loader {
        id: commandsDelegateBase;

        sourceComponent: DocumentWorkspaceCommandsDelegateBase {
        }

        onLoaded: {
            commandsDelegateBase.item.documentBase = documentBase;
            documentBase.commandsDelegateLoaded();
        }
    }
}

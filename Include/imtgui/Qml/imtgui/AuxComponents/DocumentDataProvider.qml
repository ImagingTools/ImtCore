import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0

Item {
    id: documentDataProvider;

    anchors.fill: parent;

    property string itemId;
    property string itemName;
    property string commandsId;

    property TreeItemModel documentModel:TreeItemModel{};

    property bool itemLoad: true;

    property alias commandsProvider: commandsProviderBase.item;
    property alias commandsProviderSourceCompr: commandsProviderBase.sourceComponent;

    property alias commandsDelegate: commandsDelegateBase.item;
    property alias commandsDelegateSourceComp: commandsDelegateBase.sourceComponent;

    signal commandsDelegateLoaded();

    Component.onCompleted: {
        commandsDelegate.documentBase = documentDataProvider;
    }

    onCommandsIdChanged: {
        console.log("documentBase onCommandsIdChanged", commandsId);

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

            documentDataProvider.updateGui();
        }
    }

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

        onLoaded: {
            item.documentBase = documentDataProvider;
            commandsDelegateLoaded();
        }
    }
}

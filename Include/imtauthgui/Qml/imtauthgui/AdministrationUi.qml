import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

SingleDocumentWorkspaceView {
    id: container;

    property string commandId;

    signal commandsModelChanged(var commandsModel);

    Component.onCompleted: {
        startPageObj = {"Id": "Administration",
            "Name": "Administration",
            "Source": "../../imtauthgui/AdministrationView.qml",
            "CommandsId": "Administration"};

        Events.subscribeEvent("CommandsModelChanged", container.onCommandsModelUpdated);
        Events.subscribeEvent("CommandsEnabledChanged", container.onCommandsEnabledChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("CommandsModelChanged", container.onCommandsModelUpdated);
        Events.unSubscribeEvent("CommandsEnabledChanged", container.onCommandsEnabledChanged);
    }

    UserTokenProvider {
        id: userTokenProvider;
    }

    ModalDialogManager {
        id: modalDialogManager;

        z: 30;

        anchors.fill: parent;
    }

    function login(login, password){
        console.log("login", login, password);
        userTokenProvider.authorization(login, password);
    }

    function logout(){}

    function onCommandsModelUpdated(parameters){
        console.log("onCommandsModelUpdated");
        let model = parameters["Model"];

        let commandsId = parameters["CommandsId"];
        container.commandId = commandsId;

        container.commandsModelChanged(model);
    }

    function onCommandsEnabledChanged(parameters){
        console.log("onCommandsEnabledChanged");
        container.onCommandsModelUpdated(parameters);
    }

    function onCommandActivated(value){
        var commandId = String(value);
        Events.sendEvent(container.commandId + "CommandActivated", commandId);
    }
}



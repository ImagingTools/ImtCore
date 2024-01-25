import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

SingleDocumentWorkspaceView {
    id: container;

    anchors.fill: parent;

    property string commandId;

    property Decorators decorators: decorators_
    property alias modalDialogManager: modalDialogManager_

    signal commandsModelChanged(var commandsModel);

    Component.onCompleted: {
        Style.setDecorators(decorators)

        startPageObj = {
            "Id": "Administration",
            "Name": "Administration",
            "Source": "../imtauthgui/AdministrationView.qml",
            "CommandId": "Administration"};

        Events.subscribeEvent("CommandsModelChanged", container.onCommandsModelUpdated);
        Events.subscribeEvent("CommandsEnabledChanged", container.onCommandsEnabledChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("CommandsModelChanged", container.onCommandsModelUpdated);
        Events.unSubscribeEvent("CommandsEnabledChanged", container.onCommandsEnabledChanged);
    }

    onVisibleChanged: {
        if (visible){
            container.updateModels();
        }
    }

    Decorators {
        id: decorators_
    }

    DecoratorsQt {
        id: decoratorsQt;
    }

    UserTokenProvider {
        id: userTokenProvider;
    }

    ModalDialogManager {
        id: modalDialogManager_;
        z: 99;
        anchors.fill: parent;
    }

    SubscriptionManager {
        id: subscriptionManager;
    }

    Loading {
        id: loading;

        z: 10000;

        anchors.fill: parent;

        visible: false;

        Component.onCompleted: {
            Events.subscribeEvent("StartLoading", loading.start);
            Events.subscribeEvent("StopLoading", loading.stop);
        }

        Component.onDestruction: {
            Events.unSubscribeEvent("StartLoading", loading.start);
            Events.unSubscribeEvent("StopLoading", loading.stop);
        }
    }

    function onCommandsModelUpdated(parameters){
        let model = parameters["Model"];

        let commandsId = parameters["CommandId"];
        container.commandId = commandsId;

        container.commandsModelChanged(model);
    }

    function onCommandsEnabledChanged(parameters){
        container.onCommandsModelUpdated(parameters);
    }

    function onCommandActivated(value){
        var commandId = String(value);
        Events.sendEvent(container.commandId + "CommandActivated", commandId);
    }

    function updateModels(){
        Events.sendEvent(container.commandId + "_CommandUpdateModel");
    }

    function firstModelsInit(){}
}



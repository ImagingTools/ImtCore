import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

Item {
    id: commandsItem;
    height: 30;

    property string commandId;

    signal commandActivated(string commandId);

    function getCommandData(commandId, key){
        let result = leftCommands.getCommandData(commandId, key);

        if (result == null){
            result = centerCommands.getCommandData(commandId, key);
        }

        if (result == null){
            result = rightCommands.getCommandData(commandId, key);
        }

        return result;
    }

    function setNegativeAccentCommandIds(commandIds){
        leftCommands.negativeAccentCommandIds = commandIds;
        centerCommands.negativeAccentCommandIds = commandIds;
        rightCommands.negativeAccentCommandIds = commandIds;
    }

    function setPositiveAccentCommandIds(commandIds){
        leftCommands.positiveAccentCommandIds = commandIds;
        centerCommands.positiveAccentCommandIds = commandIds;
        rightCommands.positiveAccentCommandIds = commandIds;
    }

    function setCommandData(commandId, key, value){
        leftCommands.setCommandData(commandId, key, value);
        centerCommands.setCommandData(commandId, key, value);
        rightCommands.setCommandData(commandId, key, value);
    }

    function setCommandsModel(params){
        clearModel();

        let commId = params["ViewId"];
        let model = params["Model"];

        for (let i = 0; i < model.getItemsCount(); i++){
            let alignment = model.getData("Alignment", i);
            let groupModel = model.getModelFromItem(i);
            if (alignment === 1){
                leftCommands.addCommandGroup(groupModel);
            }
            else if (alignment === 4){
                centerCommands.addCommandGroup(groupModel);
            }
            else if (alignment === 2){
                rightCommands.addCommandGroup(groupModel);
            }
        }

        widthTimer.restart();
        buttonTimer.restart();
    }

    function clearModel(){
        leftCommands.clear();
        centerCommands.clear();
        rightCommands.clear();
    }

    Component.onCompleted: {
        widthTimer.restart();
        buttonTimer.restart();
    }

    onVisibleChanged: {
        if (visible){
            checkCommandsWidth();
            buttonTimer.restart();
        }
    }

    onWidthChanged: {
        checkCommandsWidth();
        buttonTimer.restart();
    }

    Timer {
        id: widthTimer;
        interval: 500;
        onTriggered: {
            commandsItem.checkCommandsWidth();
        }
    }

    Timer {
        id: buttonTimer;
        interval: 500;
        onTriggered: {
            commandsItem.checkButtonVisible();
        }
    }

    function checkCommandsWidth(){
        leftCommands.checkWidth();
        centerCommands.checkWidth();
        rightCommands.checkWidth();
    }

    function checkButtonVisible(){
        button.visible = leftCommands.hasHiddenCommands() || centerCommands.hasHiddenCommands() || rightCommands.hasHiddenCommands();
    }

    CommandsView {
        id: leftCommands;
        anchors.left: parent.left;
        anchors.leftMargin: Style.size_mainMargin;
        maximumWidth: centerCommands.countGroups > 0 ? centerCommands.x : rightCommands.countGroups > 0 ? rightCommands.x : commandsItem.width;
        eventCommandPrefix: commandsItem.commandId;
    }

    CommandsView {
        id: centerCommands;
        anchors.horizontalCenter: parent.horizontalCenter;
        maximumWidth: rightCommands.countGroups > 0 ? rightCommands.x - centerCommands.x : commandsItem.width - centerCommands.x;
        eventCommandPrefix: commandsItem.commandId;
    }

    CommandsView {
        id: rightCommands;
        anchors.right: button.left;
        anchors.rightMargin: Style.size_mainMargin;
        eventCommandPrefix: commandsItem.commandId;
        maximumWidth: commandsItem.width - (centerCommands.x + centerCommands.contentWidth);
    }

    ToolButton {
        id: button;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        width: visible ? 30 : 0;
        height: width;
        iconSource: "../../../" + Style.getIconPath("Icons/More", Icon.Mode.On, Icon.State.Active);
        visible: false;
        decorator: Component {
            ToolButtonDecorator {
                color: baseElement && baseElement.hovered  ? Style.baseColor : "transparent";
            }
        }

        onClicked: {
            var point = button.mapToItem(null, 0, height);
            let empty = {
                "Id": "",
                "Name": "",
                "Icon": "",
                "IsEnabled": true
            };

            contextMenuModel.clear();
            if (leftCommands.hasHiddenCommands()){
                contextMenuModel.addModel(leftCommands.allElements);
            }

            if (centerCommands.hasHiddenCommands()){
                if (contextMenuModel.count > 0){
                    contextMenuModel.insertNewItem(empty);
                }

                contextMenuModel.addModel(centerCommands.allElements);
            }

            if (rightCommands.hasHiddenCommands()){
                if (contextMenuModel.count > 0){
                    contextMenuModel.insertNewItem(empty);
                }
                contextMenuModel.addModel(rightCommands.allElements);
            }

            ModalDialogManager.openDialog(popupMenu, {"x": point.x - 200, "y": point.y, "model": contextMenuModel});
        }
    }

    ListModel {
        id: contextMenuModel;

        function addModel(elements){
            let result = false;
            for (let i = 0; i < elements.length; i++){
                if (!elements[i].visible){
                    insertNewItem({
                                      "Id": elements[i].modelData.Id,
                                      "Name": elements[i].modelData.Name,
                                      "Icon": elements[i].modelData.Icon,
                                      "IsEnabled": elements[i].modelData.IsEnabled
                                  }
                                  );
                    result = true;
                }
            }

            return result;
        }

        function insertNewItem(params){
            contextMenuModel.append(params);
        }
    }

    Component {
        id: popupMenu;

        PopupMenuDialog {
            id: popupMenuDialog;
            shownItemsCount: 20;
            itemHeight: 30;
            onFinished: {
                if (commandId !== ""){
                    commandsItem.commandActivated(commandId)
                }
            }
        }
    }
}

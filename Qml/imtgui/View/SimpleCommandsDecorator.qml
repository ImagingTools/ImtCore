import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: container;

    width: row.width;

    property TreeItemModel commandModel: null;

    signal commandActivated(string commandId);
    signal commandAdded(int index, string commandId);
    property int buttonSize: 18

    onCommandModelChanged: {
        if (container.commandModel != null){
            repeater.model = container.commandModel;
        }
    }

    function setCommandVisible(commandId, visible){
        for (let i = 0; i < container.commandModel.getItemsCount(); i++){
            let id = container.commandModel.getData("id", i);
            if (id === commandId){
                container.commandModel.setData("visible", visible, i);
                break;
            }
        }
    }

    function setCommandIsEnabled(commandId, isEnabled){
        if(!container.commandModel){
            return;
        }

        for (let i = 0; i < container.commandModel.getItemsCount(); i++){
            let currentCommandId = container.commandModel.getData("id", i);
            if (currentCommandId == commandId){
                let currentIsEnabled = container.commandModel.getData("isEnabled", i);
                if (currentIsEnabled !== isEnabled){
                    container.commandModel.setData("isEnabled", isEnabled, i);
                }
            }
        }
    }

    function getCommandIndex(commandId){
        if(!container.commandModel){
            return;
        }

        for (let i = 0; i < container.commandModel.getItemsCount(); i++){
            let currentCommandId = container.commandModel.getData("id", i);
            if (currentCommandId == commandId){
                return i;
            }
        }
    }

    Row {
        id: row;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;

        spacing: Style.sizeMainMargin;

        Repeater {
            id: repeater;

            delegate: ToolButton {
                width: visible ? container.buttonSize : 0;
                height: width;

                iconSource: enabled ? "../../../../" + Style.getIconPath(model.icon, Icon.State.Off, Icon.Mode.Normal) :
                                              "../../../../" + Style.getIconPath(model.icon, Icon.State.Off, Icon.Mode.Disabled);

                enabled: model.isEnabled != undefined && model.isEnabled != null ? model.isEnabled : true;

                visible: model.visible != undefined && model.visible != null ? model.visible : true;

                tooltipText: model.tooltipText ? model.tooltipText : "";

                property string commandId: model.id;

                onClicked: {
                    container.commandActivated(model.id);
                }
            }

            onItemAdded: {
                container.commandAdded(index, item.commandId)
            }
        }
    }
}

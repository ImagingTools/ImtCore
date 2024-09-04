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

    onCommandModelChanged: {
        if (container.commandModel != null){
            repeater.model = container.commandModel;
        }
    }

    function setCommandVisible(commandId, visible){
        for (let i = 0; i < container.commandModel.getItemsCount(); i++){
            let id = container.commandModel.getData("Id", i);
            if (id === commandId){
                container.commandModel.setData("Visible", visible, i);
                break;
            }
        }
    }

    function setCommandIsEnabled(commandId, isEnabled){
        if(!container.commandModel){
            return;
        }

        for (let i = 0; i < container.commandModel.getItemsCount(); i++){
            let currentCommandId = container.commandModel.getData("Id", i);
            if (currentCommandId == commandId){
                let currentIsEnabled = container.commandModel.getData("IsEnabled", i);
                if (currentIsEnabled !== isEnabled){
                    container.commandModel.setData("IsEnabled", isEnabled, i);
                }
            }
        }
    }

    function getCommandIndex(commandId){
        if(!container.commandModel){
            return;
        }

        for (let i = 0; i < container.commandModel.getItemsCount(); i++){
            let currentCommandId = container.commandModel.getData("Id", i);
            if (currentCommandId == commandId){
                return i;
            }
        }
    }

    Row {
        id: row;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;

        spacing: Style.size_mainMargin;

        Repeater {
            id: repeater;

            delegate: ToolButton {
                width: visible ? 18 : 0;
                height: width;

                iconSource: enabled ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Normal) :
                                              "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled);

                enabled: model.IsEnabled != undefined && model.IsEnabled != null ? model.IsEnabled : false;

                visible: model.Visible != undefined && model.Visible != null ? model.Visible : false;

                tooltipText: model.TooltipText ? model.TooltipText : "";

                property string commandId: model.Id;

                onClicked: {
                    container.commandActivated(model.Id);
                }
            }

            onItemAdded: {
                container.commandAdded(index, item.commandId)
            }
        }
    }
}

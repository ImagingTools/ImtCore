import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

// Commands with priority = -1 always hidden
Item {
    id: commandsItem;
    width: contentWidth;
    height: 30;

    property string eventCommandPrefix;
    property alias contentWidth: content_.width;
    property int maximumWidth: -1;

    // Count of the command groups in this view
    property alias countGroups: repeater.count;
    property TreeItemModel commandsModel: TreeItemModel {}

    // Reference on the all elements in order of visibility to this view
    property var allElements: []

    // Reference on the all elements in order of priority to this view
    property var priorityElements: []

    // Commands with negative accent
    property var negativeAccentCommandIds: []

    // Commands with positive accent
    property var positiveAccentCommandIds: []

    property string positiveAccentColor: Style.imaginToolsAccent;
    property string negativeAccentColor: Style.errorTextColor;

    onVisibleChanged: {
        if (visible){
            timer.restart();
        }
    }

    Timer {
        id: timer;
        interval: 10;
        onTriggered: {
            if (commandsItem){
                commandsItem.checkWidth();
            }
        }
    }

    function setCommandData(commandId, key, value){
        for (let j = 0; j < allElements.length; j++){
            if (allElements[j].modelData["Id"] === commandId){
                allElements[j].modelData[key] = value;

                break;
            }
        }
    }

    function getCommandData(commandId, key){
        for (let j = 0; j < allElements.length; j++){
            if (allElements[j].modelData["Id"] === commandId){
                return allElements[j].modelData[key];
            }
        }

        return null;
    }

    function addCommandGroup(commands){
        let index = commandsModel.insertNewItem();
        commandsModel.copyItemDataFromModel(index, commands);
        repeater.model = commandsItem.commandsModel;
    }


    // Clear view
    function clear(){
        commandsModel.clear();
        allElements = []
        priorityElements = []
    }

    function hasHiddenCommands(){
        for (let j = 0; j < allElements.length; j++){
            if (!allElements[j].visible){
                return true;
            }
        }

        return false;
    }

    function isEmpty(){
        return commandsModel.getItemsCount() === 0;
    }

    // The function checks whether the commands fit into the maximum width of this view
    // If they do not fit, then we hide the command with the lowest priority
    function checkWidth(){
        if (maximumWidth < 0){
            return;
        }

        let totalContentWidth = contentWidth + Style.size_mainMargin;
        if (totalContentWidth > maximumWidth){
            let tempWidth = totalContentWidth;
            for (let j = priorityElements.length - 1; j >= 0; j--){
                if (priorityElements[j].visible){
                    let elementWidth = priorityElements[j].maxWidth;
                    tempWidth -= elementWidth;

                    priorityElements[j].modelData.Visible = false;

                    if (tempWidth < maximumWidth){
                        break;
                    }
                }
            }
        }
        else{
            let tempWidth = totalContentWidth;
            for (let j = 0; j < priorityElements.length; j++){
                if (!priorityElements[j].visible){
                    let elementWidth = priorityElements[j].maxWidth;
                    tempWidth += elementWidth;

                    if (tempWidth < maximumWidth){
                        priorityElements[j].modelData.Visible = true;
                    }

                    if (tempWidth >= maximumWidth){
                        break;
                    }
                }
            }
        }
    }

    Row {
        id: content_;
        height: commandsItem.height;
        spacing: Style.size_mainMargin;

        Repeater {
            id: repeater;
            delegate: Component {
                Item {
                    id: itemDelegate;
                    width: listView.width;
                    height: commandsItem.height;
                    property var dataModel: model;
                    property int priority: model.Priority ? model.Priority: -1;

                    Row {
                        id: listView;
                        height: commandsItem.height;
                        spacing: Style.size_mainMargin;

                        Repeater {
                            id: repeater2;
                            model: itemDelegate.dataModel.SubElements;
                            delegate: Component { Button {
                                    id: button;
                                    enabled: model.IsEnabled;
                                    visible: priority < 0 ? false : model.Visible;
                                    text: model.Name;
                                    widthFromDecorator: true;
                                    heightFromDecorator: true;
                                    iconSource: model.Icon === "" ? "" : model.IsEnabled ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal) :
                                                                                           "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled);
                                    decorator: Component {
                                        TopButtonDecorator {
                                            property string baseColor: baseElement && baseElement.mouseArea.containsMouse ? Style.baseColor : Style.backgroundColor2;
                                            property string baseTextColor: !baseElement ? "transparent" : baseElement.enabled ? Style.textColor : Style.inactive_textColor;
                                            color: button.isPositiveAccent ? commandsItem.positiveAccentColor :
                                                                             button.isNegativeAccent ? commandsItem.negativeAccentColor : baseColor;
                                            textColor: button.isPositiveAccent || button.isNegativeAccent ? "white" : baseTextColor;
                                            icon.source: button.isPositiveAccent || button.isNegativeAccent ?
                                                             "../../../../" + Style.getIconPath(button.modelData.Icon, Icon.State.Off, Icon.Mode.Disabled)
                                                           : baseElement.iconSource;
                                            border.width: button.isPositiveAccent || button.isNegativeAccent ? 0 : baseElement.mouseArea.containsMouse;
                                            opacity: button.hovered && (button.isPositiveAccent || button.isNegativeAccent) ? 0.85 : 1;
                                        }
                                    }

                                    property bool isNegativeAccent: model.IsEnabled ? commandsItem.negativeAccentCommandIds.includes(model.Id) : false;
                                    property bool isPositiveAccent: model.IsEnabled ? commandsItem.positiveAccentCommandIds.includes(model.Id) : false;
                                    property int priority: model.Priority;
                                    property int groupPriority: itemDelegate.priority;
                                    property var modelData: model;
                                    property var subElements: repeater2.model;
                                    property int maxWidth: -1;

                                    onClicked: {
                                        Events.sendEvent(commandsItem.eventCommandPrefix + "CommandActivated", model.Id);
                                    }

                                    onWidthChanged: {
                                        maxWidth = Math.max(maxWidth, width);
                                    }
                                }
                            }

                            onItemAdded: {
                                if (!commandsItem.allElements.includes(item)){
                                    commandsItem.allElements.push(item);
                                }

                                if (!commandsItem.priorityElements.includes(item)){
                                    commandsItem.priorityElements.push(item);
                                }

                                commandsItem.priorityElements.sort(function(item1, item2){
                                    if (item1.groupPriority > item2.groupPriority){
                                        return -1;
                                    }
                                    if (item1.groupPriority < item2.groupPriority){
                                        return 1;
                                    }
                                    if (item1.priority > item2.priority){
                                        return -1;
                                    }
                                    if (item1.priority < item2.priority){
                                        return 1;
                                    }

                                    return 0;
                                });
                            }
                        }
                    }
                }
            }
        }
    } // Main row
}


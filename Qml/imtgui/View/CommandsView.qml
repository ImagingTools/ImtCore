import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: commandsItem;
    width: contentWidth;
    height: 30;

    property alias contentWidth: content_.width;
    property int maximumWidth: -1;
    property string commandId;

    property alias countGroups: repeater.count;
    property TreeItemModel commandsModel: TreeItemModel {}

    onVisibleChanged: {
        if (visible){
            timer.restart();
        }
    }

    Timer {
        id: timer;
        interval: 10;
        onTriggered: {
            commandsItem.checkWidth();
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

    function clear(){
        commandsModel.clear();
        allElements = []
        priorityElements = []
    }

    function hasHiddenCommands(){
        for (let j = 0; j < allElements.length; j++){
            if (!allElements[j].modelData.Visible){
                return true;
            }
        }

        return false;
    }

    function isEmpty(){
        return commandsModel.getItemsCount() === 0;
    }

    function checkWidth(){
        console.log("CommandsView checkWidth");

        if (maximumWidth < 0){
            return;
        }

        console.log("maximumWidth", maximumWidth);
        let totalContentWidth = contentWidth + Style.size_mainMargin;
        console.log("totalContentWidth", totalContentWidth);

        if (totalContentWidth > maximumWidth){
            let tempWidth = totalContentWidth;
            for (let j = priorityElements.length - 1; j >= 0; j--){
                if (priorityElements[j].modelData.Visible){
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
                if (!priorityElements[j].modelData.Visible){
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

    property var allElements: []
    property var priorityElements: []

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

                        Repeater {
                            id: repeater2;
                            model: itemDelegate.dataModel.SubElements;
                            delegate: Component { Button {
                                    enabled: model.IsEnabled;
                                    visible: model.Visible;
                                    text: model.Name;
                                    widthFromDecorator: true;
                                    heightFromDecorator: true;
                                    iconSource: model.Icon === "" ? "" : model.IsEnabled ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal) :
                                                                                           "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled);
                                    decorator: Component {
                                        TopButtonDecorator {}
                                    }

                                    onClicked: {
                                        Events.sendEvent(commandsItem.commandId + "CommandActivated", model.Id);
                                    }

                                    onWidthChanged: {
                                        maxWidth = Math.max(maxWidth, width);
                                    }

                                    property int priority: model.Priority;
                                    property int groupPriority: itemDelegate.priority;
                                    property var modelData: model;
                                    property var subElements: repeater2.model;

                                    property int maxWidth: -1;
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
    }
}


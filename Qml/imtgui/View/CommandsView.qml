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

    property string positiveAccentColor: Style.imaginToolsAccentColor;
    property string negativeAccentColor: Style.errorTextColor;

    property int commandsCount: 0;

	signal buttonClicked(string buttonId, var button);

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
            if (allElements[j]){
                if (allElements[j].modelData["Id"] === commandId){
                    return allElements[j].modelData[key];
                }
            }
        }

        return null;
    }

    function addCommandGroup(commands){
        if (commands.containsKey("SubElements")){
            let subElements = commands.getData("SubElements")
            if (subElements && subElements.getItemsCount() > 0){
                let index = commandsModel.insertNewItem();
                commandsModel.copyItemDataFromModel(index, commands);
                repeater.model = commandsItem.commandsModel;
            }
        }
    }

    // Clear view
    function clear(){
        commandsModel.clear();
        allElements = []
        priorityElements = []
    }

    function reset(){
        for (let j = 0; j < allElements.length; j++){
            if (allElements[j]){
                allElements[j].modelData.Visible = false;
            }
        }
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
        let maxWidth = maximumWidth;
        if (maxWidth < 0){
            maxWidth = 0;
        }

        reset();

        let visibleCount = 0;
        let tempWidth = 0;
        for (let j = 0; j < priorityElements.length; j++){
            let elementWidth = priorityElements[j].maxWidth;
            tempWidth += elementWidth;
            if (tempWidth + visibleCount * Style.sizeMainMargin < maxWidth){
                priorityElements[j].modelData.Visible = true;
                visibleCount++;
            }

            if (tempWidth >= maxWidth){
                continue;
            }
        }
    }

    Row {
        id: content_;
        height: commandsItem.height;
        spacing: Style.sizeMainMargin;

        Repeater {
            id: repeater;

            delegate: Component {
                Item {
                    id: itemDelegate;
                    width: listView.width;
                    height: commandsItem.height;
                    property var dataModel: model;
                    property int priority: model.Priority ? model.Priority: -1;
                    property int commandsCount: repeater2.count;

                    Row {
                        id: listView;
                        height: commandsItem.height;
                        spacing: Style.sizeMainMargin;

						Repeater {
                            id: repeater2;
                            model: itemDelegate.dataModel.SubElements;
                            delegate: Component { Button {
                                    id: button;
                                    enabled: model ? model.IsEnabled : false;
                                    text: model ? model.Name : "";
                                    widthFromDecorator: true;
                                    heightFromDecorator: true;
									checkable: model && model.IsToggled !== undefined && model.IsToggleable === true;
                                    checked: model && model.IsToggled !== undefined ? model.IsToggled : false;
									iconSource: !model ? "" : model.Icon === "" ? "" : model.IsEnabled ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal) :
                                                                                           "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled);
                                    decorator: Component {
                                        TopButtonDecorator {
											property string baseColor: !baseElement ? "transparent" : baseElement.checked ? Style.borderColor : baseElement.mouseArea.containsMouse ? Style.baseColor : Style.backgroundColor2
                                            property string baseTextColor: !baseElement ? "transparent" : baseElement.enabled ? Style.textColor : Style.inactiveTextColor;
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

									property bool isNegativeAccent: model && model.IsEnabled ? commandsItem.negativeAccentCommandIds.includes(model.Id) : false;
									property bool isPositiveAccent: model && model.IsEnabled ? commandsItem.positiveAccentCommandIds.includes(model.Id) : false;
									property int priority: model ? model.Priority : -1;
									property int groupPriority: itemDelegate.priority;
                                    property var modelData: model;
                                    property var subElements: repeater2.model;
                                    property int maxWidth: -1;

									visible: !model || priority < 0 ? false : model.Visible;

                                    onClicked: {
										// commandsItem.buttonClicked(model.Id, this);
										Events.sendEvent(commandsItem.eventCommandPrefix + "CommandActivated", model.Id);
                                    }

                                    onWidthChanged: {
                                        maxWidth = Math.max(maxWidth, width);
                                    }
                                }
                            }

                            onItemAdded: {
								if (!item || !item.modelData){
                                    return;
                                }

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

                                commandsItem.commandsCount++;
                            }
                        }
                    }
                }
            }
        }
    } // Main row
}


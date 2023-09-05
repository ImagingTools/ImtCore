import QtQuick 2.12
import Acf 1.0

Item {
    id: commandsDecoratorContainer;

    anchors.fill: parent;

    property string commandsId;

    Component.onCompleted: {
        Events.subscribeEvent("Logout", commandsDecoratorContainer.clearModel);
        Events.subscribeEvent("SetCommandsVisible", commandsDecoratorContainer.setVisible);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("Logout", commandsDecoratorContainer.clearModel);
        Events.unSubscribeEvent("SetCommandsVisible", commandsDecoratorContainer.setVisible);
    }

    function setCommandsModel(parameters){
        console.log("CommandsDecorator setCommandsModel", parameters, commandsDecoratorContainer);
        let model = parameters["Model"];
        let commId = parameters["CommandsId"];

        commandsDecoratorContainer.commandsId = commId;
        buttonPanel.buttonModel = model;
        buttonPanel.updateModel();
    }

    function clearModel(){
        buttonPanel.model = 0;
    }

    function setVisible(visible){
        buttonPanel.visible = visible;
    }

    Component{
        id: defaultButtonDecorator;
        TopButtonDecorator{}
    }

    ButtonPanel{
        id: buttonPanel;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.leftMargin: 100;
        width:  parent.width - 2*anchors.leftMargin;
        height: parent.height

        horizontalSpacing: 15;
        verticalSpacing: 10;

        //hasActiveState: true;
        openButtonImageSource: "../../../Icons/" + Style.theme + "/Next_On_Active.svg"

        centered: true;

        buttonDelegate: Component{
            BaseButton {

                id: topButtonDelegate;

                decorator: Style.topButtonDecorator !==undefined ? Style.topButtonDecorator: defaultButtonDecorator;
                imageSource: model.IsEnabled ? "../../../../Icons/" + Style.theme + "/" + model.Icon + "_Off_Normal.svg" :
                                               "../../../../Icons/" + Style.theme + "/" + model.Icon + "_Off_Disabled.svg";

                enabled: model.IsEnabled;

                visible: model.Visible;

                text: model.Name;

                isToggled: isToggleable ? model.IsToggled !== undefined ? model.IsToggled : false : false;
                isToggleable: model.IsToggleable !== undefined ? model.IsToggleable : false

                Component.onCompleted: {
                    console.log("Command onCompleted", model.Id);
                }

                onClicked: {
                    Events.sendEvent(commandsDecoratorContainer.commandsId + "CommandActivated", model.Id);
                }

                Rectangle {
                    id: notification;

                    anchors.top: parent.top;
                    anchors.right: parent.right;

                    width: notificationText.width + 10;
                    height: notificationText.height;

                    color: Style.errorTextColor;
                    radius: width;

                    visible: model.Status && model.Status !== "";

                    Text {
                        id: notificationText;

                        anchors.centerIn: notification;

                        text: model.Status;

                        font.family: Style.fontFamilyBold;
                        font.pixelSize: Style.fontSize_common;

                        color: Style.baseColor;
                    }
                }
            }

        }

        buttonDelegateVert: Component{
            Item{
                width: model.Name == "" ? splitter.width : textButtonDelegateContainer.width;
                height: model.Name == "" ? splitter.height : textButtonDelegateContainer.height;

                Item{
                    id: textButtonDelegateContainer;

                    width: imageButton.width + textButtonDelegate.width;
                    height: textButtonDelegate.height;
                    visible: model.Name !== "";

                    AuxButton{
                        id: imageButton;

                        anchors.verticalCenter: parent.verticalCenter;

                        width: textButtonDelegate.height + 5;
                        height: textButtonDelegate.height;
                        iconWidth: width - 8;

                        enabled: model.IsEnabled;
                        hasIcon: true;
                        hasText: false;
                        highlighted: false;
                        color: "transparent";
                        iconSource: model.IsEnabled ? "../../../../Icons/" + Style.theme + "/" + model.Icon + "_Off_Normal.svg" :
                                                       "../../../../Icons/" + Style.theme + "/" + model.Icon + "_Off_Disabled.svg";


                        onClicked: {
                            Events.sendEvent(commandsDecoratorContainer.commandsId + "CommandActivated", model.Id);
                        }
                    }

                    TextButton{
                        id: textButtonDelegate;

                        anchors.left: imageButton.right;
                        anchors.verticalCenter: parent.verticalCenter;

                        legendColor: model.IsEnabled ? Style.textColor : "gray";
                        indicatorColor: "transparent";

                        enabled: model.IsEnabled;
                        legend: model.Name !== undefined ? model.Name : "";
                        active: model.Active !== undefined ? model.Active : false;
                        fontFamily: Style.fontFamily;
                        hasIndicator: false;
                        fontBold: true;
                        fontPixelSize: Style.fontSize_common;
                        property Item rootItem: buttonPanel;
                        property string id: model.Id !== undefined ? model.Id : "";

                        onClicked: {
                            Events.sendEvent(commandsDecoratorContainer.commandsId + "CommandActivated", model.Id);
                            rootItem.clicked(id);
                        }
                    }
                }



                Rectangle{
                    id: splitter;

                    anchors.top: parent.top;

                    width: buttonPanel.verticalMenuWidth;
                    height: model.Name == "" && model.index == 0 ? -buttonPanel.verticalSpacing : 2;
                    color: Style.textColor;
                    visible: model.Name !== "" ? false : model.index == 0 ? false : model.index == (buttonPanel.verticalModel.GetItemsCount() - 1) ? false : true ;
                }

            }

        }
    }

}

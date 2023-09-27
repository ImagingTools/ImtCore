import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: commandsDecoratorContainer;

    anchors.fill: parent;

    property string commandsId;

    Component.onCompleted: {
        Events.subscribeEvent("Logout", commandsDecoratorContainer.clearModel);
        Events.subscribeEvent("SetCommandsVisible", commandsDecoratorContainer.setVisible);
        Events.subscribeEvent("CommandsClearModel", commandsDecoratorContainer.clearModel);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("Logout", commandsDecoratorContainer.clearModel);
        Events.unSubscribeEvent("SetCommandsVisible", commandsDecoratorContainer.setVisible);
        Events.unSubscribeEvent("CommandsClearModel", commandsDecoratorContainer.clearModel);
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
        buttonPanel.clearModel();
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

        openButtonImageSource: "../../../" + Style.getIconPath("Icons/Next", Icon.Mode.On, Icon.State.Active);

        centered: true;

        buttonDelegate: Component{
            Item{
                width: isHorizontal ? topButtonDelegate.width : -buttonPanel.horizontalSpacing;

                height: topButtonDelegate.height;

                property bool isHorizontal: model.IsHorizontal == undefined ? true : model.IsHorizontal;
                BaseButton {

                    id: topButtonDelegate;

                    decorator: Style.topButtonDecorator !==undefined ? Style.topButtonDecorator: defaultButtonDecorator;

                    imageSource: model.IsEnabled ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal) :
                                                   "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled);

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

        }

        buttonDelegateVert: Component{
            Item{

                id: verticalComp;

                width: model.Name == "" ? splitter.width : textButtonDelegateContainer.width;
                height: isHorizontal ? -buttonPanel.verticalSpacing : model.Name == "" ? splitter.height : textButtonDelegateContainer.height;

                property bool isHorizontal: model.IsHorizontal == undefined ? true : model.IsHorizontal;

                Item{
                    id: textButtonDelegateContainer;

                    anchors.top: parent.top;

                    width: imageButton.width + textButtonDelegate.width + 5;
                    height: Math.max(textButtonDelegate.height, imageButton.height);
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

                        iconSource: model.IsEnabled !== "" ? (model.Icon !== "" ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal) : "") :
                                                       (model.Icon !== "" ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled) : "");

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

                    width: verticalComp.isHorizontal ? 1 : buttonPanel.verticalMenuWidth;
                    height: model.Name == "" && model.index == buttonPanel.horizCount ? -buttonPanel.verticalSpacing : 2;
                    color: Style.textColor;
                    visible: model.Name !== "" ? false : model.index == buttonPanel.horizCount ? false : model.index == (buttonPanel.buttonModel.GetItemsCount() - 1) ? false : true ;
                }


            }

        }
    }

}

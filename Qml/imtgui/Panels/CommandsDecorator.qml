import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: commandsDecoratorContainer;

    anchors.fill: parent;

    property string commandId;

    Component.onCompleted: {
        Events.subscribeEvent("Logout", commandsDecoratorContainer.clearModel);
        Events.subscribeEvent("SetCommandsVisible", commandsDecoratorContainer.setVisible);
        Events.subscribeEvent("ClearCommandsGui", commandsDecoratorContainer.clearModel);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("Logout", commandsDecoratorContainer.clearModel);
        Events.unSubscribeEvent("SetCommandsVisible", commandsDecoratorContainer.setVisible);
        Events.unSubscribeEvent("ClearCommandsGui", commandsDecoratorContainer.clearModel);
    }

    function setCommandsModel(parameters){
        console.log("start setCommandsModel", JSON.stringify(parameters));
        let model = parameters["Model"];
        let commId = parameters["ViewId"];

        commandsDecoratorContainer.commandId = commId;

        buttonPanel.buttonModel = model;

        buttonPanel.updateModel();

        console.log("end setCommandsModel");
    }

    function clearModel(parameters){
        console.log("start clearModel");
        if (parameters){
            let commandId = parameters["ViewId"];
            if (commandId !== commandsDecoratorContainer.commandId){
                return;
            }
        }

        buttonPanel.clearModel();
        console.log("end clearModel");
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

        anchors.verticalCenter: parent ? parent.verticalCenter : undefined;
        anchors.left: parent ? parent.left : undefined;
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
                Button {
                    id: topButtonDelegate;

                    decorator: Style.topButtonDecorator

                    iconSource: model.Icon === "" ? "" : model.IsEnabled ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal) :
                                                   "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled);

                    enabled: model.IsEnabled;

                    visible: model.Visible;

                    text: model.Name;

                    checked: checkable ? model.IsToggled !== undefined ? model.IsToggled : false : false;
                    checkable: model.IsToggleable !== undefined ? model.IsToggleable : false

                    widthFromDecorator: true;
                    heightFromDecorator: true;

                    onClicked: {
                        console.log(commandsDecoratorContainer.commandId + "CommandActivated", model.Id);
                        Events.sendEvent(commandsDecoratorContainer.commandId + "CommandActivated", model.Id);
                    }

                    Rectangle {
                        id: notification;

                        z: parent.z + 1;

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

                Button {
                    id: textButtonDelegateContainer;
                    visible: model.Name !== "";
                    iconSource: model.IsEnabled !== "" ? (model.Icon !== "" ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal) : "") :
                                                         (model.Icon !== "" ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled) : "");
                    enabled: model.IsEnabled;
                    text: model.Name !== undefined ? model.Name : "";
                    checked: checkable ? model.IsToggled !== undefined ? model.IsToggled : false : false;
                    checkable: model.IsToggleable !== undefined ? model.IsToggleable : false

                    decorator: Component {
                        ButtonDecorator {
                            color: !baseElement ? "transtarent" : baseElement.down || baseElement.checked ? Style.buttonPressedColor : baseElement.hovered ?  Style.buttonHoverColor : "transparent"
                            border.color: "transparent"
                        }
                    }

                    property string id: model.Id !== undefined ? model.Id : "";

                    onClicked: {
                        Events.sendEvent(commandsDecoratorContainer.commandId + "CommandActivated", model.Id);
                        buttonPanel.clicked(id);
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

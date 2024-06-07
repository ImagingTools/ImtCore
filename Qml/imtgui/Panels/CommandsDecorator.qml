import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

Item {
    id: commandsDecoratorContainer;

    anchors.fill: parent;

    property string commandId;

    Component.onCompleted: {
        Events.subscribeEvent("Logout", commandsDecoratorContainer.clearModel);
        Events.subscribeEvent("SetCommandsVisible", commandsDecoratorContainer.setVisible);
        Events.subscribeEvent("ClearCommandsGui", commandsDecoratorContainer.clearModel);
        Events.subscribeEvent("GetActiveCommandsViewId", commandsDecoratorContainer.getCommandIdRequest);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("Logout", commandsDecoratorContainer.clearModel);
        Events.unSubscribeEvent("SetCommandsVisible", commandsDecoratorContainer.setVisible);
        Events.unSubscribeEvent("ClearCommandsGui", commandsDecoratorContainer.clearModel);
        Events.unSubscribeEvent("GetActiveCommandsViewId", commandsDecoratorContainer.getCommandIdRequest);
    }

    property int loadImages: 0
    property TreeItemModel buttonModel;

    onLoadImagesChanged: {
        if (loadImages == 0){
            buttonPanel.buttonModel = buttonModel;

            buttonPanel.updateModel();

            Events.sendEvent("CommandsGuiReady");
         }
    }

    function getCommandIdRequest(callback){
        if (callback){
            callback(commandId);
        }
    }

    function counterImage(){
        loadImages--
    }

    function setCommandsModel(parameters){
        let model = parameters["Model"];
        let commId = parameters["ViewId"];

        commandsDecoratorContainer.commandId = commId;

        if (Qt.platform.os === "web"){
            buttonModel = model;
            for (let index = 0; index < model.GetItemsCount(); index++){
                let modelIcon = model.GetData("Icon",index)
                let isEnabled = model.GetData("IsEnabled",index)
                let path = modelIcon === "" ? "" : isEnabled ? Style.getIconPath(modelIcon, Icon.State.On, Icon.Mode.Normal) :
                                                               Style.getIconPath(modelIcon, Icon.State.Off, Icon.Mode.Disabled)
                if (path !== ""){
                    path = "/" + context.appName + "/Views/" + path
                    loadImages++;

                    ImageController.load(path, counterImage, counterImage);
                }
            }
        }
        else{
            buttonPanel.buttonModel = model;
            buttonPanel.updateModel();

            Events.sendEvent("CommandsGuiReady");
        }
    }

    function clearModel(parameters){
        if (parameters){
            let commandId = parameters["ViewId"];

            if (commandId !== commandsDecoratorContainer.commandId){
                return;
            }
        }

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

        anchors.verticalCenter: parent ? parent.verticalCenter : undefined;

        width:  parent.width - 2*anchors.leftMargin;
        height: parent.height

        horizontalSpacing: 15;
        verticalSpacing: Style.size_mainMargin;

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

                    checkable: model.IsToggleable !== undefined ? model.IsToggleable : false

                    widthFromDecorator: true;
                    heightFromDecorator: true;

                    property bool isToggled: model.IsToggled !== undefined ? model.IsToggled : false;

                    onIsToggledChanged: {
                        checked = checkable ? model.IsToggled !== undefined ? model.IsToggled : false : false;
                    }

                    onClicked: {
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
                            color: !baseElement ? "transparent" : baseElement.down || baseElement.checked ? Style.buttonPressedColor : baseElement.hovered ?  Style.buttonHoverColor : "transparent"
                            border.color: "transparent"
                        }
                    }

                    property string id: model.Id !== undefined ? model.Id : "";

                    onClicked: {
                        Events.sendEvent(commandsDecoratorContainer.commandId + "CommandActivated", model.Id);
                        if (buttonPanel){
                            buttonPanel.clicked(id);
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

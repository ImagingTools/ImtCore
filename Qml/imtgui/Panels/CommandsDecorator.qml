import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0

Item {
    id: commandsDecoratorContainer;

    property string commandId;

    property string clearEventCommandId: "ClearCommandsGui";
    property string setCommandsVisibleEventCommandId: "SetCommandsVisible";
    property string updateCommandsGuiEventCommandId: "UpdateCommandsGui";

    Component.onCompleted: {
        Events.subscribeEvent(setCommandsVisibleEventCommandId, commandsDecoratorContainer.setVisible);
        Events.subscribeEvent(clearEventCommandId, commandsDecoratorContainer.clearModel);
        Events.subscribeEvent(updateCommandsGuiEventCommandId, commandsDecoratorContainer.setCommandsModel);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(setCommandsVisibleEventCommandId, commandsDecoratorContainer.setVisible);
        Events.unSubscribeEvent(clearEventCommandId, commandsDecoratorContainer.clearModel);
        Events.unSubscribeEvent(updateCommandsGuiEventCommandId, commandsDecoratorContainer.setCommandsModel);
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
            for (let index = 0; index < model.getItemsCount(); index++){
                let modelIcon = model.getData("icon",index)
                let isEnabled = model.getData("isEnabled",index)
                let path = modelIcon === "" ? "" : isEnabled ? Style.getIconPath(modelIcon, Icon.State.On, Icon.Mode.Normal) :
                                                               Style.getIconPath(modelIcon, Icon.State.Off, Icon.Mode.Disabled)
                if (path !== ""){
                    path = "/" + context.appId + "/Views/" + path
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

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.leftMargin: Style.marginM;

        width:  parent.width - 2*anchors.leftMargin;
        height: parent.height

        horizontalSpacing: Style.marginM;
        verticalSpacing: 2;

        openButtonImageSource: "../../../" + Style.getIconPath("Icons/Next", Icon.Mode.On, Icon.State.Active);

        menuAlignRight: true;

        buttonDelegate: Component{
            Item{
                width: isHorizontal ? topButtonDelegate.width : -buttonPanel.horizontalSpacing;
                height: topButtonDelegate.height;

                property bool isHorizontal: model.IsHorizontal == undefined ? true : model.IsHorizontal;

                visible: model.id !== "";
                Button {
                    id: topButtonDelegate;

                    decorator: Style.topButtonDecorator

                    iconSource: model.icon === "" ? "" : model.IsEnabled ? "../../../../" + Style.getIconPath(model.icon, Icon.State.On, Icon.Mode.Normal) :
                                                   "../../../../" + Style.getIconPath(model.icon, Icon.State.Off, Icon.Mode.Disabled);

                    enabled: model.isEnabled;

                    visible: model.visible;

                    text: model.name;

                    checkable: model.isToggleable !== undefined ? model.IsToggleable : false

                    widthFromDecorator: true;
                    heightFromDecorator: true;

                    property bool isToggled: model.isToggled !== undefined ? model.isToggled : false;

                    onIsToggledChanged: {
                        checked = checkable ? model.isToggled !== undefined ? model.isToggled : false : false;
                    }

                    onClicked: {
                        Events.sendEvent(commandsDecoratorContainer.commandId + "CommandActivated", model.id);
                    }
                }
            }
        }

        buttonDelegateVert: Component{
            Item{
                id: verticalComp;

				width: Style.sizeHintXXXS;
                height: isHorizontal ? -buttonPanel.verticalSpacing : model.name == "" ? splitter.height : textButtonDelegateContainer.height;

                property bool isHorizontal: model.IsHorizontal == undefined ? true : model.IsHorizontal;

                Button {
                    id: textButtonDelegateContainer;
                    width: parent.width;
                    visible: model.name !== "";
                    iconSource: model.isEnabled !== "" ? (model.icon !== "" ? "../../../../" + Style.getIconPath(model.icon, Icon.State.On, Icon.Mode.Normal) : "") :
                                                         (model.icon !== "" ? "../../../../" + Style.getIconPath(model.icon, Icon.State.Off, Icon.Mode.Disabled) : "");
                    enabled: model.isEnabled;
                    text: model.name !== undefined ? model.name : "";
                    checked: checkable ? model.isToggled !== undefined ? model.isToggled : false : false;
                    checkable: model.isToggleable !== undefined ? model.isToggleable : false

                    decorator: Component {
                        ButtonDecorator {
                            border.color: "transparent"
                            color: "transparent";
                        }
                    }

                    property string id: model.id !== undefined ? model.id : "";

                    onClicked: {
                        Events.sendEvent(commandsDecoratorContainer.commandId + "CommandActivated", model.id);
                        if (buttonPanel){
                            buttonPanel.clicked(id);
                        }
                    }
                }

                Rectangle{
                    id: splitter;

                    anchors.top: parent.top;

                    width: verticalComp.isHorizontal ? 1 : buttonPanel.verticalMenuWidth;
                    height: model.name == "" && model.index == buttonPanel.horizCount ? -buttonPanel.verticalSpacing : 1;
                    color: Style.textColor;
                    visible: model.name !== "" ? false : model.index == buttonPanel.horizCount ? false : model.index == (buttonPanel.buttonModel.getItemsCount() - 1) ? false : true ;
                }
            }
        }
    }
}

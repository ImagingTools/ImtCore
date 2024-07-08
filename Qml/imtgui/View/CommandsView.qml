import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: commandsItem;
    height: 30;

    property TreeItemModel commandsModel: TreeItemModel {}
    property alias content: content_;

    function addCommandGroup(commands){
        let index = commandsModel.insertNewItem();

        let subModel = commandsModel.addTreeModel("SubElements", index)
        subModel.copy(commands);

        repeater.model = commandsItem.commandsModel;
    }

    function clear(){
        commandsModel.clear();
    }

    Row {
        id: content_;
        height: commandsItem.height;
        spacing: 50;

        Repeater {
            id: repeater;
            delegate: Component {
                Item {
                    id: item;
                    height: commandsItem.height;
                    width: commandsItem.width;
                    property var dataModel: model;

                    ButtonPanel {
                        id: listView;
                        width: commandsItem.width;
                        height: parent.height
                        horizontalSpacing: Style.size_mainMargin;
                        verticalSpacing: 2;
                        openButtonImageSource: "../../../" + Style.getIconPath("Icons/Next", Icon.Mode.On, Icon.State.Active);
                        menuAlignRight: true;
//                        buttonModel: item.dataModel.SubElements;
                        buttonModel: model.SubElements;
                        onButtonModelChanged: {
                            updateModel();
                        }

                        buttonDelegate: Component {
                            Item {
                                width: isHorizontal ? topButtonDelegate.width : -listView.horizontalSpacing;
                                height: topButtonDelegate.height;
                                property bool isHorizontal: model.IsHorizontal == undefined ? true : model.IsHorizontal;
                                visible: model.Id !== "";

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
//                                        Events.sendEvent(commandsDecoratorContainer.commandId + "CommandActivated", model.Id);
                                    }
                                }
                            }
                        }

                        buttonDelegateVert: Component{
                            Item{
                                id: verticalComp;

                                width: 150;
                                height: isHorizontal ? -listView.verticalSpacing : model.Name == "" ? splitter.height : textButtonDelegateContainer.height;

                                property bool isHorizontal: model.IsHorizontal == undefined ? true : model.IsHorizontal;

                                Button {
                                    id: textButtonDelegateContainer;
                                    width: parent.width;
                                    visible: model.Name !== "";
                                    iconSource: model.IsEnabled !== "" ? (model.Icon !== "" ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal) : "") :
                                                                         (model.Icon !== "" ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled) : "");
                                    enabled: model.IsEnabled;
                                    text: model.Name !== undefined ? model.Name : "";
                                    checked: checkable ? model.IsToggled !== undefined ? model.IsToggled : false : false;
                                    checkable: model.IsToggleable !== undefined ? model.IsToggleable : false

                                    decorator: Component {
                                        ButtonDecorator {
                                            border.color: "transparent"
                                            color: "transparent";
                                        }
                                    }

                                    property string id: model.Id !== undefined ? model.Id : "";

                                    onClicked: {
//                                        Events.sendEvent(commandsDecoratorContainer.commandId + "CommandActivated", model.Id);
//                                        if (listView){
//                                            listView.clicked(id);
//                                        }
                                    }
                                }

                                Rectangle{
                                    id: splitter;

                                    anchors.top: parent.top;

                                    width: verticalComp.isHorizontal ? 1 : listView.verticalMenuWidth;
                                    height: model.Name == "" && model.index == listView.horizCount ? -listView.verticalSpacing : 1;
                                    color: Style.textColor;
                                    visible: model.Name !== "" ? false : model.index == listView.horizCount ? false : model.index == listView ? false : true ;
                                }
                            }
                        }
                    }

//                    ListView {
//                        id: listView;
//                        width: contentWidth;
//                        height: parent.height;
//                        boundsBehavior: Flickable.StopAtBounds;
//                        orientation: ListView.Horizontal;
//                        interactive: false;
//                        spacing: Style.size_mainMargin;
//                        model: item.dataModel.SubElements;
//                        delegate: Button {
//                            enabled: model.IsEnabled;
//                            visible: model.Visible;
//                            text: model.Name;
//                            widthFromDecorator: true;
//                            heightFromDecorator: true;
//                            iconSource: model.Icon === "" ? "" : model.IsEnabled ? "../../../../" + Style.getIconPath(model.Icon, Icon.State.On, Icon.Mode.Normal) :
//                                                           "../../../../" + Style.getIconPath(model.Icon, Icon.State.Off, Icon.Mode.Disabled);
//                            decorator: Component {
//                                TopButtonDecorator {}
//                            }
//                        }
//                    }
                }
            }
        }
    }
}


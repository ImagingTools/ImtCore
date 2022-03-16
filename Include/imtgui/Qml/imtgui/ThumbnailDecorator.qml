import QtQuick 2.0
import Acf 1.0
import 'AuxComponents'
import imtauthgui 1.0
import imtlicgui 1.0
import imtqml 1.0;
import imtgui 1.0;

Rectangle
{
    id: thubnailDecoratorContainer;

    anchors.fill: parent;

    width: 300;
    height: 200;

    color: Style.baseColor;

    property Item activeItem;

    function updateModels() {
        console.log("ThumbnailDecorator updateModels()");
        menuPanel.updateModels();
    }

    function openDialog(source, parameters) {
        console.log("ThumbnailDecorator openDialog", source, parameters);

        modelLayers.append({"source": source, "parameters": parameters})

        if (!listViewDialogs.visible){
            listViewDialogs.visible = true;
        }

        console.log("ThumbnailDecorator listViewDialogs.count", listViewDialogs.count);
        return modelLayers.count - 1;
    }

    function dialogIsActive() {
        return modelLayers.GetItemsCount() > 0;
    }

    function closeDialog() {
        var index = modelLayers.count - 1;
        if (index > -1) {
            modelLayers.remove(index);
        }

        if (modelLayers.count === 0){
            listViewDialogs.visible = false;
        }
    }

    ListModel {
        id: modelLayers;
    }

//    TreeItemModel {
//        id: modelLayers;
//    }

    TopPanel {
        id: topPanel;

        title: menuPanel.activePageName;

        onMenuActivatedSignal: {
            console.log("ThumbnailDecorator TopPanel onMenuActivatedSignal", menuId, thubnailDecoratorContainer.activeItem);
            thubnailDecoratorContainer.activeItem.menuActivated(menuId);
        }

        onCommandsChangedSignal: {
            console.log("ThumbnailDecorator TopPanel onCommandsChangedSignal", commandsId);
            thubnailDecoratorContainer.activeItem.commandsChanged(commandsId);
        }
    }

    MenuPanel {
        id: menuPanel;

        anchors.top: topPanel.bottom;
        anchors.bottom: parent.bottom;

        onPagesCountChanged: {
            console.log("ThumbnailDecorator MenuPanel onPagesCountChanged", menuPanel.pagesCount);
            pagesData.model = menuPanel.pagesCount
        }

        onActivePageIdChanged: {
            console.log("ThumbnailDecorator MenuPanel onActivePageIdChanged");
            if (thubnailDecoratorContainer.activeItem){
                thubnailDecoratorContainer.activeItem.updateCommandId();
            }
        }
        onActivePageIndexChanged: {
            console.log("ThumbnailDecorator MenuPanel onActivePageIndexChanged", menuPanel.activePageIndex);
        }
    }

    Repeater {
        id: pagesData;

        clip: true;
        model: menuPanel.model;

        delegate: Rectangle {
            id: pagesDeleg;

            anchors.left: menuPanel.right;
            anchors.right: thubnailDecoratorContainer.right;
            anchors.top: topPanel.bottom;
            anchors.bottom: thubnailDecoratorContainer.bottom;

            color: "transparent";
            visible: menuPanel.activePageIndex === model.index;

            onVisibleChanged: {
                console.log("ThumbnailDecorator Repeater onVisibleChanged", pagesLoader.item, menuPanel.activePageIndex, model.index)
                if(pagesDeleg.visible){
                    console.log("ThumbnailDecorator Repeater onVisibleChanged visible", pagesDeleg.visible)
                    thubnailDecoratorContainer.activeItem = pagesLoader.item;
                    pagesLoader.item.visible = pagesDeleg.visible;

//                    thubnailDecoratorContainer.activeItem.commandsChanged(commandsId);
                }
            }

            function changeCommandsId(commandsId) {
                console.log("ThumbnailDecorator Repeater changeCommandsId", topPanel.activeCommandsModelId , commandsId)

                if (topPanel.activeCommandsModelId === commandsId) {
                    topPanel.activeCommandsModelId = ""
                }

                topPanel.activeCommandsModelId = commandsId
            }

            function setModeMenuButton(commandId, mode) {
                console.log("ThumbnailDecorator Repeater setModeMenuButton", commandId, mode)
                topPanel.setModeMenuButton(commandId, mode);
            }

            Loader {
                id: pagesLoader;

                anchors.fill: parent;

                Component.onCompleted: {
                    var source = menuPanel.model.GetData(PageEnum.SOURCE, model.index);
                    pagesLoader.source = source;
                }

                onItemChanged: {
                    console.log("ThumbnailDecorator Repeater Loader onItemChanged", pagesLoader.source)
                    if (pagesLoader.item){
                        pagesLoader.item.rootItem = pagesDeleg;
                        console.log("ThumbnailDecorator pagesLoader.item.rootItem", pagesLoader.item.rootItem)
                        pagesLoader.item.firstElementImageSource =  menuPanel.model.GetData(PageEnum.ICON, model.index);
                    }
                }
            }
        }
    }

    Repeater {
        id: listViewDialogs;
        z: 10;

        anchors.fill: parent;

        model: modelLayers;
        visible: false;

        onModelChanged: {
            console.log("ThumbnailDecorator ListView onModelChanged!");
        }

        delegate: Rectangle {
            id: delegateListViewDialogs;

            width: thubnailDecoratorContainer.width;
            height: thubnailDecoratorContainer.height;

            color: "transparent";

            Rectangle {
                id: darkBackground;

                anchors.fill: parent;

                color: "gray";

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        console.log("ThumbnailDecorator dialog background clicked!");

                        if (loaderDialog.item.clickBackgroundClose){
                            loaderDialog.closeItem();
                        }

                    }

                    onWheel: {

                    }
                }
            }

            Loader {
                  id: loaderDialog;

                  function closeItem() {
                      console.log("ThumbnailDecorator close dialog", model.index);

                      modelLayers.remove(model.index);

//                      if (modelLayers.count === 0){
//                          listViewDialogs.visible = false;
//                      }
                  }

                  Component.onCompleted: {
                      console.log("model.source", model.source);
                      if (!model.source){
                          return;
                      }

                      loaderDialog.source = model.source;
                  }

                  onItemChanged: {
                      console.log("ThumbnailDecorator loader onItemChanged!");
                      for (var key in model.parameters) {
                          console.log(key, model.parameters[key]);
                          loaderDialog.item[key]  = model.parameters[key];
                      }
                      loaderDialog.item["loaderDialog"] = loaderDialog;

                      if (loaderDialog.item.centered){
                          loaderDialog.anchors.centerIn = delegateListViewDialogs;
                      }

                      darkBackground.opacity = loaderDialog.item.backgroundOpacity;
                  }
             }
        }
    }
}

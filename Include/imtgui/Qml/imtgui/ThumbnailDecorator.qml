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
//        modelLayers.insert(0, {"source": source, "parameters": parameters})
        modelLayers.append({"source": source, "parameters": parameters});

        for (var i = 0; i < modelLayers.count; i++) {
            console.log(i, modelLayers.get(i).source);
        }

        console.log("ThumbnailDecorator listViewDialogs.count", listViewDialogs.count);
        return modelLayers.count - 1;
    }

    function dialogIsActive() {
        return modelLayers.count > 0;
    }

    function getIndexActiveDialog() {
        return modelLayers.count - 1;
    }

    function closeDialog() {

        var index = thubnailDecoratorContainer.getIndexActiveDialog();
        console.log("ThumbnailDecorator closeDialog ", index);
        if (index > -1) {
            modelLayers.remove(index);
        }
    }

    ListModel {
        id: modelLayers;
    }

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

//    Loader {
//        id: dialogLoader;
//    }

    ListView {
        id: listViewDialogs;
        anchors.fill: parent;
        model: modelLayers;
        visible: modelLayers.count > 0;
        z: 10;
        boundsBehavior: Flickable.StopAtBounds;

        onModelChanged: {
            console.log("ThumbnailDecorator ListView onModelChanged!");
//            loaderDialog.source = model.source;
        }

        delegate: Rectangle {
            id: delegateListViewDialogs;
            //anchors.fill:thubnailDecoratorContainer;
            width: thubnailDecoratorContainer.width;
            height: thubnailDecoratorContainer.height;

            color: "transparent";

            Rectangle {
                id: darkBackground;
                anchors.fill: parent;
                color: "gray";
                opacity: loaderDialog.item.backgroundOpacity;

                MouseArea {
                    anchors.fill: parent;

                    visible: loaderDialog.item.clickBackgroundClose;

                    onClicked: {
                        thubnailDecoratorContainer.closeDialog();
                    }
                }
            }

            Loader {
                  id: loaderDialog;

                  anchors.centerIn: loaderDialog.item.centered ? delegateListViewDialogs : "";

                  function closeItem() {
                      console.log("ThummnailDecorator close dialog", model.index);
                      modelLayers.remove(model.index);
                  }

                  Component.onCompleted: {
                      console.log("model.source", model.source);
                      loaderDialog.source = model.source;
                  }

                  onItemChanged: {
                      console.log("ThumbnailDecorator loader onItemChanged!");
                      for (var key in model.parameters) {
                          console.log(key, model.parameters[key]);
                          loaderDialog.item[key]  = model.parameters[key];
                      }
                      loaderDialog.item["loaderDialog"] = loaderDialog;
                      console.log(loaderDialog.item.activeFocus);
                  }


             }
        }
    }
}

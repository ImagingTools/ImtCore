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
   // color: "white";
    color: Style.baseColor;
    property Item activeItem;

    function updateModels() {
        console.log("ThumbnailDecorator updateModels()");
        menuPanel.updateModels();
    }

    function openDialog(source, parameters) {
        console.log("ThumbnailDecorator openDialog", source, parameters);
        modelLayers.append({"source": source, "parameters": parameters});
        return modelLayers.count - 1;
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
                console.log("ThumbnailDecorator Repeater onVisibleChanged", loader.item, menuPanel.activePageIndex, model.index)
                if(pagesDeleg.visible){
                    thubnailDecoratorContainer.activeItem = loader.item;
                    loader.item.visible = pagesDeleg.visible;
                }
            }

            function changeCommandsId(commandsId) {
                console.log("ThumbnailDecorator Repeater changeCommandsId", topPanel.activeCommandsModelId , commandsId)
                topPanel.activeCommandsModelId = commandsId
            }

            function setModeMenuButton(commandId, mode) {
                console.log("ThumbnailDecorator Repeater setModeMenuButton", commandId, mode)
                topPanel.setModeMenuButton(commandId, mode);
            }

            Loader {
                id: loader;
                anchors.fill: parent;

                Component.onCompleted: {
                    loader.source = "../imtlicgui/" + menuPanel.model.GetData(PageEnum.ID, model.index) + "MultiDocView.qml";
                    console.log("ThumbnailDecorator loader.source", loader.source);
                }
                onItemChanged: {
                    console.log("ThumbnailDecorator Repeater Loader onItemChanged", loader.item)
                    if (loader.item){
                        loader.item.firstElementImageSource =  menuPanel.model.GetData(PageEnum.ICON, model.index);
                    }
                }

            }
        }
    }

//    TreeView {
//        id: treeView;
//        width: 200;
//        //height: parent.height - topPanel.height;
//        anchors.top: topPanel.bottom;
//        anchors.right: parent.right;
//    }

    Loader {
        id: dialogLoader;
    }

    ListView {
        id: listViewDialogs;

        model: modelLayers;

        onModelChanged: {
            console.log("ThumbnailDecorator ListView onModelChanged!");
            loaderDialog.source = model.source;
        }

        delegate: Item {
            id: delegateListViewDialogs;
            //anchors.fill:thubnailDecoratorContainer;
            width: thubnailDecoratorContainer.width;
            height: thubnailDecoratorContainer.height;

            Rectangle {
                id: darkBackground;
                anchors.fill: parent;
                color: "gray";
                opacity: 0.4;

                visible: loaderDialog.item.visible;

                MouseArea {
                    anchors.fill: parent;
                }
            }

            Loader {
                  id: loaderDialog;
                  anchors.centerIn: delegateListViewDialogs;

                  function closeItem() {
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
                  }


             }
        }
    }
}

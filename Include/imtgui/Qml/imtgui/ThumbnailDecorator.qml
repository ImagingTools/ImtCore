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

    height: 480;
    width: 740;

    color: Style.backgroundColor;

    property Item activeItem;
    property TreeItemModel localSettings;
    property bool serverIsConnection: false;

    property int mainMargin: 10;

    Component.onCompleted: {
    }

    onLocalSettingsChanged: {
        console.log("ThumbnailDecorator onLocalSettingsChanged");
        //console.log("localSettings", thubnailDecoratorContainer.localSettings.toJSON());
    }

    function setFocusOnMenuPanel(){
        menuPanel.forceActiveFocus();
    }

    function updateModels() {
        console.log("ThumbnailDecorator updateModels()");
        Style.changeSchemeDesign("");
        menuPanel.updateModels();
        topPanel.updateModels();
    }

    function openDialog(source, parameters) {
        console.log("ThumbnailDecorator openDialog", source, parameters);

        modelLayers.append({"source": source, "parameters": parameters})

        if (!listViewDialogs.visible){
            listViewDialogs.visible = true;
        }

        return modelLayers.count - 1;
    }

    function dialogIsActive() {
        return modelLayers.count > 0;
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

    function setInvalidConnection(state){
        console.log("ThumbnailDecorator setInvalidConnection", state);
        thubnailDecoratorContainer.serverIsConnection = !state;
        refreshButton.enabled = true;
//        textNoConnection.visible = state;
        errorBackground.visible = state;
    }

    MenuPanel {
        id: menuPanel;

        anchors.left: parent.left;
        anchors.leftMargin: thubnailDecoratorContainer.mainMargin;
        anchors.top: topPanel.bottom;
        anchors.topMargin: thubnailDecoratorContainer.mainMargin;
//        anchors.bottom: customPanel.visible ? customPanel.top : parent.bottom;
        anchors.bottom: customPanel.top;
        anchors.bottomMargin: customPanel.visible ? 0 : thubnailDecoratorContainer.mainMargin;

        //visible: thubnailDecoratorContainer.serverIsConnection;
        focus: true;

        Keys.onPressed: {
            console.log("ThumbnailDecorator MenuPanel keys pressed")
            if (event.key === Qt.Key_Tab){
                console.log('Key tab was pressed');
                //event.accepted = true;
//                pagesLoader.item.setFocus();
                thubnailDecoratorContainer.activeItem.setFocus();
            }
            else if (event.key === Qt.Key_Up){
                console.log('Key up was pressed');
                if (menuPanel.activePageIndex == 0){
                    menuPanel.activePageIndex = menuPanel.pagesCount - 1;
                }
                else
                    menuPanel.activePageIndex--;
            }
            else if (event.key === Qt.Key_Down){
                console.log('Key down was pressed');
                if (menuPanel.activePageIndex == menuPanel.pagesCount - 1){
                    menuPanel.activePageIndex = 0;
                }
                else
                    menuPanel.activePageIndex++;
            }
        }

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

    Item {
        id: customPanel;

//        anchors.top: menuPanel.bottom;
//        anchors.topMargin: thubnailDecoratorContainer.mainMargin;
        anchors.left: parent.left;
        anchors.leftMargin: thubnailDecoratorContainer.mainMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: thubnailDecoratorContainer.mainMargin;

        Rectangle {
            anchors.fill: parent;
            color: "red";
        }

        //visible: false;

        Loader {
            id: loaderCustomPanel;

            source: Style.customPanelDecoratorPath;

            onItemChanged: {
                if (loaderCustomPanel.item){
                    console.log("loaderCustomPanel ", Style.customPanelDecoratorPath);
                    customPanel.height = loaderCustomPanel.item.height;
                    customPanel.width = loaderCustomPanel.item.width;
                    customPanel.visible = true;
                    console.log("loaderCustomPanel.item.height ", loaderCustomPanel.item.height);
                }
            }
        }
    }

    FeaturesTreeView {
        id: featuresTreeView;
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
                    console.log("PagesLoader ", source);
                    pagesLoader.source = source;
                    console.log("PagesLoader ", pagesLoader.source);
                }

                onItemChanged: {
                    console.log("ThumbnailDecorator Repeater Loader onItemChanged", pagesLoader.source)
                    if (pagesLoader.item){
                        pagesLoader.item.rootItem = pagesDeleg;
                         pagesLoader.item.title = menuPanel.model.GetData(PageEnum.NAME, model.index);
//                        pagesLoader.item.forceActiveFocus();
                        console.log("ThumbnailDecorator pagesLoader.item.rootItem", pagesLoader.item.rootItem)
                        pagesLoader.item.firstElementImageSource =  menuPanel.model.GetData(PageEnum.ICON, model.index);
                    }
                }
            }
        }
    }

    Rectangle {
        id: errorBackground;

        anchors.fill: parent;

        color: "transparent";

        visible: false;

        MouseArea {
            anchors.fill: parent;
        }

        Rectangle {
            anchors.verticalCenter: errorBackground.verticalCenter;
            anchors.left: errorBackground.left;
            anchors.right: errorBackground.right;

            height: 300;

            color: Style.backgroundColor;
            opacity: 0.8;

            Text {
                id: textNoConnection;

                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;

                text: qsTr("There is no connection to the server!");
                //visible: false;
                color: "red";
                font.pixelSize: Style.fontSize_title;
                font.family: Style.fontFamily;
            }

            AuxButton {
                id: refreshButton;

                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.bottom: parent.bottom;
                anchors.bottomMargin: 50;

                width: 70;
                height: 25;
                backgroundColor: Style.imagingToolsGradient1;

                hasText: true;
                textButton: qsTr("Refresh");

                onClicked: {
                    refreshButton.enabled = false;
                    thubnailDecoratorContainer.updateModels();
                }
            }
        }

    }

    TopPanel {
        id: topPanel;
//        anchors.left: parent.left;
//        anchors.right: parent.right;
        anchors.top: parent.top;
        width: parent.width;
        height: 55;
//        width: parent.width;

        title: menuPanel.activePageName;
        onMenuActivatedSignal: {
            console.log("ThumbnailDecorator TopPanel onMenuActivatedSignal");
            console.log("menuId", idMenu);
            thubnailDecoratorContainer.activeItem.menuActivated(idMenu);
        }

        onCommandsChangedSignal: {
            console.log("ThumbnailDecorator TopPanel onCommandsChangedSignal", commandsId);
            thubnailDecoratorContainer.activeItem.commandsChanged(commandsId);
        }

        MouseArea {
            id: rectBlock;

            anchors.horizontalCenter: topPanel.horizontalCenter;

            width: topPanel.width - 100;
            height: topPanel.height;

            visible: errorBackground.visible;
        }
    }


//    Connections {
//        target: Qt.application;

//        onAboutToQuit: {
//            console.log("Quit application");
//            listViewDialogs.destroy();
//        }
//    }

    Repeater {
        id: listViewDialogs;
        z: 10;

        width: parent.width;
        height: parent.height;

        model: modelLayers;

        onModelChanged: {
            console.log("ThumbnailDecorator ListView onModelChanged!");
        }

        delegate: Rectangle {
            id: delegateListViewDialogs;

            width: thubnailDecoratorContainer.width;
            height: thubnailDecoratorContainer.height;

            color: "transparent";

//            Timer {
//                id: timepBackground;

//                Component.onCompleted: {
//                    timepBackground.start(100);

//                }

//                onTriggered: {
//                    darkBackground.visible = true;
//                }
//            }


            Rectangle {
                id: darkBackground;

                anchors.fill: parent;

                color: "gray";
//                visible: false;

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        console.log("ThumbnailDecorator dialog background clicked!");

                        if (loaderDialog.item.clickBackgroundClose){
                            loaderDialog.closeItem();
                        }
                    }

                    onWheel: {}
                }
            }

            function setLoaderItem(mySource){
               // loaderDialog.source = mySource;
                loaderDialog.destroy();
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
                      loaderDialog.item["thumbnailItem"] = thubnailDecoratorContainer;
                      loaderDialog.item.visible = true;
                      loaderDialog.item.forceActiveFocus();
                  }
             }
        }
    }
}

import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: dialogContainer;

    width: 300;
    height: dialogColumn.height;

    color: Style.backgroundColor;

    property string title;
    property string bodySource;
    property string topPanelSource;

    property bool centered: true;
    property bool hasIcon: true;

    property Item root: null;
    property Item rootItem: null;
    property Item bodyItem: loaderBodyDialog.item;

    property alias topPanel: loaderTopPanel.item;
    property alias buttons: buttonsDialog;
    property alias buttonsModel: buttonsDialog.buttons;

    property string notClosingButtons: "";

    signal finished(string buttonId);
    signal started();
    signal localizationChanged(string language);

    property alias contentComp: loaderBodyDialog.sourceComponent;
    property alias topPanelComp: loaderTopPanel.sourceComponent;
    property alias contentItem: loaderBodyDialog.item;

    property bool canMove: false;

    property Component topPanelDefault: Component{
        TopPanelDialog {}
    }

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", dialogContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", dialogContainer.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        dialogContainer.localizationChanged(language);
    }

    Keys.onPressed: {
        console.log("Dialog onPressed", event.key);
    }

    onFocusChanged: {
        console.log("Dialog onFocusChanged", dialogContainer.focus);

        if (dialogContainer.focus && loaderBodyDialog.item){
            loaderBodyDialog.item.focus = true;
        }
    }

    onWidthChanged: {
        if (loaderTopPanel && loaderTopPanel.item){
            loaderTopPanel.item.width = dialogContainer.width;
        }

        if (loaderBodyDialog && loaderBodyDialog.item){
            loaderBodyDialog.item.width = dialogContainer.width;
        }
    }

    onRootChanged: {
        root.backgroundItem.opacity = 0.4;
    }

    onFinished: {
        if (dialogContainer.root){
            if(dialogContainer.notClosingButtons.indexOf(buttonId) == -1){
                dialogContainer.root.closeDialog();
            }
        }
    }

    onTitleChanged: {
        if(loaderTopPanel.item){
            console.log("Dialog onTitleChanged", dialogContainer.title);
            loaderTopPanel.item.title = dialogContainer.title;
        }
    }

    onBodySourceChanged: {
        loaderBodyDialog.source = dialogContainer.bodySource;
    }

    onTopPanelSourceChanged: {
        loaderTopPanel.source = dialogContainer.topPanelSource;
    }

    onWidthChanged: {
        if(loaderTopPanel.item){
            loaderTopPanel.item.width = dialogContainer.width;
        }
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: {}
    }

    Column {
        id: dialogColumn;

        width: dialogContainer.width;

        Item {
            id: topPanelContainer;

            width: loaderTopPanel.width;
            height: loaderTopPanel.height;

            onWidthChanged: {
                console.log("topPanelContainer onWidthChanged", width);

            }

            onHeightChanged: {
                console.log("topPanelContainer onHeightChanged", height);

            }

            MovingItem {
                visible: dialogContainer.canMove;
                containerItem: topPanelContainer;
                globalParent: dialogContainer.root;
                movingItem: dialogContainer;
            }

            Loader {
                id: loaderTopPanel;

                source: "../../../../qml/imtgui/AuxComponents/Dialogs/TopPanelDialog.qml";
                onLoaded:  {
                    console.log("loaderTopPanel onLoaded");

                    loaderTopPanel.item.width = dialogContainer.width;
                    if(loaderTopPanel.item.title !== undefined){
                        loaderTopPanel.item.title = dialogContainer.title;
                    }

                    console.log("loaderTopPanel.item.height", loaderTopPanel.item.height);

                    loaderTopPanel.width = loaderTopPanel.item.width;
                    loaderTopPanel.height = loaderTopPanel.item.height;

                    if (loaderTopPanel.item.closeButtonClicked){
                        loaderTopPanel.item.closeButtonClicked.connect(dialogContainer.finished);
                    }
                }
                onItemChanged: {
                    console.log("loaderTopPanel onItemChanged");

                    //loaderTopPanel.item.closeButtonClicked.connect(dialogContainer.finished);
                    if(loaderTopPanel.item){
                        loaderTopPanel.item.width = dialogContainer.width;
                        if(loaderTopPanel.item.title !== undefined){
                            loaderTopPanel.item.title = dialogContainer.title;
                        }
                    }
                                   }
            }
        }

        Loader {
            id: loaderBodyDialog;

            sourceComponent: dialogContainer.contentComp;

            onLoaded: {
                loaderBodyDialog.item.width = dialogContainer.width;
                if(loaderBodyDialog.item.rootItem !== undefined){
                    loaderBodyDialog.item.rootItem = dialogContainer;
                }
            }

            onItemChanged: {
                if (loaderBodyDialog.item){
                    loaderBodyDialog.item.width = dialogContainer.width;
                }
            }
        }

        Item {
            anchors.right: parent.right;
            anchors.rightMargin: 10;

            width: buttonsDialog.width;
            height: buttonsDialog.height + 10;

            ButtonsDialog {
                id: buttonsDialog;

                anchors.right: parent.right;
                anchors.bottom: parent.bottom;
                anchors.bottomMargin: 10;

                onButtonClicked: {
                    console.log("ButtonsDialog onButtonClicked", buttonId);
                    dialogContainer.finished(buttonId);
                }
            }
        }
    }

    Shortcut {
        sequence: "Escape";
        enabled: true;
        onActivated: {
            dialogContainer.finished("Close")
        }
    }
}

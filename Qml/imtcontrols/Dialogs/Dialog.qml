import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0


ControlBase {
    id: dialogContainer;

    width: Style.isQtStyle ? !decorator ? 0 : decorator.width : dialogColumn.width;
    height: Style.isQtStyle ? !decorator ? 0 : decorator.height : dialogColumn.height;

    decoratorComponent: Style.isQtStyle ? DecoratorsQt.dialogDecorator: Decorators.dialogDecorator;
    property var decoratorItem: null;

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

    property string backgroundColor: Style.backgroundColor;
    property alias radius: background.radius;

    property string notClosingButtons: "";


    property alias topPanelComp: loaderTopPanel.sourceComponent;
    property alias contentItem: loaderBodyDialog.item;

    property Component contentComp;//: loaderBodyDialog.sourceComponent;
    property Component emptyComp : Component{Item {}};

    property int buttonIds: 0;//81920;

    property bool canMove: false;
    property bool decoratorVisible: !decorator ? true: decorator.visible;

    property Component topPanelDefault: Component{
        TopPanelDialog {}
    }

    signal finished(string buttonId);
    signal started();
    signal localizationChanged(string language);

    signal accepted();
    signal canceled();
    signal applied();


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
            //loaderBodyDialog.item.width = dialogContainer.width;
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

    onAccepted: {
        //console.log("Dialog::accepted()");
        finished(Enums.ButtonType.Ok)
    }
    onCanceled: {
        //console.log("Dialog::canceled()");
        finished(Enums.ButtonType.Cancel)
    }

    onDecoratorVisibleChanged: {
        if(!decoratorVisible){
            root.closeDialog();
        }
    }


    onDecoratorComponentChanged: {
        if(decorator){
            if(decorator.accepted !==undefined){
                decorator.accepted.connect(this.accepted);
            }
            if(decorator.rejected !==undefined){
                decorator.rejected.connect(this.canceled);
            }
            if(decorator.applied !==undefined){
                decorator.applied.connect(this.applied);
            }

           addContentItemToDecorator();
        }
    }

    function addContentItemToDecorator(){
        if(Style.isQtStyle){
            decorator.buttonIds = dialogContainer.buttonIds;
            var content_ = dialogContainer.contentComp.createObject(decorator);
            dialogContainer.decorator.contentItem.children.push(content_);

            dialogContainer.decoratorItem = content_;

            setDecoratorSize(content_)

            if(decorator.rootItem !==undefined){
                decorator.rootItem = dialogContainer;
            }

        }//if(Style.isQtStyle)
    }


    function setButtonIds(){
            let buttonIds = 0;
            for(let i = 0; i < buttonsDialog.buttons.count; i++){
                let id = buttonsDialog.buttons.get(i).Id;
                if(i == 0){
                    buttonIds = id;

                }
                else {
                    buttonIds = buttonIds | id;
                }

            }
            dialogContainer.buttonIds = buttonIds;
    }

    function setDecoratorSize(content_){
        if(Style.isQtStyle){
            let width_  = content_.width +
                dialogContainer.decorator.leftPadding +
                dialogContainer.decorator.rightPadding;
            dialogContainer.decorator.width = Math.max(width_, dialogContainer.decorator.footer.width);

            let hasButtons = dialogContainer.buttonIds !== 0;
            dialogContainer.decorator.height = content_.height +
                    dialogContainer.decorator.header.height +
                    dialogContainer.decorator.footer.height * hasButtons +
                    dialogContainer.decorator.topPadding +
                    dialogContainer.decorator.bottomPadding;
        }

        content_.x = (dialogContainer.decorator.width - content_.width)/2 -
                dialogContainer.decorator.leftPadding;
    }


    MouseArea {
        anchors.fill: parent;
        onClicked: {}
    }

    Rectangle{
        id: background;

        anchors.fill: parent;

        color: dialogContainer.backgroundColor;
        visible: !Style.isQtStyle;
    }

    DropShadow {
        id: dropShadow;

        anchors.fill: background;

        horizontalOffset: 2;
        verticalOffset: 2;

        radius: background.radius;
        color: Style.shadowColor;

        source: background;
    }

    Column {
        id: dialogColumn;

        width: Math.max(buttonsWidth, bodyWidth)//buttonsWidth;
        visible: !Style.isQtStyle;
        spacing: 10;

        property real bodyWidth: !loaderBodyDialog.item ? 1 : loaderBodyDialog.item.width + 20;
        property real buttonsWidth: buttonsContainer.width + 2 * buttonsContainer.anchors.rightMargin;

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

            anchors.horizontalCenter: parent.horizontalCenter;

            sourceComponent: !Style.isQtStyle ? dialogContainer.contentComp : dialogContainer.emptyComp;
            onLoaded: {
                //loaderBodyDialog.item.width = dialogContainer.width;
                if(loaderBodyDialog.item.rootItem !== undefined){
                    loaderBodyDialog.item.rootItem = dialogContainer;
                }
            }

            onItemChanged: {
                if (loaderBodyDialog.item){
                    //loaderBodyDialog.item.width = dialogContainer.width;
                }
            }
        }

        Item {
            id: buttonsContainer;

            anchors.right: parent.right;
            anchors.rightMargin: 10;

            width: buttonsDialog.width;
            height: buttonsDialog.height + 2* buttonsDialog.anchors.bottomMargin;

            ButtonsDialog {
                id: buttonsDialog;

                anchors.right: parent.right;
                anchors.bottom: parent.bottom;
                anchors.bottomMargin: 10;

                onButtonsChanged: {
                    dialogContainer.setButtonIds();
                }

                onCountChanged: {
                    dialogContainer.setButtonIds();
                }

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
            //dialogContainer.finished("Cancel")
            dialogContainer.finished(Enums.ButtonType.Cancel)
        }
    }
}

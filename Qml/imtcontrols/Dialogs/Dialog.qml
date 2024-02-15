import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0


ControlBase {
    id: dialogContainer;

    width:  !decorator_ ? 0 :  decorator_.width ;
    height:  !decorator_ ? 0 :  decorator_.height ;

    decorator: Style.dialogDecorator;
    property var decoratorItem: null;

    property string title;
    property string bodySource;
    property string topPanelSource;

    property bool centered: true;
    property bool hasIcon: true;
    property bool modal: false;

    property Item root: null;
    property Item rootItem: null;

    property ListModel buttonsModel: ListModel{};
    property int buttonsModelCount: buttonsModel.count;
    property string notClosingButtons: "";
    property int buttonIds: 0;//81920;

    property string backgroundColor: Style.dialogBackgroundColor;
    property int radius: 0;

    property bool canMove: false;
    property bool decoratorVisible: !decorator_ ? true: decorator_.visible;

    property Component topPanelComp: Component{TopPanelDialog{}};
    property Component topPanelDefault: Component{TopPanelDialog{}};
    property Component contentComp: null;

    property Item topPanel: null;
    property Item contentItem: null;
    property Item bodyItem: null;
    property Item buttons: null;

    signal finished(int buttonId);
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

    onAccepted: {
        //console.log("Dialog::accepted()");
        finished(Enums.ok)
    }
    onCanceled: {
        //console.log("Dialog::canceled()");
        finished(Enums.cancel)
    }

    onDecoratorVisibleChanged: {
        if(!decoratorVisible){
            root.closeDialog();
        }
    }

    onDecoratorChanged: {
        if(decorator_){
            if(decorator_.accepted !==undefined){
                decorator_.accepted.connect(this.accepted);
            }
            if(decorator_.rejected !==undefined){
                decorator_.rejected.connect(this.canceled);
            }
            if(decorator_.applied !==undefined){
                decorator_.applied.connect(this.applied);
            }

        }
    }

    onButtonsModelChanged: {
        setButtonIds();
    }

    onButtonsModelCountChanged: {
        setButtonIds();
    }

    function setButtonIds(){
        if(!buttonsModel){
            return;
        }
        let buttonIds = 0;
        for(let i = 0; i < dialogContainer.buttonsModel.count; i++){
            let id = dialogContainer.buttonsModel.get(i).Id;
            if(i == 0){
                buttonIds = id;
            }
            else {
                buttonIds = buttonIds | id;
            }
        }
        console.log("buttonIds", buttonIds);

        dialogContainer.buttonIds = buttonIds;
    }

    Shortcut {
        sequence: "Escape";
        enabled: true;
        onActivated: {
            //dialogContainer.finished("Cancel")
            dialogContainer.finished(Enums.cancel)
        }
    }
}

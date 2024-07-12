pragma Singleton

import QtQuick 2.12
import Acf 1.0
import QtQuick.Window 2.2

Item {
    id: container;

    property var backgroundItem: null;
    property Item activeView: null; // Active view for this Dialog Manager
    property Item topItem: null;
    property ListModel modalDialogModels: ListModel {}
    property alias count: container.modalDialogModels.count;

    Component.onDestruction: {
        modalDialogModels.clear();
    }

    /*
        This signal is emitted when finished top level dialog.
        \c finished.
    */
    signal finished(string result);

    function openDialog(comp, parameters, mode, callback){
        let dialogMode = mode !== undefined ? mode : Style.dialogMode !== undefined ? Style.dialogMode: "Internal";

        /*for Windows style dialogs*/
        if(dialogMode == "External"){
            container.openWindow(comp, parameters);
        }
        else {
            modalDialogModels.append({"Component": comp, "Parameters": parameters});

            if (callback){
                finished.connect(callback);
            }
        }
    }

    function closeDialog(index){
        if (!index){
            index = -1;

            if (modalDialogModels.count > 0){
                index = modalDialogModels.count - 1;
            }
        }

        if (index >= 0 && index < modalDialogModels.count){
            modalDialogModels.remove(index);
        }
    }

    function closeByComp(comp){
        for (let i = 0; i < modalDialogModels.count; i++){
            let c = modalDialogModels.get(i).Component;
            if (c && c === comp){
                modalDialogModels.remove(i);
                return true;
            }
        }

        return false;
    }

    function showWarningDialog(message){
        openDialog(warningDialog, {"message": message});
    }

    function showCriticalDialog(message){
        openDialog(criticalDialog, {"message": message});
    }

    property Component warningDialog: Component{
        ErrorDialog {
            title: qsTr("Warning message");
            onFinished: {}
        }
    }

    property Component criticalDialog: Component{
        CriticalErrorDialog {
            title: qsTr("Critical message");
            onFinished: {}
        }
    }

    /*for Windows style dialogs*/
    function openWindow(comp, parameters){
        let obj = windowComp.createObject(container);
        obj.parameters = parameters;
        obj.sourceComponent = comp;
        for (let key in parameters) {
            if(key == "modality"){
                obj[key] = parameters[key];
            }
        }
        obj.show();
    }

    property Component windowComp: Component {
        Window {
            id: windowObj;

            title: " ";

            flags: Qt.Dialog;
            modality: Qt.NonModal;//Qt.WindowModal

            width: 100;
            height: 100;

            property var parameters;
            property alias sourceComponent: windowLoader.sourceComponent;
            property Item item: !windowLoader.item ? null : windowLoader.item;

            function closeFunc(buttonId){
                windowObj.close();
            }

            Component.onDestruction:  {
            }

            onClosing:{
                windowObj.destroy();
            }

            onWidthChanged: {
                if(windowLoader.item){
                    windowLoader.item.width = windowObj.width;
                }
            }

            onHeightChanged: {
                if(windowLoader.item){
                    windowLoader.item.height = windowObj.height;
                }
            }

            Loader{
                id: windowLoader;

                onLoaded: {
                    item.finished.connect(windowObj.closeFunc);
                    windowObj.width = item.width;
                    windowObj.height = item.height;

                    for (let key in windowObj.parameters) {
                        if(item[key] !== undefined){
                            item[key] = windowObj.parameters[key];
                        }
                    }
                }
            }
        }
    }
}

import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

Item{
    id: dialog;

    property string title;
    property Component contentComp;
    property ListModel buttonsModel: ListModel{};

    property string backgroundColor: Style.backgroundColor;
    property int radius: 0;

    property string notClosingButtons: "";
    property bool centered: true;
    property bool hasIcon: true;

    property string bodySource;
    property string topPanelSource;

    signal finished(string buttonId);

    signal accepted();
    signal canceled();
    signal applied();

    function open(){
        var parameters = {"centered": true};
        modalDialogManager.openDialog(dialogComp, parameters);
    }

    Component{
        id: dialogComp;

        Dialog {
            id: dialogComponent;

            title: dialog.title;
            contentComp: dialog.contentComp;
            buttonsModel: dialog.buttonsModel;

            backgroundColor: dialog.backgroundColor;
            radius: dialog.radius;

            notClosingButtons: dialog.notClosingButtons;
            centered: dialog.centered;
            hasIcon: dialog.hasIcon;

            bodySource: dialog.bodySource;
            topPanelSource: dialog.topPanelSource;

            Component.onCompleted: {
                dialogComponent.finished.connect(dialog.finished);

                dialogComponent.accepted.connect(dialog.accepted);
                dialogComponent.canceled.connect(dialog.canceled);
                dialogComponent.applied.connect(dialog.applied);
            }

        }

    }
}



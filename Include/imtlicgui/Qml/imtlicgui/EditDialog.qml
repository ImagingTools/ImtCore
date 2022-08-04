import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Dialog {
    id: container;

    property string titleId;
    property string titleName;

    property string valueId;
    property string valueName;

    property TreeItemModel model;

    onFinished: {
        if (buttonId === "Ok"){
            container.valueId = container.bodyItem.inputId;
            container.valueName = container.bodyItem.inputName;
        }
    }

    onTitleIdChanged: {
        console.log("EditDialog onTitleIdChanged", titleId);
        container.bodyItem.titleId = container.titleId;
        console.log("container.bodyItem.titleId", container.bodyItem.titleId);
    }

    onTitleNameChanged: {
        container.bodyItem.titleName = container.titleName;
    }

    onValueIdChanged: {
        container.bodyItem.inputId = container.valueId;
    }

    onValueNameChanged: {
        container.bodyItem.inputName = container.valueName;
    }

    Component.onCompleted: {
        container.buttons.addButton({"Id":"Ok", "Name":"OK"});
        container.buttons.addButton({"Id":"Cancel", "Name":"Cancel"});

        container.bodySource = "../../../imtlicgui/EditDialogBody.qml";
        container.title = qsTr("Edit");
    }

    function setInputNameValidator(validator){
//        container.bodyItem.inputName.setValidator(validator);
    }

    function setInputIdValidator(validator){
//        container.bodyItem.inputId.setValidator(validator);
    }
}

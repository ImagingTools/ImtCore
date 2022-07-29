import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Dialog {
    id: container;

    property string titleId;
    property string titleName;

    property string valueId;
    property string valueName;

    onFinished: {
        if (buttonId === "Ok"){
            container.valueId = container.bodyItem.valueId;
            container.valueName = container.bodyItem.valueName;
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
        container.bodyItem.valueId = container.valueId;
    }

    onValueNameChanged: {
        container.bodyItem.valueName = container.valueName;
    }

    Component.onCompleted: {
        container.buttons.addButton({"Id":"Ok", "Name":"OK"});
        container.buttons.addButton({"Id":"Cancel", "Name":"Cancel"});

        container.bodySource = "../../../imtlicgui/EditDialogBody.qml";
        container.title = qsTr("Edit");
    }
}

import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Dialog {
    id: editDialogContainer;

    width: 300;

    property string titleId: qsTr("License-ID");
    property string titleName: qsTr("License Name");

    property string valueId: null;
    property string valueName: null;

    property TreeItemModel licensesModel;

    property int index: -1;

    contentComp: Component { EditLicenseDialogBody {
    } }

    Component.onCompleted: {
        editDialogContainer.buttons.addButton({"Id": "Ok", "Name": "OK", "Enabled": true});
        editDialogContainer.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        editDialogContainer.title = qsTr("Edit License");
    }
}

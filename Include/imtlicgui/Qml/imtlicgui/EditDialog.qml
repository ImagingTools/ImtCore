import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Dialog {
    id: editDialogContainer;

    width: 700;

    property string titleId;
    property string titleName;

    property string valueId: null;
    property string valueName;

    property bool autoGenerate: false;

    property TreeItemModel model;
    property alias subFeaturesModel: subFeaturesCopyModel;

    property int index: -1;

    content: EditDialogBody {
    }

    TreeItemModel {
        id: subFeaturesCopyModel;
    }

    Component.onCompleted: {
        editDialogContainer.buttons.addButton({"Id": "Ok", "Name": "OK", "Enabled": true});
        editDialogContainer.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        editDialogContainer.title = qsTr("Edit");
    }

    onFinished: {
        if (buttonId === "Ok"){
            let subFeaturesModel = model.GetData("ChildModel", index);

            let json = editDialogContainer.subFeaturesModel.toJSON();
            subFeaturesModel.Parse(json);

//            editDialogContainer.valueId = editDialogContainer.bodyItem.inputId;
//            editDialogContainer.valueName = editDialogContainer.bodyItem.inputName;
        }
    }

    onValueIdChanged: {
        getSubFeaturesFromModel();
    }

    function getSubFeaturesFromModel(){
        let subFeaturesModel = model.GetData("ChildModel", index);
        if (!subFeaturesModel){
            subFeaturesModel = model.AddTreeModel("ChildModel", index);
        }

        let json = subFeaturesModel.toJSON();
        editDialogContainer.subFeaturesModel.Parse(json);
    }
}

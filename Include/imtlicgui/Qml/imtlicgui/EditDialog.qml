import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Dialog {
    id: editDialogContainer;

    width: 700;

    property string titleId: qsTr("Feature-ID");
    property string titleName: qsTr("Feature Name");

    property string valueId: null;
    property string valueName: null;

    property bool autoGenerate: false;

    property TreeItemModel featuresModel;
    property alias subFeaturesModel: subFeaturesCopyModel;
    property alias dialogModel: dialogModelObj;

    property int index: -1;

    contentComp: EditDialogBody {
        id: dialogBody;

        rootItem: editDialogContainer;
    }

    TreeItemModel {
        id: dialogModelObj;
    }

    TreeItemModel {
        id: subFeaturesCopyModel;
    }

    function subFeaturesModelUpdated(){
        console.log("subFeaturesModelUpdated");

        editDialogContainer.buttons.setButtonState('Ok', true);
    }

    Component.onCompleted: {
        editDialogContainer.buttons.addButton({"Id": "Ok", "Name": "OK", "Enabled": true});
        editDialogContainer.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        editDialogContainer.title = qsTr("Edit");
    }

    Component.onDestruction: {
        subFeaturesCopyModel.modelChanged.disconnect(editDialogContainer.subFeaturesModelUpdated);
    }

    onFinished: {
        if (buttonId === "Ok"){
            console.log("onFinished", buttonId);
            let subFeaturesModel = editDialogContainer.featuresModel.GetData("ChildModel", editDialogContainer.index);

            let json = editDialogContainer.subFeaturesModel.toJSON();
            console.log("json", json);

            subFeaturesModel.CreateFromJson(json);

            editDialogContainer.valueId = dialogModelObj.GetData("Id");
            editDialogContainer.valueName = dialogModelObj.GetData("Name");
        }
    }

    onValueIdChanged: {
        editDialogContainer.getSubFeaturesFromModel();
    }

    function getSubFeaturesFromModel(){
        console.log("getSubFeaturesFromModel");
        let subFeaturesModel = editDialogContainer.featuresModel.GetData("ChildModel", editDialogContainer.index);
        if (!subFeaturesModel){
            subFeaturesModel = editDialogContainer.featuresModel.AddTreeModel("ChildModel", editDialogContainer.index);
        }
        else{
            let json = subFeaturesModel.toJSON();
            console.log("subFeaturesModel", subFeaturesModel.GetItemsCount());

            console.log("json", json);

            editDialogContainer.subFeaturesModel.CreateFromJson(json);

            console.log("subFeaturesModel", editDialogContainer.subFeaturesModel.toJSON());
        }

        subFeaturesCopyModel.modelChanged.connect(editDialogContainer.subFeaturesModelUpdated);
    }
}

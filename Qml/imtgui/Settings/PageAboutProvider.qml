import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0

QtObject {
    id: root;

    property ApplicationInfoProvider applicationInfoProvider: null;

    property TreeItemModel representationModel: TreeItemModel {};

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged)
    }

    function onLocalizationChanged(language){
        if (applicationInfoProvider != null){
            applicationInfoProvider.updateModel();
        }
    }

    function getRepresentationModel(){
        console.log("PageAboutProvider getRepresentationModel");
        root.representationModel.Clear();

        root.representationModel.SetData("Id", "About");
        root.representationModel.SetData("Name", qsTr("About"));

        let parameters = root.representationModel.AddTreeModel("Parameters");

        let index;

        if (root.applicationInfoProvider != null){
            if (root.applicationInfoProvider.clientApplicationInfo != null){
                index = parameters.InsertNewItem();
                parameters.CopyItemDataFromModel(index, applicationInfoProvider.clientApplicationInfo)
                parameters.SetData("Name", qsTr("Client Version"), index);
            }

            if (root.applicationInfoProvider.serverApplicationInfo != null){
                index = parameters.InsertNewItem();
                parameters.CopyItemDataFromModel(index, applicationInfoProvider.serverApplicationInfo)
                parameters.SetData("Name", qsTr("Server Version"), index);
            }
        }

        let canReadServerLog = PermissionsController.checkPermission("ShowServerLog");
        if (canReadServerLog){
            index = parameters.InsertNewItem();
            parameters.SetData("Id", "ServerLog", index);
            parameters.SetData("Name", qsTr("Server Log"), index);
            parameters.SetData("Source", "qrc:/qml/imtgui/Settings/ServerLogProvider.qml", index);
        }

        root.representationModel.Refresh();

        return root.representationModel;
    }
}

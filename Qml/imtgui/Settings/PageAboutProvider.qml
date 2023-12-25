import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0

QtObject {
    id: root;

    property TreeItemModel pageModel: TreeItemModel {};
    property ApplicationInfoProvider applicationInfoProvider: null;

//    onApplicationInfoProviderChanged: {
//        console.log("onApplicationInfoProviderChanged");
//        root.applicationInfoProvider.updated.connect(root.createRepresentationModel);
//        root.createRepresentationModel();
//    }

//    Component.onDestruction: {
//        if (root.applicationInfoProvider){
//            root.applicationInfoProvider.updated.disconnect(root.createRepresentationModel);
//        }
//    }

    property TreeItemModel representationModel: TreeItemModel {};

    property string pageId: "About";
    property string pageName: qsTr("About");

    function getRepresentationModel(){
        root.representationModel.Clear();

        root.representationModel.SetData("Id", root.pageId);
        root.representationModel.SetData("Name", root.pageName);

        let parameters = root.representationModel.AddTreeModel("Parameters");

        let index;

        if (root.applicationInfoProvider != null){
            if (root.applicationInfoProvider.clientApplicationInfo != null){
                index = parameters.InsertNewItem();

                parameters.CopyItemDataFromModel(index, applicationInfoProvider.clientApplicationInfo)
            }

            if (root.applicationInfoProvider.serverApplicationInfo != null){
                index = parameters.InsertNewItem();
                parameters.CopyItemDataFromModel(index, applicationInfoProvider.serverApplicationInfo)
            }
        }

        let canReadServerLog = PermissionsController.checkPermission("ShowServerLog");
        if (canReadServerLog){
            index = parameters.InsertNewItem();
            parameters.SetData("Id", "ServerLog", index);
            parameters.SetData("Name", qsTr("Server Log"), index);
            parameters.SetData("Source", "qrc:/qml/imtgui/Settings/ServerLogProvider.qml", index);
        }

        return root.representationModel;
    }

    function createRepresentationModel(){
        root.pageModel.Clear();

        if (root.applicationInfoProvider != null){

            root.pageModel.SetData("Id", root.pageId);
            root.pageModel.SetData("Name", root.pageName);

            let parameters = root.pageModel.AddTreeModel("Parameters");

            let index;
            if (root.applicationInfoProvider.clientApplicationInfo != null){
                index = parameters.InsertNewItem();
                parameters.CopyItemDataFromModel(index, applicationInfoProvider.clientApplicationInfo)
            }

            if (root.applicationInfoProvider.serverApplicationInfo != null){
                index = parameters.InsertNewItem();
                parameters.CopyItemDataFromModel(index, applicationInfoProvider.serverApplicationInfo)
            }

            let canReadServerLog = PermissionsController.checkPermission("ShowServerLog");
            if (canReadServerLog){
                index = parameters.InsertNewItem();
                parameters.SetData("Id", "ServerLog", index);
                parameters.SetData("Name", qsTr("Server Log"), index);
                parameters.SetData("Source", "qrc:/qml/imtgui/Settings/ServerLogProvider.qml", index);
            }
        }
    }

    function getRepresentation(){
        return pageModel;
    }
}

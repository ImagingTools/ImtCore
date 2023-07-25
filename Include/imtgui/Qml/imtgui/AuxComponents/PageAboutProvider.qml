import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: root;

    property TreeItemModel pageModel: TreeItemModel {};
    property ApplicationInfoProvider applicationInfoProvider: null;



//    Component.onCompleted: {
//        root.applicationInfoProvider.updated.connect(root.createRepresentationModel);
//    }

    onApplicationInfoProviderChanged: {
        console.log("onApplicationInfoProviderChanged");
        root.applicationInfoProvider.updated.connect(root.createRepresentationModel);
        root.createRepresentationModel();
    }

    Component.onDestruction: {
        if (root.applicationInfoProvider){
            root.applicationInfoProvider.updated.disconnect(root.createRepresentationModel);
        }
    }

    property string pageId: "About";
    property string pageName: qsTr("About");

    function createRepresentationModel(){
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

            index = parameters.InsertNewItem();
            parameters.SetData("Id", "ServerLog", index);
            parameters.SetData("Name", qsTr("Server Log"), index);
            parameters.SetData("Source", "qrc:/qml/imtgui/AuxComponents/ServerLogProvider.qml", index);
        }
    }

    function getRepresentation(){
        return pageModel;
    }
}

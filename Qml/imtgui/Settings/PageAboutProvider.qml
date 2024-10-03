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
        root.representationModel.clear();

        root.representationModel.setData("Id", "About");
        root.representationModel.setData("Name", qsTr("About"));

        let parameters = root.representationModel.addTreeModel("Parameters");

        let index;

        if (root.applicationInfoProvider != null){
            if (root.applicationInfoProvider.serverApplicationInfo != null){
                index = parameters.insertNewItem();
                parameters.setData("Name", qsTr("Server Version"), index);
                parameters.setData("Value", root.applicationInfoProvider.serverApplicationInfo.m_version, index);
                parameters.setData("Source", "qrc:/qml/imtgui/Params/TextParamView.qml", index);
            }
        }

        let canReadServerLog = PermissionsController.checkPermission("ShowServerLog");
        if (canReadServerLog){
            index = parameters.insertNewItem();
            parameters.setData("Id", "ServerLog", index);
            parameters.setData("Name", qsTr("Server Log"), index);
            parameters.setData("Source", "qrc:/qml/imtgui/Settings/ServerLogProvider.qml", index);
        }

        root.representationModel.refresh();

        return root.representationModel;
    }
}

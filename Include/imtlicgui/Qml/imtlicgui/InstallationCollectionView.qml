import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: installationCollectionContainer;
    anchors.fill: parent;
    property Item rootItem;
    property alias itemId: installationCollectionView.itemId;
    property alias model: installationCollectionView.model;

    function menuActivated(menuId) {
        installationCollectionView.menuActivated(menuId)
    }

    function commandsChanged(commandsId) {
        console.log("InstallationCollectionView commandsChanged!", commandsId);
        if (commandsId !== "Installations"){
            return;
        }
        if (installationCollectionView.selectedIndex > -1) {
            installationCollectionContainer.rootItem.setModeMenuButton("Remove", "Active");
            installationCollectionContainer.rootItem.setModeMenuButton("Edit", "Active");
            installationCollectionContainer.rootItem.setModeMenuButton("Duplicate", "Active");
            installationCollectionContainer.rootItem.setModeMenuButton("CreateLicense", "Activ");
        } else {
            installationCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            installationCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            installationCollectionContainer.rootItem.setModeMenuButton("Duplicate", "Disabled");
            installationCollectionContainer.rootItem.setModeMenuButton("CreateLicense", "Disabled");
        }
    }

    CollectionView {
        id: installationCollectionView;
        anchors.fill: parent;
        Component.onCompleted: {
            installationCollectionView.gqlModelInfo = "InstallationInfo"
            installationCollectionView.gqlModelItems = "InstallationList"
            installationCollectionView.gqlModelItems = "InstallationRemove"
        }

        onSelectItem: {
            multiDocView.addToHeadersArray(itemId, name,  "InstallationInfoEditor.qml", "InstallationEdit")
        }

        onSelectedIndexChanged: {
            if (installationCollectionView.selectedIndex > -1){
                installationCollectionContainer.commandsChanged("Installations")
            }
        }
    }

}

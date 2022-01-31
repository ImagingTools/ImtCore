import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: installationCollectionContainer;
    anchors.fill: parent;
    property Item rootItem;
    property Item multiDocViewItem;
    property alias itemId: installationCollectionView.itemId;
    property alias itemName: installationCollectionView.itemName;
    property alias model: installationCollectionView.model;

    function menuActivated(menuId) {
        installationCollectionView.menuActivated(menuId)
    }

    function refresh() {
        installationCollectionView.refresh();
    }

    function commandsChanged(commandsId) {
        console.log("InstallationCollectionView commandsChanged!", commandsId);
        if (commandsId !== "Installations"){
            return;
        }
        if (installationCollectionView.selectedIndex > -1) {
            installationCollectionContainer.rootItem.setModeMenuButton("Remove", "Normal");
            installationCollectionContainer.rootItem.setModeMenuButton("Edit", "Normal");
            installationCollectionContainer.rootItem.setModeMenuButton("Duplicate", "Normal");
            installationCollectionContainer.rootItem.setModeMenuButton("CreateLicense", "Normal");
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

            var typeOperation = "Open";
            if (itemId === "") {
                name = "New Installation";
                typeOperation = "New";
            }
            installationCollectionContainer.multiDocViewItem.activeCollectionItem = installationCollectionContainer;
            installationCollectionContainer.multiDocViewItem.addToHeadersArray(itemId, name,  "../../imtlicgui/InstallationInfoEditor.qml", "InstallationEdit", typeOperation)
//            multiDocView.addToHeadersArray(itemId, name,  "../../imtlicgui/InstallationInfoEditor.qml", "InstallationEdit")
        }

        onSelectedIndexChanged: {
            if (installationCollectionView.selectedIndex > -1){
                installationCollectionContainer.commandsChanged("Installations")
            }
        }
    }

}

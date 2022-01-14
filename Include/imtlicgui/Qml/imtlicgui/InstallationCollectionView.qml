import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: installationCollectionContainer;
    anchors.fill: parent;
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
            docsDataDeleg.setModeMenuButton("Remove", "Active");
            docsDataDeleg.setModeMenuButton("Edit", "Active");
            docsDataDeleg.setModeMenuButton("Duplicate", "Active");
            docsDataDeleg.setModeMenuButton("CreateLicense", "Activ");
        } else {
            docsDataDeleg.setModeMenuButton("Remove", "Disabled");
            docsDataDeleg.setModeMenuButton("Edit", "Disabled");
            docsDataDeleg.setModeMenuButton("Duplicate", "Disabled");
            docsDataDeleg.setModeMenuButton("CreateLicense", "Disabled");
        }
    }

    CollectionView {
        id: installationCollectionView;
        anchors.fill: parent;
        Component.onCompleted: {
            installationCollectionView.gqlModelInfo = "InstallationInfo"
            installationCollectionView.gqlModelItems = "InstallationList"
        }

        onSelectItem: {
            multiDocView.addToHeadersArray(itemId, name,  "../../imtgui/AuxComponents/InstallationInfoEditor.qml", "InstallationEdit")
        }

        onSelectedIndexChanged: {
            if (installationCollectionView.selectedIndex > -1){
                installationCollectionContainer.commandsChanged("Installations")
            }
        }
    }

}

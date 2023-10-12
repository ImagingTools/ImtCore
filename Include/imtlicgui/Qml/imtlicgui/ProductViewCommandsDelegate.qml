import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    property Item tableData: null;

    property ModelIndex selectedIndex: container.tableData != null ? container.tableData.selectedIndex: null;

    onSelectedIndexChanged: {
        console.log("CollectionViewCommands onSelectedIndexChanged");
        let isEnabled = false;

        if (container.selectedIndex != null){
            let level = container.selectedIndex.depth;
            if (level === 0){
                isEnabled = true;
            }
        }

        container.documentBase.commandsProvider.setCommandIsEnabled("Remove", isEnabled);
    }

    onCommandActivated: {
        console.log("ProductsCommands onCommandActivated", commandId);

        if (commandId === "New"){
            let features = documentBase.documentModel.GetData("Features");

            let featureIds = []
            if (features !== ""){
                featureIds = features.split(';');
            }

            modalDialogManager.openDialog(featuresDialogComp, {"featuresModel": documentBase.allFeaturesModel, "excludeFeatureIds": featureIds});
        }
        else if (commandId === "Remove"){
            let selectedIndex = tableData.selectedIndex;
            if (selectedIndex != null){
                let index = selectedIndex.index;

                let featureId = tableData.rowModel.GetData("Id", index);

                documentBase.removeFeature(featureId);

                documentBase.updateFeaturesGui();
            }
        }
    }

    property Component featuresDialogComp: Component {
        FeaturesDialog {
            onFinished: {
                if (selectedIndexes.length > 0){
                    let index = selectedIndexes[0];

                    let featureId = tableModel.GetData("Id", index);

                    documentBase.addFeature(featureId);
                    documentBase.updateFeaturesGui();
                }
            }
        }
    }
}


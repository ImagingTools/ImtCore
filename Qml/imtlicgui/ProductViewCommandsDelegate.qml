import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtdocgui 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewCommandsDelegateBase {
    id: container;

    onViewChanged: {
        if (view){
            tableConnections.target = view.tableView;
        }
    }

    Connections {
        id: tableConnections;

        function onSelectedIndexChanged(){
            let selectedIndex = container.view.tableView.selectedIndex;

            let isEnabled = false;
            if (selectedIndex != null){
                let level = selectedIndex.depth;
                if (level === 0){
                    isEnabled = true;
                }
            }

            container.view.commandsController.setCommandIsEnabled("Remove", isEnabled);
        }
    }

    onCommandActivated: {
        if (!container.view){
            return;
        }

        if (commandId === "New"){
            let features = container.view.model.m_features;

            let featureIds = []
            if (features !== ""){
                featureIds = features.split(';');
            }

            ModalDialogManager.openDialog(featuresDialogComp, {"excludeFeatureIds": featureIds, "featuresModel": container.view.allFeaturesModel});
        }
        else if (commandId === "Remove"){
            let selectedIndex = container.view.tableView.selectedIndex;
            if (selectedIndex !== null){
                let index = selectedIndex.index;

                let featureId = container.view.tableView.rowModel.getData("id", index);
                container.view.removeFeature(featureId);

                container.view.updateFeaturesGui();
            }
        }
    }

    property Component featuresDialogComp: Component {
        FeaturesDialog {
            onFinished: {
                if (selectedIndexes.length > 0){
                    container.view.productData.beginChanges();
                    for (let i = 0; i < selectedIndexes.length; i++){
                        let featureId = tableModel.getData("id", selectedIndexes[i]);
                        container.view.addFeature(featureId);
                    }
                    container.view.productData.endChanges();

                    container.view.updateFeaturesGui();
                }
            }
        }
    }
}


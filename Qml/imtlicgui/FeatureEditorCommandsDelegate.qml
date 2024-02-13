import QtQuick 2.12
import Acf 1.0
import imtauthgui 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0


DocumentWorkspaceCommandsDelegateBase {
    id: root;

    onCommandActivated: {
        let selectedIndex = null;
        if (tableView.tableSelection.items.length > 0){
            selectedIndex = tableView.tableSelection.items[0];
        }

        if (commandId === "New"){
            if (selectedIndex != null){
                tableView.addChildItem(selectedIndex, {"FeatureId":"", "FeatureName":"Feature Name", "FeatureDescription":"", "Dependencies":"", "Optional":false, "ChildModel":0})

                featureEditor.model.dataChanged(null, null);
            }
        }
        else if (commandId === "Remove"){
            if (selectedIndex != null){
                tableView.removeChildItem(selectedIndex);
                featureEditor.model.dataChanged(null, null);
            }
        }
    }
}

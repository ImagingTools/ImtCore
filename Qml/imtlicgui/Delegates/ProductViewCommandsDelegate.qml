import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewCommandsDelegateBase {
    id: container;

    onViewChanged: {
		viewConnections.target = view;
		container.updateCommandStates();
    }

	function updateCommandStates(){
		if (!container.view || !container.view.commandsController)
			return;
		container.view.commandsController.setCommandIsEnabled("Remove",
			container.view.selectedRootFeatureId() !== "");
	}

    Connections {
        id: viewConnections;

		function onSelectedNodeChanged(){
			container.updateCommandStates();
		}
    }

    onCommandActivated: {
        if (!container.view){
            return;
        }

        if (commandId === "New"){
			container.view.openFeaturesDialog();
        }
        else if (commandId === "Remove"){
			container.view.removeSelectedRootFeature();
        }
    }
}


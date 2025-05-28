import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0

DocumentCollectionViewDelegate {
	id: container;
	
	removeDialogTitle: qsTr("Deleting a role");
	removeMessage: qsTr("Delete the selected role ?");
	
	documentTypeIds: ["Role"];
	documentViewTypeIds: ["RoleEditor"];
	
	function updateStateBaseCommands(selection, commandsController, elementsModel){
		let isEnabled = selection.length > 0;
		if(commandsController){
			commandsController.setCommandIsEnabled("Remove", isEnabled);
			commandsController.setCommandIsEnabled("Edit", selection.length === 1);
			commandsController.setCommandIsEnabled("Export", selection.length === 1);
			commandsController.setCommandIsEnabled("Revision", selection.length === 1);
		}
	}
}

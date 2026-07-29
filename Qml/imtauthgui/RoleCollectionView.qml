import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtauthgui 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthRolesSdl 1.0

RemoteCollectionView {
	id: roleCollectionViewContainer;
	
	collectionId: "Roles";
	visibleMetaInfo: true;
	documentCollectionFilter: null

	additionalFieldIds: [RoleItemDataTypeMetaInfo.s_productId, RoleItemDataTypeMetaInfo.s_parentRoles]
	
	commandsDelegateComp: Component {DocCollectionViewDelegate {
			collectionView: roleCollectionViewContainer;
			documentManager: roleCollectionViewContainer.documentManager;

			removeDialogTitle: qsTr("Deleting a role");
			removeMessage: qsTr("Delete the selected role ?");

			Component.onCompleted: registerDocumentType("Role", qsTr("Role"))

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
	}
	
	property string productId;
	property var documentManager: null;
	
	function handleSubscription(dataModel){
		roleCollectionViewContainer.doUpdateGui();
	}
}

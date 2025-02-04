import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0

Item {
	id: workspaceView;

	property DocumentManager documentManager;
	property int popupWidth: 200;

	Connections {
		id: connections;
		target: workspaceView.documentManager;

		function onDocumentSaved(documentId){
			let typeId = workspaceView.documentManager.getDocumentTypeId(documentId);
			getVisualStatusInfoRequest.send({"m_objectId": documentId, "m_typeId": typeId})
		}

		function onDocumentAdded(documentId){
			let typeId = workspaceView.documentManager.getDocumentTypeId(documentId);
			getVisualStatusInfoRequest.send({"m_objectId": documentId, "m_typeId": typeId})
			let documentData = workspaceView.documentManager.getDocumentDataById(documentId);

			let name = documentData.documentName;
			if (name === ""){
				name = workspaceView.documentManager.defaultDocumentName;
			}

			let tabIndex = tabView.getIndexById(documentId);
			if (tabIndex < 0){
				tabView.addTab(documentData.documentId, "", documentData.viewComp, "", "", true);
				tabIndex = tabView.tabModel.count - 1;
			}

			tabView.currentIndex = tabIndex;
		}

		function onDocumentClosed(documentId){
			let tabIndex = tabView.getIndexById(documentId);
			tabView.removeTab(documentId);
		}

		function onDocumentIsDirtyChanged(documentId, isDirty){
			let tabIndex = tabView.getIndexById(documentId);
			if (tabIndex >= 0){
				let tabName = tabView.getTabName(documentId);
				if (tabName === ""){
					tabName = workspaceView.documentManager.defaultDocumentName;
				}

				let dirtyPrefix = "* ";

				if (isDirty){
					tabView.setTabName(documentId, dirtyPrefix + tabName);
				}
				else{
					if (tabName.startsWith(dirtyPrefix)){
						tabName = tabName.slice(dirtyPrefix.length);
						tabView.setTabName(documentId, tabName);
					}
				}
			}
		}
	}

	function addFixedView(viewComp, name, forceFocus){
		if (!forceFocus){
			forceFocus = false;
		}

		tabView.addTab(UuidGenerator.generateUUID(), name, viewComp);

		if (forceFocus){
			tabView.currentIndex = tabView.tabModel.count - 1;
		}
	}

	Rectangle {
		anchors.fill: parent;
		color: Style.backgroundColor2;
	}

	Component {
		id: popupMenuDialog;

		PopupMenuDialog {
			itemWidth: workspaceView.popupWidth;
			onFinished: {
				if (commandId === "Close"){
					if (tabView.currentIndex > 0){
						let tabId = tabView.getTabIdByIndex(tabView.currentIndex);
						workspaceView.documentManager.closeDocument(tabId);
					}
				}
				else if (commandId === "CloseAll"){
					let documentIds = workspaceView.documentManager.getOpenedDocumentIds();

					for (let i = 0; i < documentIds.length; i++){
						workspaceView.documentManager.closeDocument(documentIds[i], true);
					}
				}
			}
		}
	}

	LocalizationEvent {
		onLocalizationChanged: {
			tabContextMenuModel.fillModel();
		}
	}

	PopupMenuModel {
		id: tabContextMenuModel;
		Component.onCompleted: {
			fillModel();
		}

		function fillModel(){
			tabContextMenuModel.clear();
			tabContextMenuModel.addItem("Close", qsTr("Close document"), "", true);
			tabContextMenuModel.addItem("CloseAll", qsTr("Close all documents"), "", true);
		}
	}

	GqlSdlRequestSender {
		id: getVisualStatusInfoRequest;
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getObjectVisualStatus;
		inputObjectComp: Component {
			ObjectVisualStatusInput {
			}
		}

		sdlObjectComp: Component {
			VisualStatus {
				onFinished: {
					let name = m_text;
					if (name === ""){
						name = workspaceView.documentManager.defaultDocumentName;
					}

					tabView.setTabName(m_objectId, name);
					tabView.setTabDescription(m_objectId, m_description);
					tabView.setTabIcon(m_objectId, m_icon);
				}
			}
		}
	}

	TabView {
		id: tabView;
		anchors.fill: parent;
		closable: true;

		onTabLoaded: {
			workspaceView.documentManager.setupDocumentView(tabId, tabItem);
		}

		onTabClicked: {
			if (mouse.button === Qt.RightButton && index != 0){
				var point = tabItem.mapToItem(this, 0, 0);
				ModalDialogManager.openDialog(popupMenuDialog, {"x": point.x + workspaceView.popupWidth, "y": point.y, "model": tabContextMenuModel});
			}
		}

		function onCloseTab(index){
			let tabId = getTabIdByIndex(index);
			let documentData = workspaceView.documentManager.getDocumentDataById(tabId);
			if (documentData){
				workspaceView.documentManager.closeDocument(tabId);
			}
			else{
				removeTab(tabId);
			}
		}
	}
}

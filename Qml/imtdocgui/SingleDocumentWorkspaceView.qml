import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: root;

	color: Style.backgroundColor2;

	property string titleSeparation: "/";
	property bool initialItemTitleVisible: false;

	property DocumentManager documentManager;
	
	property ObjectVisualStatusProvider visualStatusProvider: null
	
	Connections {
		target: root.visualStatusProvider ? root.visualStatusProvider : undefined
		
		function onVisualStatusReceived(objectId, icon, text, description){
			if (text === ""){
				text = root.documentManager.defaultDocumentName
			}

			headersModel.setHeaderName(objectId, text)
		}
		
		function onVisualStatusReceiveFailed(objectId, errorMessage){
			headersModel.setHeaderName(objectId, root.documentManager.defaultDocumentName)
		}
	}
	
	Connections {
		id: connections;
		target: root.documentManager;

		function onDocumentSaved(documentId){
			Events.sendEvent("StopLoading")
			let typeId = root.documentManager.getDocumentTypeId(documentId);
			
			if (root.visualStatusProvider){
				root.visualStatusProvider.getVisualStatus(documentId, typeId)
			}
		}
		
		function onDocumentSavingStarted(documentId){
			Events.sendEvent("StartLoading")
		}
		
		function onDocumentSavingFailed(documentId, message){
			root.openErrorDialog(message)
		}

		function onDocumentAdded(documentId){
			let typeId = root.documentManager.getDocumentTypeId(documentId)
			let name = root.documentManager.getDocumentName(documentId)

			if (root.visualStatusProvider){
				name = ""
				root.visualStatusProvider.getVisualStatus(documentId, typeId)
			}
			else{
				if (name === ""){
					name = root.documentManager.defaultDocumentName
				}
			}
			
			headersModel.addHeader(documentId, name)

			let documentData = root.documentManager.getDocumentDataById(documentId)
			if (documentData){
				stackView.push(documentData.viewComp)
			}
		}

		function onDocumentClosed(documentId){
			stackView.pop();
			headersModel.removeHeader(documentId);
		}

		function onDocumentIsDirtyChanged(documentId, isDirty){
			let tabIndex = headersModel.getIndexById(documentId);;
			if (tabIndex >= 0){
				let tabName = headersModel.getHeaderName(documentId);
				if (tabName === ""){
					tabName = root.documentManager.defaultDocumentName;
				}

				let dirtySuffix = " *";

				if (isDirty){
					headersModel.setHeaderName(documentId, tabName + dirtySuffix);
				}
				else{
					if (tabName.endsWith(dirtySuffix)){
						tabName = tabName.slice(0, -dirtySuffix.length);
						headersModel.setHeaderName(documentId, tabName);
					}
				}
			}
		}
		
		function onDocumentOpened(documentId){
			Events.sendEvent("StopLoading")
		}
		
		function onDocumentOpeningStarted(documentId){
			Events.sendEvent("StartLoading")
		}
		
		function onDocumentOpeningFailed(documentId, message){
			Events.sendEvent("StopLoading")
			root.openErrorDialog(message)
		}
		
		function onTryCloseDirtyDocument(documentId, callback){
			let dialogCallback = function(result){
				if (result === Enums.yes){
					callback(true)
				}
				else if (result === Enums.no){
					callback(false)
				}
				else{
					callback(undefined)
				}
			}

			ModalDialogManager.showConfirmationDialog(
						qsTr("Save document"),
						qsTr("Save all changes ?"),
						dialogCallback)
		}
	}
	
	function openErrorDialog(message){
		ModalDialogManager.showErrorDialog(message)
	}

	function addInitialItem(viewComp, name){
		if (!documentManager){
			console.error("Unable to add the initial item to single document view. Error: Document manager is invalid");
			return;
		}

		stackView.push(viewComp);
		headersModel.addHeader(UuidGenerator.generateUUID(), name)
	}

	Item {
		id: buttonPanel;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.leftMargin: visible ? Style.marginM : 0;
		width: visible ? closeButton.width: 0;
		height: headersListView.height;
		visible: headersModel.count > 1;

		ToolButton {
			id: closeButton;
			anchors.centerIn: buttonPanel;
			width: visible ? Style.buttonWidthM: 0;
			height: width;
			iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal);
			onClicked: {
				root.documentManager.closeDocumentByIndex(root.documentManager.documentsModel.count - 1);
			}
		}
	}

	ListModel {
		id: headersModel;

		function addHeader(id, name){
			headersModel.append({id: id, name: name})
		}

		function getIndexById(id){
			for (let i = 0; i < headersModel.count; i++){
				if (headersModel.get(i).id === id){
					return i;
				}
			}

			return -1;
		}

		function removeHeader(id){
			for (let i = 0; i < headersModel.count; i++){
				if (headersModel.get(i).id === id){
					headersModel.remove(i);
					return;
				}
			}
		}

		function setHeaderName(id, name){
			for (let i = 0; i < headersModel.count; i++){
				if (headersModel.get(i).id === id){
					headersModel.setProperty(i, "name", name)
					return;
				}
			}
		}

		function getHeaderName(id){
			let index = getIndexById(id);
			if (index >= 0){
				return headersModel.get(index).name;
			}

			return "";
		}
	}

	ListView {
		id: headersListView;
		anchors.top: parent.top;
		anchors.left: buttonPanel.right;
		anchors.leftMargin: Style.marginM;
		anchors.right: parent.right;
		height: visible ? Style.controlHeightL : 0;
		orientation: ListView.Horizontal;
		boundsBehavior: Flickable.StopAtBounds;
		visible: count === 1 && !root.initialItemTitleVisible ? false : true;
		model: headersModel;
		delegate: Item {
			width: visible ? content.width + spacer.width: 0;
			height: headersListView.height;
			visible: model.index === 0 ? root.initialItemTitleVisible : true;

			Item {
				id: spacer;
				anchors.left: parent.left;
				width: visible ? Style.marginM : 0;
				visible: model.index === 1 && !root.initialItemTitleVisible ? false : true;
			}

			Row {
				id: content;
				anchors.left: spacer.right;
				height: parent.height;
				spacing: Style.marginM;

				Text {
					id: separator;
					anchors.verticalCenter: content.verticalCenter;
					font.pixelSize: Style.fontSizeXXL;
					font.family: Style.fontFamily;
					color: Style.titleColor;
					text: root.titleSeparation;
					visible: model.index === 1 && !root.initialItemTitleVisible ? false : model.index > 0;
				}

				Text {
					id: headerText;
					anchors.verticalCenter: content.verticalCenter;
					font.pixelSize: Style.fontSizeXXL;
					font.family: Style.fontFamily;
					color: Style.titleColor;
					text: model.name;
				}
			}

			MouseArea {
				anchors.fill: parent;
				onClicked: {}
			}
		}
	}

	StackView {
		id: stackView;
		z: 10;
		anchors.top: headersListView.bottom;
		anchors.bottom: parent.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;

		onItemAdded: {
			let documentData = root.documentManager.getDocumentData(root.documentManager.documentsModel.count - 1);
			if (documentData){
				root.documentManager.setupDocumentView(documentData.documentId, item);
			}
		}
	}
}



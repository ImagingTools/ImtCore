import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: root;

	color: Style.backgroundColor2;

	property string titleSeparation: "/";
	property bool initialItemTitleVisible: false;

	property DocumentManager documentManager;
	
	Connections {
		id: connections;
		target: root.documentManager;

		function onDocumentSaved(documentId){
			let typeId = root.documentManager.getDocumentTypeId(documentId);
			
			let name = root.getViewName(documentId, typeId)
			root.setViewName(documentId, name);
		}

		function onDocumentAdded(documentId){
			let typeId = root.documentManager.getDocumentTypeId(documentId);
			
			let name = root.getViewName(documentId, typeId)
			if (name == ""){
				name = root.documentManager.defaultDocumentName
			}

			headersModel.addHeader(documentId, name)

			let documentData = root.documentManager.getDocumentDataById(documentId);
			if (documentData){
				stackView.push(documentData.viewComp);
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
	}

	function addInitialItem(viewComp, name){
		if (!documentManager){
			console.error("Unable to add the initial item to single document view. Error: Document manager is invalid");
			return;
		}

		stackView.push(viewComp);
		headersModel.addHeader(UuidGenerator.generateUUID(), name)
	}
	
	function setViewName(id, name){
		headersModel.setHeaderName(id, name)
	}
	
	function getViewName(id, typeId){
		return ""
	}

	Item {
		id: buttonPanel;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.leftMargin: visible ? Style.sizeMainMargin : 0;
		width: visible ? closeButton.width: 0;
		height: headersListView.height;
		visible: headersModel.count > 1;

		ToolButton {
			id: closeButton;
			anchors.centerIn: buttonPanel;
			width: visible ? 25: 0;
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
			console.log("addHeader", id, name)
			headersModel.append({Id: id, Name: name})
		}

		function getIndexById(id){
			for (let i = 0; i < headersModel.count; i++){
				if (headersModel.get(i).Id === id){
					return i;
				}
			}

			return -1;
		}

		function removeHeader(id){
			for (let i = 0; i < headersModel.count; i++){
				if (headersModel.get(i).Id === id){
					headersModel.remove(i);
					return;
				}
			}
		}

		function setHeaderName(id, name){
			console.log("setHeaderName", id, name)
			
			for (let i = 0; i < headersModel.count; i++){
				if (headersModel.get(i).Id === id){
					headersModel.setProperty(i, "Name", name)
					return;
				}
			}
		}

		function getHeaderName(id){
			let index = getIndexById(id);
			if (index >= 0){
				return headersModel.get(index).Name;
			}

			return "";
		}
	}
	


	ListView {
		id: headersListView;
		anchors.top: parent.top;
		anchors.left: buttonPanel.right;
		anchors.leftMargin: Style.sizeMainMargin;
		anchors.right: parent.right;
		height: visible ? 40 : 0;
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
				width: visible ? Style.sizeMainMargin : 0;
				visible: model.index === 1 && !root.initialItemTitleVisible ? false : true;
			}

			Row {
				id: content;
				anchors.left: spacer.right;
				height: parent.height;
				spacing: Style.sizeMainMargin;

				Text {
					id: separator;
					anchors.verticalCenter: content.verticalCenter;
					font.pixelSize: Style.fontSizeXXLarge;
					font.family: Style.fontFamily;
					color: Style.titleColor;
					text: root.titleSeparation;
					visible: model.index === 1 && !root.initialItemTitleVisible ? false : model.index > 0;
				}

				Text {
					id: headerText;
					anchors.verticalCenter: content.verticalCenter;
					font.pixelSize: Style.fontSizeXXLarge;
					font.family: Style.fontFamily;
					color: Style.titleColor;
					text: model.Name;
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



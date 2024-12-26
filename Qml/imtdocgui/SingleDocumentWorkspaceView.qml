import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0

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
			getVisualStatusInfoRequest.send({"m_objectId": documentId, "m_typeId": typeId})
		}

		function onDocumentAdded(documentId){
			let typeId = root.documentManager.getDocumentTypeId(documentId);
			getVisualStatusInfoRequest.send({"m_objectId": documentId, "m_typeId": typeId})

			let documentData = root.documentManager.getDocumentDataById(documentId);
			if (documentData){
				stackView.push(documentData.viewComp);
				let name = documentData.documentName;
				if (name === ""){
					name = root.documentManager.defaultDocumentName;
				}

				headersModel.addHeader(documentId, name);
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

	Item {
		id: buttonPanel;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.leftMargin: visible ? Style.size_mainMargin : 0;
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
						name = root.documentManager.defaultDocumentName;
					}

					headersModel.setHeaderName(m_objectId, name)
				}
			}
		}

		function onError(message, type){

		}
	}

	ListView {
		id: headersListView;
		anchors.top: parent.top;
		anchors.left: buttonPanel.right;
		anchors.leftMargin: Style.size_mainMargin;
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
				width: visible ? Style.size_mainMargin : 0;
				visible: model.index === 1 && !root.initialItemTitleVisible ? false : true;
			}

			Row {
				id: content;
				anchors.left: spacer.right;
				height: parent.height;
				spacing: Style.size_mainMargin;

				Text {
					id: separator;
					anchors.verticalCenter: content.verticalCenter;
					font.pixelSize: Style.fontSize_title;
					font.family: Style.fontFamily;
					color: Style.titleColor;
					text: root.titleSeparation;
					visible: model.index === 1 && !root.initialItemTitleVisible ? false : model.index > 0;
				}

				Text {
					id: headerText;
					anchors.verticalCenter: content.verticalCenter;
					font.pixelSize: Style.fontSize_title;
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
				documentData.view = item;
			}
		}
	}
}



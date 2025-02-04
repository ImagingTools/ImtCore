import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

// GuiComponentBase
ViewBase {
	id: root;

	property string documentId;
	property string documentTypeId;
	property string viewTypeId;
	property DocumentManager documentManager;
	property Component viewComp: null;
	property ViewBase view: contentLoader.item;
	property alias historyVisible: historyPanel.visible;

	signal viewRegistered();

	onModelChanged: {
		if (view){
			view.model = model;
		}
	}

	onViewRegistered: {
		console.debug("onViewRegistered", documentId, documentTypeId, viewTypeId, root);
	}

	function updateGui(){
		if (view){
			view.doUpdateGui();
		}
	}

	function updateModel(){
		if (view){
			view.doUpdateModel();
		}
	}

	DocumentHistoryPanel {
		id: historyPanel;
		documentId: root.documentId;
		editorFlickable: flickable;
		objectName: "DocumentView";
	}

	CustomScrollbar {
		id: scrollbar;
		z: parent.z + 1;
		anchors.right: parent.right;
		anchors.top: flickable.top;
		anchors.bottom: flickable.bottom;
		secondSize: 10;
		targetItem: flickable;
		radius: 2;
		objectName: "DocumentView";
	}

	CustomScrollbar{
		id: scrollHoriz;
		z: parent.z + 1;
		anchors.left: flickable.left;
		anchors.right: flickable.right;
		anchors.bottom: flickable.bottom;
		secondSize: 10;
		vertical: false;
		targetItem: flickable;
		objectName: "DocumentView";
	}

	Flickable {
		id: flickable;
		anchors.left: parent.left;
		anchors.leftMargin: Style.size_largeMargin;
		anchors.top: parent.top;
		anchors.topMargin: Style.size_largeMargin;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.size_largeMargin;
		anchors.right: scrollbar.left;
		anchors.rightMargin: Style.size_largeMargin;
		contentWidth: contentLoader.width;
		contentHeight: Math.max(contentLoader.height + 2 * Style.size_largeMargin + 100, historyPanel.contentHeight + 2 * Style.size_largeMargin);
		boundsBehavior: Flickable.StopAtBounds;

		clip: true;
		objectName: "DocumentView";

		Loader {
			id: contentLoader;
			sourceComponent: root.viewComp;
			onLoaded: {
				console.debug("View for document successfully created");
				item.model = root.model;
			}
		}
	}
}

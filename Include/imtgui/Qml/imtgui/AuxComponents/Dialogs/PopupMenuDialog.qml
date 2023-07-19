import QtQuick 2.12
import Qt5Compat.GraphicalEffects 1.0
import QtGraphicalEffects 1.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
	id: popupMenuContainer;

	width: itemWidth;
	height: popupMenuListView.height;

	property var model;
	property Item root: null;
	property Item rootItem: null;

	property int itemWidth: 200;
	property int itemHeight: 26;
	property int shownItemsCount: 5;

	property int textSize: Style.fontSize_common;
	property string fontColor: Style.textColor;

	property bool hiddenBackground: true;
	property bool visibleScrollBar: true;
	property bool moveToEnd: false;
	property int moveToIndex: -1;

	property alias contentY: popupMenuListView.contentY;

	property bool forceFocus: false;
    property bool closingByFinished: true;

	// ID for display in combo box delegates
	property string nameId: "Name";

	property Component delegate: PopupMenuDelegate{
		width: popupMenuContainer.width;
		height: popupMenuContainer.itemHeight;
		textSize: popupMenuContainer.textSize;
		fontColor: popupMenuContainer.fontColor;

        onClicked: {
//            if (popupMenuContainer.closingByFinished){
//                console.log("PopupMenuDialog.qml closeDialog");
//                popupMenuContainer.root.closeDialog();
//            }
            console.log("popupMenuContainer onClicked");
            popupMenuContainer.finished(commandId, index)
        }

		rootItem: popupMenuContainer;
	}

	/**
		Count of the visible item, if value = -1 then count unlimited
	*/
	property int countVisibleItem: -1;

	signal finished(string commandId, int index);
	signal started();

	Component.onDestruction: {
		Events.unSubscribeEvent("DialogBackgroundClicked", popupMenuContainer.onBackgroundClicked)
	}

	onFinished: {
        console.log("popupMenuContainer onFinished");

        popupMenuContainer.root.closeDialog();
	}

	onModelChanged: {
		popupMenuListView.model = popupMenuContainer.model;

		if(popupMenuContainer.moveToEnd){
			popupMenuListView.positionViewAtEnd();
		}
		if(popupMenuContainer.moveToIndex >= 0 && popupMenuContainer.moveToIndex < popupMenuListView.count){
			popupMenuListView.positionViewAtIndex(popupMenuContainer.moveToIndex, ListView.Beginning);
		}
	}

	onRootChanged: {
		console.log("PopupMenu onRootChanged", root)
		/**
			Opacity of the background = 0
		*/
		if(popupMenuContainer.hiddenBackground){
			popupMenuContainer.root.backgroundItem.opacity = 0;
		}

		/**
			Close the dialog by clicking on the background
		*/
		Events.subscribeEvent("DialogBackgroundClicked", popupMenuContainer.onBackgroundClicked)
	}

	function onBackgroundClicked(){
		popupMenuContainer.root.closeDialog();
//		if (rootItem){
//			popupMenuContainer.finished('', rootItem.currentIndex);
//		}
	}

	onContentYChanged: {
		popupMenuContainer.rootItem.contentY = popupMenuContainer.contentY;
	}

	Rectangle {
		id: itemBody;

		width: popupMenuContainer.width;
		height: popupMenuListView.height;

		color: Style.baseColor;

		border.width: 1;
		border.color: Style.alternateBaseColor;

		CustomScrollbar {
			id: scrollbar;

			z: 100;

			anchors.right: popupMenuListView.right;
			anchors.bottom: popupMenuListView.bottom;

			backgroundColor: Style.baseColor;

			secondSize: 10;
			targetItem: popupMenuListView;
//            visible: !popupMenuContainer.visibleScrollBar ? false : hideNotUsed ? !notUsed : true;

		}

		ListView {
			id: popupMenuListView;

			width: popupMenuContainer.width;
			//            height: (parent.countVisibleItem == -1 || parent.countVisibleItem > popupMenuListView.count) ?
			//                        popupMenuListView.count * popupMenuContainer.itemHeight :
			//                        parent.countVisibleItem * popupMenuContainer.itemHeight;
			height: Math.min(popupMenuContainer.shownItemsCount * popupMenuContainer.itemHeight, contentHeight);

			boundsBehavior: Flickable.StopAtBounds;
			clip: true;

			delegate: popupMenuContainer.delegate;
		}//ListView
	}//ItemListView

	DropShadow {
		id: dropShadow;

		anchors.fill: itemBody;

		horizontalOffset: 2;
		verticalOffset: 2;

		radius: 4;
		color: Style.shadowColor;

		source: itemBody;
	}
}

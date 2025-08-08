import QtQuick
import Qt5Compat.GraphicalEffects
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase {
	id: root;

	height: bottomContentLoader.y + bottomContentLoader.height;

	property int itemHeight: !baseElement ? 0 : baseElement.itemHeight;
	property int itemWidth: !baseElement ? 0 : baseElement.itemWidth;
	property int shownItemsCount: !baseElement ? 0 : baseElement.shownItemsCount;
	property var delegate: !baseElement ? null : baseElement.delegate;
	property alias contentY: popupMenuListView.contentY;
	property bool moveToEnd: !baseElement ? false : baseElement.moveToEnd;
	property int moveToIndex: baseElement && popupMenuListView.count > 0 ? baseElement.moveToIndex : -1
	property var model: !baseElement ? -1 : baseElement.model;

	property int selectedIndex: !baseElement ? -1 : baseElement.selectedIndex;

	property alias topContentLoaderSourceComp: topContentLoader.sourceComponent;
	property alias bottomContentLoaderSourceComp: bottomContentLoader.sourceComponent;
	property alias repeater: popupMenuListView;

	property bool isUpwards: !baseElement ? false: baseElement.isUpwards;

	onModelChanged: {
		popupMenuListView.model = root.model;

		let hasItems = root.model && root.model.count > 0;
		root.setNoDataRecVisible(!hasItems)
		if(!hasItems){
			return
		}

		if(root.moveToEnd){
			popupMenuListView.positionViewAtEnd();
		}
		if(root.moveToIndex >= 0 && root.moveToIndex < popupMenuListView.count){
			popupMenuListView.positionViewAtIndex(root.moveToIndex, ListView.Beginning);
		}
	}

	onMoveToIndexChanged: {
		if(root.moveToIndex >= 0 && root.moveToIndex < popupMenuListView.count){
			popupMenuListView.positionViewAtIndex(root.moveToIndex, ListView.Beginning);
		}
	}


	function setNoDataRecVisible(visible_){
		itemBody.visible = !visible_
		if(visible_){
			// popupMenuListView.height = 0
			root.bottomContentLoaderSourceComp = noDataRecComp
		}
		else{
			root.bottomContentLoaderSourceComp = null;
		}
	}

	function setLoadingSplashRecVisible(visible_){
		loadingSplashRec.visible = visible_;
	}

	Component{
		id: noDataRecComp

		Rectangle{
			id: noDataRec;

			width: root ? root.width : 0;
			height: 50;
			radius: itemBody.radius;
			color: itemBody.color;

			Text{
				id: noDataText;

				anchors.centerIn: parent;

				font.pixelSize: !root.baseElement ? Style.fontSizeM : root.baseElement.textSize;
				color: !root.baseElement ? Style.textColor : root.baseElement.fontColor;

				text: qsTr("No data");
			}

			DropShadow {
				id: dropShadow;

				anchors.fill: parent;
				z: parent.z - 1

				horizontalOffset: 3;
				verticalOffset: root.isUpwards ? -3 : 3;

				radius: Style.radiusL;
				spread: 0; // unset
				color: Style.shadowColor;
				source: parent;

				Component.onCompleted: {
					if (Qt.platform.os === "web"){
						dropShadow.samples = 17;
					}
				}
			}
		}
	}

	Loader {
		id: topContentLoader;
	}

	Rectangle {
		id: itemBody;

		anchors.top: topContentLoader.bottom;

		width: root ? root.width : 0;
		height: visible ? popupMenuListView.height + 2 * Style.marginXS : 0

		color: Style.baseColor;

		visible: popupMenuListView.count > 0;

		radius: Style.buttonRadius;

		Rectangle{
			id: loadingSplashRec;

			anchors.fill: parent;
			opacity: 0.5;
			color: "transparent";
			visible: false;

			Text {
				anchors.centerIn: parent;

				color: Style.textColor;
				font.pixelSize: Style.fontSizeM;
				font.family: Style.fontFamily;

				text: qsTr("Loading") + "..."
			}
		}

		CustomScrollbar {
			id: scrollbar;

			z: 100;

			anchors.right: popupMenuListView.right;
			anchors.bottom: popupMenuListView.bottom;

			secondSize: !root.baseElement ? 0 :
											!root.baseElement.visibleScrollBar ? 0 : Style.isMobile === undefined ? 8 : Style.isMobile ? 4 : 8;
			targetItem: popupMenuListView;
			canFade: Style.isMobile === undefined ? false : Style.isMobile;
		}

		ListView {
			id: popupMenuListView;

			anchors.verticalCenter: parent.verticalCenter;

			width: root ? root.width : 0;

			boundsBehavior: Flickable.StopAtBounds;
			clip: true;

			onContentHeightChanged: {
				// console.log("onContentHeightChanged",  popupMenuListView.count);
				let height_ = Math.min(root.shownItemsCount * root.itemHeight, contentHeight);
				popupMenuListView.height = height_;
			}

			onContentYChanged: {
				if(contentHeight - contentY - popupMenuListView.height == 0){
					root.baseElement.requestNextBatch();
				}
			}

			delegate: root.delegate;
		}

		MouseArea{
			anchors.fill: parent;
			hoverEnabled: true;
			visible: root.baseElement ? root.baseElement.hoverBlocked : true;

			onPositionChanged: {
				if(root.baseElement){
					root.baseElement.hoverBlocked = false;
				}
			}
		}
	}//ItemListView

	Loader {
		id: bottomContentLoader;

		anchors.top: itemBody.bottom;
	}

	DropShadow {
		id: dropShadow;

		anchors.fill: itemBody;
		z: itemBody.z - 1

		horizontalOffset: 3; // 2
		verticalOffset: root.isUpwards ? -3 : 3; // 2

		radius: Style.radiusL; // Style.radiusM
		spread: 0; // unset
		color: Style.shadowColor;
		source: itemBody;

		Component.onCompleted: {
			if (Qt.platform.os === "web"){
				dropShadow.samples = 17;
			}
		}
	}

	function contentYCorrection(down_){
		let contentY	 = popupMenuListView.contentY;
		let itemHeight	 = root.itemHeight;
		let visibleCount = root.shownItemsCount;
		let index		 = root.selectedIndex;

		if(root.baseElement && root.selectedIndex >= 0){
			if(down_){
				let nextElementIndex = index + 1
				if(nextElementIndex * itemHeight > contentY + visibleCount * itemHeight){
					popupMenuListView.contentY = nextElementIndex * itemHeight - visibleCount * itemHeight
				}
			}
			else {
				if(index * itemHeight < contentY){
					popupMenuListView.contentY = index * itemHeight
				}
			}
		}
	}
}

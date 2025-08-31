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
	property int selectedIndex: !baseElement ? -1 : baseElement.selectedIndex;
	property int shownItemsCount: !baseElement ? 0 : baseElement.shownItemsCount;
	property int moveToIndex: baseElement && popupMenuListView.count > 0 ? baseElement.moveToIndex : -1
	property int radius: !baseElement ? Style.buttonRadius : baseElement.radius;

	property bool moveToEnd: !baseElement ? false : baseElement.moveToEnd;
	property bool isUpwards: !baseElement ? false: baseElement.isUpwards;

	property var model: !baseElement ? 0 : baseElement.model;
	property var delegate: !baseElement ? null : baseElement.delegate;

	property alias contentY: popupMenuListView.contentY;
	property alias topContentLoaderSourceComp: topContentLoader.sourceComponent;
	property alias bottomContentLoaderSourceComp: bottomContentLoader.sourceComponent;
	property alias noDataContentLoaderSourceComp: noDataContentLoader.sourceComponent;
	property alias repeater: popupMenuListView;
	property alias scrollbar: scrollbar;

	onModelChanged: {
		popupMenuListView.model = root.model;

		let hasItems = popupMenuListView.count > 0;
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
			root.noDataContentLoaderSourceComp = noDataRecComp
		}
		else{
			root.noDataContentLoaderSourceComp = null;
		}
	}

	function setLoadingSplashRecVisible(visible_){
		loadingSplashRec.visible = visible_;
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

	Component{
		id: dropShadow;

		DropShadow {
			anchors.fill: parent;
			horizontalOffset: 3;
			verticalOffset: root.isUpwards ? -3 : 3;

			radius: Style.radiusL;
			spread: 0;
			color: Style.shadowColor;

			Component.onCompleted: {
				if (Qt.platform.os === "web"){
					samples = 17;
				}
			}
		}
	}

	Component{
		id: noDataRecComp

		Rectangle{
			id: noDataRecContainer

			width: root ? root.width : 0;
			height: Style.size_indicatorHeight;
			radius: itemBody.radius;
			color: itemBody.color;

			Loader {
				id: shadowLoaderNoData;

				sourceComponent: dropShadow
				anchors.fill: noDataRec

				onLoaded: {
					if(item){
						item.source = noDataRec
					}
				}
			}

			Rectangle{
				id: noDataRec;

				anchors.fill: parent
				radius: parent.radius;
				color: parent.color;

				Text{
					id: noDataText;

					anchors.centerIn: parent;

					font.pixelSize: !root.baseElement ? Style.fontSizeM : root.baseElement.textSize;
					color: !root.baseElement ? Style.textColor : root.baseElement.fontColor;

					text: qsTr("No data");
				}
			}
		}
	}

	Loader {
		id: topContentLoader;
		visible: !root.isUpwards
	}

	Loader {
		id: shadowLoaderBody;

		sourceComponent: dropShadow
		anchors.fill: itemBody

		onLoaded: {
			if(item){
				item.source = itemBody
			}
		}
	}

	Rectangle {
		id: itemBody;

		anchors.top: topContentLoader.bottom;

		width: root ? root.width : 0;
		height: visible ? popupMenuListView.height + 2 * Style.marginXS : 0

		color: Style.baseColor;
		visible: popupMenuListView.count > 0;

		radius: root.radius;
		clip: true;

		Rectangle{
			id: loadingSplashRec;

			anchors.fill: parent;
			opacity: 0.5;
			color: itemBody.color;
			visible: false;
			z: popupMenuListView.z + 1

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
			anchors.rightMargin: parent.radius/2

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
	} // ItemListView

	Loader {
		id: bottomContentLoader;
		anchors.top: itemBody.bottom;
		visible: root.isUpwards
	}

	Loader{
		id: noDataContentLoader;
		anchors.top: bottomContentLoader.bottom;
	}
}

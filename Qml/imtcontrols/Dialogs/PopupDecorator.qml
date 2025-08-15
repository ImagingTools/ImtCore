import QtQuick
import Qt5Compat.GraphicalEffects
import QtGraphicalEffects 1.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase {
	id: root;

	height: bottomContentLoader.y + bottomContentLoader.height;
	property int radius: !baseElement ? Style.buttonRadius : baseElement.radius;
	
	property int itemHeight: !baseElement ? 0 : baseElement.itemHeight;
	property int itemWidth: !baseElement ? 0 : baseElement.itemWidth;
	property int shownItemsCount: !baseElement ? 0 : baseElement.shownItemsCount;
	property var delegate: !baseElement ? null : baseElement.delegate;
	property alias contentY: popupMenuListView.contentY;
	property bool moveToEnd: !baseElement ? false : baseElement.moveToEnd;
	property int moveToIndex: baseElement && popupMenuListView.count > 0 ? baseElement.moveToIndex : -1
	property var model: !baseElement ? 0 : baseElement.model;

	property int selectedIndex: !baseElement ? -1 : baseElement.selectedIndex;

	property alias topContentLoaderSourceComp: topContentLoader.sourceComponent;
	property alias bottomContentLoaderSourceComp: bottomContentLoader.sourceComponent;
	property alias repeater: popupMenuListView;

	property bool isUpwards: !baseElement ? false: baseElement.isUpwards;

	onModelChanged: {
		popupMenuListView.model = root.model;

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

	Loader {
		id: topContentLoader;
	}

	Rectangle {
		id: itemBody;

		anchors.top: topContentLoader.bottom;

		width: root ? root.width : 0;
		height: visible ? popupMenuListView.height + 2 * Style.marginXS : false;

		color: Style.baseColor;

		visible: popupMenuListView.count > 0;

		radius: root.radius;
		clip: true;

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
				// console.log("onContentHeightChanged", root, popupMenuListView);
				let height_ = Math.min(root.shownItemsCount * root.itemHeight, contentHeight);
				popupMenuListView.height = height_;
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
		z: itemBody.z-1
		horizontalOffset: 3;
		verticalOffset: root.isUpwards ? -3 : 3;
		radius: Style.radiusL;
		spread: 0;
		color: Style.shadowColor;
		source: itemBody;

		Component.onCompleted: {
			if (Qt.platform.os === "web"){
				dropShadow.samples = 17;
			}
		}
	}

	function contentYCorrection(down_){
		var contentY = popupMenuListView.contentY;
		var itemHeight = root.itemHeight;
		var visibleCount = root.shownItemsCount;
		var index = root.selectedIndex;

		if(root.baseElement){
			if(root.selectedIndex >= 0){

				if(down_){
					if((index+1) * itemHeight > contentY + visibleCount * itemHeight){
						popupMenuListView.contentY = (index+1) * itemHeight - visibleCount * itemHeight
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
}

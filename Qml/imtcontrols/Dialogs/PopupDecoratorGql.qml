import QtQuick
import Qt5Compat.GraphicalEffects
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase {
	id: root;

	height: column.height;

	property int itemHeight: !baseElement ? 0 : baseElement.itemHeight;
	property int itemWidth: !baseElement ? 0 : baseElement.itemWidth;
	property int selectedIndex: !baseElement ? -1 : baseElement.selectedIndex;
	property int shownItemsCount: !baseElement ? 0 : baseElement.shownItemsCount;
	property int moveToIndex: baseElement && popupMenuListView.count > 0 ? baseElement.moveToIndex : -1
	property int radius: !baseElement ? Style.buttonRadius : baseElement.radius;

	property bool moveToEnd: !baseElement ? false : baseElement.moveToEnd;
	property bool isUpwards: !baseElement ? false: baseElement.isUpwards;
	property bool modelLoadingState: dataProviderState.toLowerCase() === "loading"

	property string dataProviderState: !baseElement ? "ready": baseElement.dataProviderState;

	property var model: !baseElement ? 0 : baseElement.model;
	property var delegate: !baseElement ? null : baseElement.delegate;

	property alias contentY: popupMenuListView.contentY;
	property alias topContentLoaderSourceComp: topContentLoader.sourceComponent;
	property alias bottomContentLoaderSourceComp: bottomContentLoader.sourceComponent;
	property alias repeater: popupMenuListView;
	property alias scrollbar: scrollbar;

	onMoveToIndexChanged: {
		if(root.moveToIndex >= 0 && root.moveToIndex < popupMenuListView.count){
			popupMenuListView.positionViewAtIndex(root.moveToIndex, ListView.Beginning);
		}
	}

	onDataProviderStateChanged: {
		if(model){
			setLoadingSplashRecVisible(modelLoadingState)
		}
		else{
			setModelStateRecVisible(modelLoadingState)
		}
	}

	onModelChanged: {
		popupMenuListView.model = root.model;

		let hasItems = popupMenuListView.count > 0;
		root.setModelStateRecVisible(!hasItems)

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

	onIsUpwardsChanged: {
		// swap search/no-data loader content
		let top = topContentLoader.sourceComponent
		let bot = bottomContentLoader.sourceComponent

		if (top && bot){
			topContentLoader.sourceComponent = bot
			bottomContentLoader.sourceComponent = top
		}
		else if (top){
			bottomContentLoader.sourceComponent = top
			topContentLoader.sourceComponent = null
		}
		else if (bot){
			topContentLoader.sourceComponent = bot
			bottomContentLoader.sourceComponent = null
		}
	}

	function setLoadingSplashRecVisible(visible_){
		loadingSplashRec.visible = visible_;
	}

	function setModelStateRecVisible(visible_){
		itemBodyContainer.visible = !visible_
		let comp = visible_ ? modelStateRecComp : null

		if (isUpwards){
			topContentLoaderSourceComp = comp
		}
		else{
			bottomContentLoaderSourceComp = comp
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
			else{
				if(index * itemHeight < contentY){
					popupMenuListView.contentY = index * itemHeight
				}
			}
		}
	}

	Column {
		id: column
		width: root ? root.width : 0;
		spacing: 0

		Loader {
			id: topContentLoader;
			width: root.width
			visible: sourceComponent !== null
		}

		Item {
			id: itemBodyContainer
			width: parent.width;
			height: itemBody.height

			Loader {
				id: shadowLoaderBody;

				anchors.fill: itemBody
				sourceComponent: dropShadow

				onLoaded: {
					if(item){
						item.source = itemBody
					}
				}
			}

			Rectangle {
				id: itemBody;

				width: parent.width;
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
						id: loadingText

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

					anchors.centerIn: parent
					width: parent.width;

					boundsBehavior: Flickable.StopAtBounds;
					clip: true;

					onContentHeightChanged: {
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
		}

		Loader {
			id: bottomContentLoader;
			width: root.width
			visible: sourceComponent !== null
		}
	}

	/// components

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
		id: modelStateRecComp

		Rectangle{
			id: modelStateRecContainer

			width: root ? root.width : 0;
			height: Style.size_indicatorHeight;
			radius: itemBody.radius;
			color: itemBody.color;

			Loader{
				id: shadowLoaderNoData;

				sourceComponent: dropShadow
				anchors.fill: modelStateRec

				onLoaded: {
					if(item){
						item.source = modelStateRec
					}
				}
			}

			Rectangle{
				id: modelStateRec;

				anchors.fill: parent
				radius: parent.radius;
				color: parent.color;

				Text{
					id: noDataText;

					anchors.centerIn: parent;

					font.pixelSize: !root.baseElement ? Style.fontSizeM : root.baseElement.textSize;
					color: !root.baseElement ? Style.textColor : root.baseElement.fontColor;

					text: root.modelLoadingState ? loadingText.text : qsTr("No data");
				}
			}
		}
	}
}

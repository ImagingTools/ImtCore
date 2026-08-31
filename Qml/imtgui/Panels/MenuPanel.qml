import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Rectangle {
	id: menuPanel;

	objectName: "MenuPanel"

	clip: false;

	color: Style.baseColor;
	radius: 0;

	property string textColor: Style.textColor;
	property string fontName: Style.fontFamily;
	property string activePageId;
	property string activePageName;
	property string activeIcon;
	property string firstElementImageSources: "";

	property int activePageIndex: -1;

	property TreeItemModel model: TreeItemModel {};

	// Model for pages with top alignment
	property TreeItemModel topPagesModel: TreeItemModel {};

	// Model for pages with bottom alignment
	property TreeItemModel bottomPagesModel: TreeItemModel {};

	property int spacing: 0;
	signal activePageChanged;

	property int buttonWidth: -1;
	property int buttonHeight: -1;

	property bool centered: Style.menuPanelCentered !== undefined ? Style.menuPanelCentered : false;
	property bool collapsed: false
	property real menuDefaultWidth: 0

	property int iconSize: Style.menuPanelIconSize;
	property int rowHeight: Style.controlHeightL;
	property int expandedWidth: Style.menuPanelWidth !== undefined ? Style.menuPanelWidth : Style.sizeHintXXS;
	property int collapsedWidth: Style.menuPanelMinWidth;

	property int autoCollapseWidth: Style.menuPanelAutoCollapseWidth !== undefined ? Style.menuPanelAutoCollapseWidth : 0;
	property bool autoCollapsed: false;

	readonly property int rowWidth: Style.enableMenuPanelCollapse ? menuPanel.expandedWidth : menuPanel.buttonWidth;

	property string hintText: "";
	property real hintY: 0;

	width: Style.enableMenuPanelCollapse ? menuPanel.collapsedWidth : menuPanel.rowWidth;

	Component.onCompleted: {
		if (Style.enableMenuPanelCollapse){
			menuPanel.width = menuPanel.collapsed ? menuPanel.collapsedWidth : menuPanel.expandedWidth;
			menuPanel.menuDefaultWidth = menuPanel.expandedWidth;
		}

		Events.subscribeEvent("MenuModelRequest", menuPanel.onMenuModelRequest);
		Events.subscribeEvent("UpdatePageVisualStatus", menuPanel.updateVisualStatus);
		Events.subscribeEvent("ChangePage", menuPanel.setActivePage);
		Events.subscribeEvent("CollapseMenu", menuPanel.setCollapsed);
		Events.subscribeEvent("ExpandMenu", menuPanel.setCollapsed);

		menuPanel.updateAutoCollapse();
	}

	Connections {
		target: menuPanel.parent;

		function onWidthChanged(){
			menuPanel.updateAutoCollapse();
		}
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("MenuModelRequest", menuPanel.onMenuModelRequest);
		Events.unSubscribeEvent("UpdatePageVisualStatus", menuPanel.updateVisualStatus);
		Events.unSubscribeEvent("ChangePage", menuPanel.setActivePage);
		Events.unSubscribeEvent("CollapseMenu", menuPanel.setCollapsed);
		Events.unSubscribeEvent("ExpandMenu", menuPanel.setCollapsed);

	}

	onActivePageIdChanged: {
		if(activePageId !== ""){
			Events.sendEvent("ActivePageIdChanged", activePageId);

			NavigationController.push(activePageId)
		}
	}

	onWidthChanged: {
		if (!widthAnimation.running){
			Events.sendEvent("MenuWidthChanged", width)
		}
	}

	Keys.onPressed: {
		if (event.key === Qt.Key_Up){
			if (menuPanel.activePageIndex >= 1){
				menuPanel.activePageIndex--;
			}
		}

		if (event.key === Qt.Key_Down){
			if (menuPanel.activePageIndex < model.getItemsCount() - 1){
				menuPanel.activePageIndex++;
			}
		}
	}

	function onMenuModelRequest(ok){
		Events.sendEvent("MenuModelChanged", menuPanel.model);
	}

	function updateVisualStatus(data){
		if (!data){
			return;
		}

		if (!('id' in data)){
			return;
		}
		let pageId = data["id"];

		if ('name' in data){
			let pageName = data["name"];

			for (let i = 0; i < menuPanel.model.getItemsCount(); i++){
				let id = menuPanel.model.getData("id", i);
				if (pageId == id){
					menuPanel.model.setData("name", pageName,  i);

					break;
				}
			}
		}

		if ('icon' in data){
			let pageIcon = data["icon"];

			for (let i = 0; i < menuPanel.model.getItemsCount(); i++){
				let id = menuPanel.model.getData("id", i);
				if (pageId == id){
					menuPanel.model.setData("icon", pageIcon,  i);

					break;
				}
			}
		}
	}

	function clearModels(){
		menuPanel.model.clear();
		menuPanel.activePageIndex = -1;
		menuPanel.activePageId = "";
		allPages.model = 0;
		topAlignmentPages.model = 0;
		bottomAlignmentPages.model = 0;
	}

	function setCollapsed(stateArg){
		collapsed = stateArg;
	}

	function updateAutoCollapse(){
		if (!Style.enableMenuPanelCollapse || menuPanel.autoCollapseWidth <= 0 || !menuPanel.parent){
			return;
		}

		if (menuPanel.parent.width <= 0){
			return;
		}

		let isNarrow = menuPanel.parent.width < menuPanel.autoCollapseWidth;

		if (isNarrow && !menuPanel.collapsed){
			menuPanel.autoCollapsed = true;
			Events.sendEvent("CollapseMenu", true);
		}
		else if (!isNarrow && menuPanel.autoCollapsed){
			Events.sendEvent("ExpandMenu", false);
		}
	}

	function showHint(text, y){
		menuPanel.hintText = text;
		menuPanel.hintY = y;
	}

	function hideHint(){
		menuPanel.hintText = "";
	}

	onModelChanged: {
		Events.sendEvent("MenuModelChanged", menuPanel.model);
		updateGui();
	}

	onCollapsedChanged: {
		menuPanel.hintText = "";

		if(!menuPanel.collapsed){
			menuPanel.menuDefaultWidth = menuPanel.expandedWidth;
			menuPanel.autoCollapsed = false;
		}

		widthAnimation.from = menuPanel.width;
		widthAnimation.to = menuPanel.collapsed ? menuPanel.collapsedWidth : menuPanel.menuDefaultWidth;
		widthAnimation.restart();
	}

	NumberAnimation {
		id: widthAnimation;

		target: menuPanel;
		property: "width";
		duration: 220;
		easing.type: Easing.InOutQuad;

		onFinished: {
			Events.sendEvent("MenuWidthChanged", menuPanel.width)
		}
	}

	function updateGui(){
		if (!model){
			return;
		}

		let savedActivePageId = menuPanel.activePageIndex >= 0 ? menuPanel.activePageId : "";
		let targetIndex = 0;
		if (savedActivePageId !== "" && model.getItemsCount() > 0){
			for (let i = 0; i < model.getItemsCount(); i++){
				let id = model.getData("id", i);
				if (id === savedActivePageId){
					targetIndex = i;
					break;
				}
			}
		}

		topPagesModel.clear();
		bottomPagesModel.clear();

		allPages.model = 0;
		topAlignmentPages.model = 0;
		bottomAlignmentPages.model = 0;

		if (model.getItemsCount() > 0){
			menuPanel.activePageIndex = targetIndex;
			menuPanel.activePageId = model.getData("id", targetIndex);
		}

		for (let i = 0; i < model.getItemsCount(); i++){
			if (model.containsKey("alignment", i)){
				let alignment = model.getData("alignment", i);
				if (alignment === 32/*Qt.AlignTop*/|| alignment === 1){
					let index = topPagesModel.insertNewItem();
					topPagesModel.copyItemDataFromModel(index, model, i)
				}
				else if (alignment === 64/*Qt.AlignBottom*/){
					let index = bottomPagesModel.insertNewItem();
					bottomPagesModel.copyItemDataFromModel(index, model, i)
				}
			}
		}

		allPages.model = model;
		topAlignmentPages.model = topPagesModel;
		bottomAlignmentPages.model = bottomPagesModel;
	}

	function setActivePage(pageId){
		for (let i = 0; i < menuPanel.model.getItemsCount(); i++){
			let id = menuPanel.model.getData("id", i);
			if (id === pageId){
				menuPanel.activePageId = id;
				menuPanel.activePageIndex = i;
				break;
			}
		}
	}


	property Component delegate: Component {
		MenuPanelButton {
			objectName: model["id"] + "Button"

			width: Style.enableMenuPanelCollapse ? menuPanel.expandedWidth : (decorator_ ? decorator_.width : 0);
			height: Style.enableMenuPanelCollapse ? menuPanel.rowHeight : (decorator_ ? decorator_.height : 0);

			Component.onCompleted: {
				if (model.index === 0 && menuPanel.activePageIndex === -1){
					this.clicked();
				}
			}

			text:  model["name"];
			textColor: Style.textColor;
			fontName: menuPanel.fontName;
			menuPanelRef: menuPanel;
			iconSource: (highlighted || selected) ? "../../../" + Style.getIconPath(model["icon"], "On", "Selected"):
													"../../../" + Style.getIconPath(model["icon"], "On", "Normal");

			selected: menuPanel.activePageIndex === model.index;
			property string pageId: model["id"];

			onClicked: {
				menuPanel.setActivePage(model.id)
			}

			onWidthChanged: {
				menuPanel.buttonWidth = Math.max(menuPanel.buttonWidth, width);
			}

			onHeightChanged: {
				menuPanel.buttonHeight = height;
			}

			NavigableItem {
				paths: [model["id"]]
				onActivated: {
					menuPanel.activePageIndex = model.index
				}
			}
		}
	}

	Loader{
		anchors.fill: parent
		sourceComponent: Style.menuPanelDecorator//backgroundComp
	}

	Item {
		id: contentArea;

		anchors.fill: parent;

		clip: true;

		Flickable{
			id: allPagesFlick;

			anchors.top: parent.top;
			anchors.topMargin: Style.enableMenuPanelCollapse ? Style.marginXL + Style.marginXXXS : Style.marginM;
			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.bottom: parent.bottom;

			boundsBehavior: Flickable.StopAtBounds;
			flickableDirection: Flickable.VerticalFlick;
			clip: true;
			contentWidth: allPagesFlick.width;
			contentHeight:  allPagesColumn.height;

			visible: topAlignmentColumn.y + topAlignmentColumn.height > bottomAlignmentColumn.y;

			Column{
				id: allPagesColumn;
				Repeater{
					id: allPages;
					delegate: menuPanel.delegate
				}
			}
		}

		CustomScrollbar {
			id: allPagesScrollbar;
			z: allPagesFlick.z + 1;
			anchors.right: allPagesFlick.right;
			anchors.top: allPagesFlick.top;
			anchors.bottom: allPagesFlick.bottom;
			secondSize: Style.spacingS;
			targetItem: allPagesFlick;
			alwaysVisible: false;
			visible: allPagesFlick.visible;
		}

		Column{
			id: topAlignmentColumn;

			anchors.top: parent.top;
			anchors.left: parent.left;

			width: menuPanel.rowWidth;

			anchors.topMargin: Style.menuPanelTopMargin !==undefined ? Style.menuPanelTopMargin :
																	   !menuPanel.centered ? (Style.enableMenuPanelCollapse ? Style.marginXL + Style.marginXXXS : Style.marginM):
																							 parent.height - bottomAlignmentColumn.height -  height > 0 ? (parent.height - bottomAlignmentColumn.height - height)/2 : 0

			visible: !allPagesFlick.visible;

			Repeater{
				id: topAlignmentPages;

				delegate: Component {
					MenuPanelButton {
						objectName: model["id"] + "Button"

						width: Style.enableMenuPanelCollapse ? menuPanel.expandedWidth : (decorator_ ? decorator_.width : 0);
						height: Style.enableMenuPanelCollapse ? menuPanel.rowHeight : (decorator_ ? decorator_.height : 0);

						text:  model["name"];
						textColor: Style.textColor;
						menuPanelRef: menuPanel;
						iconSource: (highlighted || selected) ? "../../../" + Style.getIconPath(model["icon"], "On", "Selected"):
																"../../../" + Style.getIconPath(model["icon"], "On", "Normal");
						selected: menuPanel.activePageIndex <= topAlignmentPages.count - 1 ? model.index === menuPanel.activePageIndex : false;
						onClicked: {
							menuPanel.setActivePage(model.id)
						}
					}
				}
			}
		}

		Column{
			id: bottomAlignmentColumn;

			anchors.left: parent.left;
			anchors.bottom: parent.bottom;

			width: menuPanel.rowWidth;

			visible: !allPagesFlick.visible;

			Repeater{
				id: bottomAlignmentPages;

				delegate: Component {
					MenuPanelButton {
						objectName: model["id"] + "Button"

						width: Style.enableMenuPanelCollapse ? menuPanel.expandedWidth : (decorator_ ? decorator_.width : 0);
						height: Style.enableMenuPanelCollapse ? menuPanel.rowHeight : (decorator_ ? decorator_.height : 0);

						text:  model["name"];
						textColor: Style.textColor;
						menuPanelRef: menuPanel;
						iconSource: (highlighted || selected) ? "../../../" + Style.getIconPath(model["icon"], "On", "Selected"):
																"../../../" + Style.getIconPath(model["icon"], "On", "Normal");
						selected: menuPanel.activePageIndex > topAlignmentPages.count - 1 ? menuPanel.activePageIndex - topAlignmentPages.count === model.index : false;
						onClicked: {
							menuPanel.setActivePage(model.id)
						}
					}
				}
			}
		}

		Item{
			id: controlPanel
			width: parent.width
			height: Style.controlHeightS
			visible: Style.enableMenuPanelCollapse
			Rectangle{
				anchors.fill: parent
				color: Style.backgroundColor2
				opacity: 0.7
			}

			Rectangle{
				anchors.bottom: parent.bottom
				width: parent.width
				height: 1

				color: Style.backgroundColor

			}
			Item{
				id: collapseRow

				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXXXS

				width: collapseMarker.width
				height: collapseMarker.height

				visible: Style.enableMenuPanelCollapse

				Rectangle {
					id: collapseMarker

					width: Style.buttonWidthS - Style.marginXXS
					height: width

					radius: 1
					color:"transparent"
				}

				Image{
					id: collapseIcon

					width: collapseMarker.width
					height: width

					sourceSize.width: width
					sourceSize.height: height

					source: menuPanel.collapsed ? "qrc:/" + Style.getIconPath("Icons/Expand", Icon.State.Off, Icon.Mode.Disabled)
												: "qrc:/" + Style.getIconPath("Icons/Collapse", Icon.State.Off, Icon.Mode.Disabled)

					opacity: menuButtonArea.containsMouse ? 1.0 : Style.opacityHigh
				}

				MouseArea {
					id: menuButtonArea;

					anchors.fill: collapseMarker;

					hoverEnabled: true;
					cursorShape: Qt.PointingHandCursor;

					onClicked: {
						if(!menuPanel.collapsed){
							Events.sendEvent("CollapseMenu", true)
						}
						else {
							Events.sendEvent("ExpandMenu", false)
						}
					}

					onContainsMouseChanged: {
						if(!menuButtonArea.containsMouse){
							menuPanel.hideHint();
						}
						else if (menuPanel.collapsed){
							menuPanel.showHint(qsTr("Expand menu"),
								collapseIcon.mapToItem(menuPanel, 0, collapseIcon.height / 2).y);
						}
						else if(!menuPanel.collapsed){
							menuPanel.showHint(qsTr("Collapse menu"),
								collapseIcon.mapToItem(menuPanel, 0, collapseIcon.height / 2).y)
						}
					}
				}
			}
		}

	}

	Rectangle {
		visible: Style.enableMenuPanelCollapse;

		anchors.right: parent.right;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;

		width: 1;
		opacity: 0.5;
		color: Style.borderColor;
	}

	Item {
		id: hint;

		x: menuPanel.width + hint.slide;
		y: menuPanel.hintY - height / 2 + Style.marginXXS;
		z: 100;

		width: hintBody.width + Style.spacingS;
		height: Style.controlHeightS + Style.marginXXS;

		visible: hint.opacity > 0;
		opacity: 0;

		property real slide: 0;

		Rectangle {
			id: arrowTip;

			x: Style.spacingS - width / 2;
			z: -1;

			anchors.verticalCenter: parent.verticalCenter;

			width: Style.marginS;
			height: width;
			rotation: 45;
			color: hintBody.color;
		}

		Rectangle {
			id: hintBody;

			anchors.left: parent.left;
			anchors.leftMargin: Style.spacingS;
			anchors.verticalCenter: parent.verticalCenter;

			width: hintLabel.width + 2 * Style.marginM;
			height: parent.height;
			radius: Style.radiusM;
			color: Style.titleColor;

			BaseText {
				id: hintLabel;

				anchors.centerIn: parent;

				text: menuPanel.hintText;
				font.pixelSize: Style.fontSizeM;
				color: Style.baseColor;
			}
		}

		ParallelAnimation {
			id: hintIn;

			NumberAnimation {
				target: hint;
				property: "opacity";
				to: 1;
				duration: 120;
				easing.type: Easing.OutQuad;
			}

			NumberAnimation {
				target: hint;
				property: "slide";
				to: Style.spacingXS;
				duration: 120;
				easing.type: Easing.OutCubic;
			}
		}

		NumberAnimation {
			id: hintOut;

			target: hint;
			property: "opacity";
			to: 0;
			duration: 90;
			easing.type: Easing.InQuad;
		}
	}

	onHintTextChanged: {
		if (menuPanel.hintText === ""){
			hintIn.stop();
			hintOut.restart();
		}
		else if (hint.opacity < 1){
			hintOut.stop();
			hint.slide = 0;
			hintIn.restart();
		}
	}
}

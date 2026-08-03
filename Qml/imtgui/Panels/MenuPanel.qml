import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Rectangle {
	id: menuPanel;

	objectName: "MenuPanel"

	// The rows are clipped by contentArea instead, which leaves the hint card
	// free to stand outside the rail.
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
	// Exactly wide enough to hold an icon at its usual distance from the edge,
	// which is what keeps the icons still while the panel moves.
	property int collapsedWidth: Style.menuPanelMinWidth;

	// What the hint card is naming, and where along the rail it points.
	property string hintText: "";
	property real hintY: 0;

	width: menuPanel.collapsedWidth;

	Component.onCompleted: {
		menuPanel.width = menuPanel.collapsed ? menuPanel.collapsedWidth : menuPanel.expandedWidth;
		menuPanel.menuDefaultWidth = menuPanel.expandedWidth;

		Events.subscribeEvent("MenuModelRequest", menuPanel.onMenuModelRequest);
		Events.subscribeEvent("UpdatePageVisualStatus", menuPanel.updateVisualStatus);
		Events.subscribeEvent("ChangePage", menuPanel.setActivePage);
		Events.subscribeEvent("CollapseMenu", menuPanel.setCollapsed);
		Events.subscribeEvent("ExpandMenu", menuPanel.setCollapsed);
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

	// A row asks for the card by name, and gives it back by name: two rows can
	// trade the pointer between them without one clearing the other's card.
	function showHint(text, y){
		menuPanel.hintText = text;
		menuPanel.hintY = y;
	}

	function hideHint(text){
		if (menuPanel.hintText === text){
			menuPanel.hintText = "";
		}
	}

	onModelChanged: {
		Events.sendEvent("MenuModelChanged", menuPanel.model);
		updateGui();
	}

	onCollapsedChanged: {
		menuPanel.hintText = "";

		if(!menuPanel.collapsed){
			menuPanel.menuDefaultWidth = menuPanel.expandedWidth;
		}

		// Both ends spelled out: from wherever the edge stands right now to the
		// other width. Left to work its start out for itself the animation can
		// pick up a width the panel has not settled on yet and run from there.
		widthAnimation.from = menuPanel.width;
		widthAnimation.to = menuPanel.collapsed ? menuPanel.collapsedWidth : menuPanel.menuDefaultWidth;
		widthAnimation.restart();
	}

	// Eased at both ends. A curve that leaves at full speed shows every frame it
	// misses; one that builds up and settles reads as motion rather than a jump.
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

			// Kept at the open width and clipped, rather than resized with the
			// panel: a width that moves re-elides every label on every frame.
			width: menuPanel.expandedWidth;
			height: menuPanel.rowHeight;

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
			anchors.topMargin: Style.marginS;
			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.bottom: collapseRow.visible ? collapseRow.top : parent.bottom;

			boundsBehavior: Flickable.StopAtBounds;
			// The rows stay at the open width, so without this the closed rail
			// would let itself be dragged sideways over them.
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

			width: menuPanel.expandedWidth;

			anchors.topMargin: Style.menuPanelTopMargin !==undefined ? Style.menuPanelTopMargin :
																	   !menuPanel.centered ? Style.marginS :
																							 parent.height - bottomAlignmentColumn.height -  height > 0 ? (parent.height - bottomAlignmentColumn.height - height)/2 : 0

			visible: !allPagesFlick.visible;

			Repeater{
				id: topAlignmentPages;

				delegate: Component {
					MenuPanelButton {
						objectName: model["id"] + "Button"

						width: menuPanel.expandedWidth;
						height: menuPanel.rowHeight;

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
			anchors.bottom: collapseRow.visible ? collapseRow.top : parent.bottom;

			width: menuPanel.expandedWidth;

			visible: !allPagesFlick.visible;

			Repeater{
				id: bottomAlignmentPages;

				delegate: Component {
					MenuPanelButton {
						objectName: model["id"] + "Button"

						width: menuPanel.expandedWidth;
						height: menuPanel.rowHeight;

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
			id: collapseRow

			anchors.bottom: parent.bottom
			anchors.left: parent.left

			width: menuPanel.expandedWidth;
			height: visible ? menuPanel.rowHeight : 0

			visible: Style.enableMenuPanelCollapse


			Rectangle {
				id: collapseMarker

				anchors.left: parent.left
				anchors.leftMargin: Style.marginS
				anchors.top: parent.top
				anchors.bottom: parent.bottom

				width: (!menuPanel ? parent.width : menuPanel.width) - 2 * Style.marginS

				radius: Style.buttonRadius
				color: menuButtonArea.containsMouse ? Style.backgroundColor2 : "transparent"
			}

			Image{
				id: collapseIcon

				anchors.left: parent.left
				anchors.leftMargin: Style.marginL
				anchors.verticalCenter: parent.verticalCenter

				width: menuPanel.iconSize
				height: width

				sourceSize.width: width
				sourceSize.height: height

				source: menuPanel.collapsed ? "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Disabled)
											: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Disabled)
			}

			BaseText {
				anchors.left: collapseIcon.right
				anchors.leftMargin: Style.marginM
				anchors.right: parent.right
				anchors.rightMargin: Style.marginL
				anchors.verticalCenter: parent.verticalCenter

				visible: !menuPanel.collapsed
				text: qsTr("Collapse")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
				elide: Text.ElideRight
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
					if (menuButtonArea.containsMouse && menuPanel.collapsed){
						menuPanel.showHint(qsTr("Expand menu"),
							collapseIcon.mapToItem(menuPanel, 0, collapseIcon.height / 2).y);
					}
					else{
						menuPanel.hideHint(qsTr("Expand menu"));
					}
				}
			}
		}
	}

	// Parts the rail from the page beside it without drawing a hard edge.
	Rectangle {
		anchors.right: parent.right;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;

		width: 1;
		opacity: 0.5;
		color: Style.borderColor;
	}

	// The name of the page under the pointer while the rail is closed. It stands
	// outside the rail, so it lives here rather than inside contentArea, and it
	// carries a tip back towards the icon it belongs to.
	Item {
		id: hint;

		x: menuPanel.width + hint.slide;
		y: menuPanel.hintY - height / 2;
		z: 100;

		width: hintBody.width + Style.spacingS;
		height: Style.controlHeightM;

		visible: hint.opacity > 0;
		opacity: 0;

		property real slide: 0;

		Rectangle {
			id: arrowTip;

			x: Style.spacingS - width / 2;
			z: -1;

			anchors.verticalCenter: parent.verticalCenter;

			width: Style.marginM;
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

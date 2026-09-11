import QtQuick 2.12
import Qt.labs.settings 1.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import Qt.labs.settings 1.0

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

	// Keeps the selected page across a browser reload.
	property Settings storage: Settings {
		category: "MenuPanel";
	}

	// Set while updateGui() falls back to the first page because the stored one is missing.
	property bool __keepStoredPage: false;

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
	property int expandedWidth: menuPanel.railMode && Style.menuPanelRailWidth !== undefined ? Style.menuPanelRailWidth
		: Style.menuPanelWidth !== undefined ? Style.menuPanelWidth : Style.sizeHintXXS;
	property int collapsedWidth: Style.menuPanelMinWidth;

	property int autoCollapseWidth: Style.menuPanelAutoCollapseWidth !== undefined ? Style.menuPanelAutoCollapseWidth : 0;
	property bool autoCollapsed: false;

	readonly property int rowWidth: Style.enableMenuPanelCollapse ? menuPanel.expandedWidth : menuPanel.buttonWidth;

	property string hintText: "";
	property real hintY: 0;

	// Without a top bar the rail has to carry the brand and the account controls
	// itself. Applications that keep TopPanel keep the plain list of pages they
	// always had.
	readonly property bool railMode: Style.enableTopPanel !== undefined ? !Style.enableTopPanel : false;

	property bool loggedIn: false;

	// Without user management there are no accounts to show, exactly as
	// TopRightPanelDecorator hides its UserPanel in that mode.
	property bool userManagementAvailable: true;

	// On the web build settings only make sense once there is a session to settle
	// them against.
	readonly property bool settingsAvailable: Qt.platform.os !== "web" || menuPanel.loggedIn;

	// Every icon in the rail - brand, pages, account - sits on this line, so
	// nothing shifts sideways when the panel collapses.
	readonly property real railCenterX: Style.marginL + menuPanel.iconSize / 2;
	readonly property int headerHeight: Style.controlHeightL + Style.marginS;

	width: Style.enableMenuPanelCollapse ? menuPanel.collapsedWidth : menuPanel.rowWidth;

	// Logged out the page list is cleared, and a rail with nothing in it is just a
	// white stripe beside the sign-in page.
	visible: !menuPanel.railMode || allPages.count > 0;

	// Folded or open is the user's choice, so it outlives a page reload. Read and
	// written by key like AuthorizationController does - declared properties on
	// Settings are not carried over by the web build.
	Settings {
		id: railSettings;

		category: "MenuPanel";
	}

	Component.onCompleted: {
		if (Style.enableMenuPanelCollapse){
			menuPanel.restoreCollapsed();

			menuPanel.menuDefaultWidth = menuPanel.expandedWidth;
		}

		Events.subscribeEvent("MenuModelRequest", menuPanel.onMenuModelRequest);
		Events.subscribeEvent("UpdatePageVisualStatus", menuPanel.updateVisualStatus);
		Events.subscribeEvent("ChangePage", menuPanel.setActivePage);
		Events.subscribeEvent("CollapseMenu", menuPanel.setCollapsed);
		Events.subscribeEvent("ExpandMenu", menuPanel.setCollapsed);

		menuPanel.updateAutoCollapse();

		menuPanel.setUserMode(AuthorizationController.getUserMode());
	}

	// An empty mode means the server has not answered yet, and the account row
	// stays as it is until it does.
	function setUserMode(userMode){
		if (userMode === "NO_USER_MANAGEMENT"){
			menuPanel.userManagementAvailable = false;
		}
		else if (userMode !== ""){
			menuPanel.userManagementAvailable = true;
		}
	}

	Connections {
		target: AuthorizationController;

		function onLoggedIn(){
			menuPanel.loggedIn = true;
			menuPanel.restoreCollapsed();
		}

		function onLoggedOut(){
			menuPanel.loggedIn = false;
		}

		function onUserModeChanged(userMode){
			menuPanel.setUserMode(userMode);
		}
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

	onActivePageIndexChanged: {
		if (menuPanel.__keepStoredPage){
			return;
		}

		if (menuPanel.activePageIndex < 0 || !menuPanel.model || menuPanel.activePageIndex >= menuPanel.model.getItemsCount()){
			return;
		}

		menuPanel.storage.setValue("activePageId", menuPanel.model.getData("id", menuPanel.activePageIndex));
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

	// Only a deliberate fold is remembered - the automatic one that happens in a
	// narrow window is not a preference.
	function storeCollapsed(stateArg){
		if (menuPanel.railMode){
			railSettings.setValue(menuPanel.collapsedKey(), stateArg ? "true" : "false");
		}
	}

	// The fold is a personal choice, so the key carries the user it belongs to.
	// Before a login there is no identity yet and the shared key is used.
	function collapsedKey(){
		let userId = AuthorizationController.userTokenProvider ? AuthorizationController.userTokenProvider.userId : "";

		return userId ? "collapsed_" + userId : "collapsed";
	}

	function restoreCollapsed(){
		if (!Style.enableMenuPanelCollapse){
			return;
		}

		if (menuPanel.railMode){
			menuPanel.collapsed = railSettings.value(menuPanel.collapsedKey(), "") === "true";
		}

		widthAnimation.stop();
		menuPanel.width = menuPanel.collapsed ? menuPanel.collapsedWidth : menuPanel.expandedWidth;
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

		let storedPageId = menuPanel.storage.value("activePageId", "");
		let savedActivePageId = menuPanel.activePageIndex >= 0 ? menuPanel.activePageId : (storedPageId ? storedPageId : "");
		let targetIndex = 0;
		let isPageFound = false;
		if (savedActivePageId !== "" && model.getItemsCount() > 0){
			for (let i = 0; i < model.getItemsCount(); i++){
				let id = model.getData("id", i);
				if (id === savedActivePageId){
					targetIndex = i;
					isPageFound = true;
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
			menuPanel.__keepStoredPage = savedActivePageId !== "" && !isPageFound;
			menuPanel.activePageIndex = targetIndex;
			menuPanel.activePageId = model.getData("id", targetIndex);
			menuPanel.__keepStoredPage = false;
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

	// Brand corner. Open it reads as a title with a way to fold the rail away;
	// folded it is just the product mark, and only becomes a control once the
	// pointer is on it - so the collapsed rail stays quiet until you reach for it.
	Item {
		id: header;

		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;

		height: menuPanel.railMode ? menuPanel.headerHeight : 0;

		visible: menuPanel.railMode;
		clip: true;

		// Folded, the mark doubles as the control that unfolds the rail. The state
		// has to be read off the panel and not off the mouse area alone: a disabled
		// area keeps whatever containsMouse it had when it went quiet, which left
		// the unfold arrow frozen in place after the first click.
		readonly property bool brandHovered: menuPanel.collapsed
			&& (brandArea.containsMouse || brandArea.activeFocus);

		// Held back until the rail is most of the way open - drawn any earlier the
		// name is squeezed against the mark and reads as crawling out from under it.
		readonly property int brandNameRevealWidth: menuPanel.collapsedWidth
			+ Math.round((menuPanel.expandedWidth - menuPanel.collapsedWidth) * 0.6);

		Rectangle {
			id: brandTarget;

			x: menuPanel.railCenterX - width / 2;
			anchors.verticalCenter: parent.verticalCenter;

			width: Style.controlHeightM;
			height: width;
			radius: width / 2;

			color: header.brandHovered ? Style.alternateBaseColor : "transparent";
		}

		Image {
			id: brandIcon;

			anchors.centerIn: brandTarget;

			width: menuPanel.iconSize;
			height: width;

			fillMode: Image.PreserveAspectFit;
			sourceSize.width: width;
			sourceSize.height: height;

			source: header.brandHovered ? "qrc:/" + Style.getIconPath("Icons/SidebarExpand", Icon.State.On, Icon.Mode.Normal)
				: context && context.appIcon && context.appIcon !== "" ? context.appIcon
				: "qrc:/" + Style.getLogoIconPath(Icon.State.On, Icon.Mode.Normal);
		}

		MouseArea {
			id: brandArea;

			anchors.fill: brandTarget;

			enabled: Style.enableMenuPanelCollapse;
			hoverEnabled: true;
			cursorShape: menuPanel.collapsed ? Qt.PointingHandCursor : Qt.ArrowCursor;

			// Folded, this is the only way back out of the rail, so it has to be
			// reachable and operable without a pointer as well.
			activeFocusOnTab: menuPanel.collapsed;

			Accessible.role: Accessible.Button;
			Accessible.name: qsTr("Expand menu");
			Accessible.onPressAction: brandArea.expandMenu();

			function expandMenu(){
				if (menuPanel.collapsed){
					Events.sendEvent("ExpandMenu", false);
					menuPanel.storeCollapsed(false);
				}
			}

			onClicked: {
				brandArea.expandMenu();
			}

			Keys.onPressed: {
				if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter || event.key === Qt.Key_Space){
					brandArea.expandMenu();
					event.accepted = true;
				}
			}
		}

		onBrandHoveredChanged: {
			if (header.brandHovered){
				menuPanel.showHint(qsTr("Expand menu"),
					brandTarget.mapToItem(menuPanel, 0, brandTarget.height / 2).y);
			}
			else {
				menuPanel.hideHint();
			}
		}

		Text {
			id: brandName;

			anchors.left: brandTarget.right;
			anchors.leftMargin: Style.marginS;
			anchors.right: navButtons.visible ? navButtons.left : collapseButton.left;
			anchors.rightMargin: Style.marginXXS;
			anchors.verticalCenter: parent.verticalCenter;

			opacity: !menuPanel.collapsed && menuPanel.width >= header.brandNameRevealWidth ? 1 : 0;
			visible: brandName.opacity > 0;

			text: context && context.appName ? context.appName : "";
			color: Style.titleColor;
			font.family: Style.fontFamilyBold;
			font.pixelSize: Style.fontSizeL;
			elide: Text.ElideRight;
			maximumLineCount: 1;
			verticalAlignment: Text.AlignVCenter;

			Behavior on opacity {
				NumberAnimation {
					duration: 120;
					easing.type: Easing.OutQuad;
				}
			}
		}

		// A name too long for the header is cut, so the whole of it is still
		// readable on hover.
		Text {
			id: brandNameMeasure;

			font.family: brandName.font.family;
			font.pixelSize: brandName.font.pixelSize;
			text: brandName.text;
			wrapMode: Text.NoWrap;
			visible: false;
		}

		MouseArea {
			id: brandNameArea;

			anchors.fill: brandName;

			enabled: brandName.visible && brandNameMeasure.width > brandName.width;
			hoverEnabled: true;
		}

		TooltipArea {
			anchors.fill: brandName;

			mouseArea: brandNameArea;
			text: brandNameArea.enabled ? brandName.text : "";
		}

		// Back and forward followed the top bar out. They earn their place in the
		// header only once there is a trail to walk, so the usual header is just
		// the mark, the name and the fold control.
		Row {
			id: navButtons;

			anchors.right: collapseButton.left;
			anchors.rightMargin: Style.marginXXS;
			anchors.verticalCenter: parent.verticalCenter;

			height: Style.controlHeightM;

			// Anchored to the folding button rather than to the panel edge, the row
			// slid in over the brand mark while the rail was still narrow. It waits
			// for the same width the name does.
			visible: !menuPanel.collapsed
				&& menuPanel.width >= header.brandNameRevealWidth
				&& (prevButton.enabled || nextButton.enabled);

			Component.onCompleted: {
				prevButton.enabled = NavigationController.hasPrev();
				nextButton.enabled = NavigationController.hasNext();
			}

			Connections {
				target: NavigationController;

				function onCurrentIndexChanged(index){
					prevButton.enabled = NavigationController.hasPrev();
					nextButton.enabled = NavigationController.hasNext();
				}
			}

			ToolButton {
				id: prevButton;
				objectName: "NavigatePrevButton";

				width: Style.buttonWidthM;
				height: Style.controlHeightM;

				enabled: false;

				tooltipText: qsTr("Back");
				iconSource: enabled ? "qrc:/" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)
									: "qrc:/" + Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled);

				decorator: Component {
					ToolButtonDecorator {
						icon.width: Style.iconSizeS;
						radius: height / 2;
					}
				}

				onClicked: {
					NavigationController.prev();
				}
			}

			ToolButton {
				id: nextButton;
				objectName: "NavigateNextButton";

				width: Style.buttonWidthM;
				height: Style.controlHeightM;

				enabled: false;

				tooltipText: qsTr("Forward");
				iconSource: enabled ? "qrc:/" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
									: "qrc:/" + Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled);

				decorator: Component {
					ToolButtonDecorator {
						icon.width: Style.iconSizeS;
						radius: height / 2;
					}
				}

				onClicked: {
					NavigationController.next();
				}
			}
		}

		ToolButton {
			id: collapseButton;
			objectName: "CollapseMenuButton";

			anchors.right: parent.right;
			anchors.rightMargin: Style.marginS;
			anchors.verticalCenter: parent.verticalCenter;

			width: Style.controlHeightM;
			height: width;

			visible: Style.enableMenuPanelCollapse && !menuPanel.collapsed;

			tooltipText: qsTr("Collapse menu");
			iconSource: "qrc:/" + Style.getIconPath("Icons/SidebarCollapse", Icon.State.On, Icon.Mode.Normal);

			decorator: Component {
				ToolButtonDecorator {
					icon.width: Style.iconSizeS;
					radius: height / 2;
				}
			}

			onClicked: {
				Events.sendEvent("CollapseMenu", true);
				menuPanel.storeCollapsed(true);
				brandArea.forceActiveFocus();
			}
		}
	}

	// Account and settings live at the foot of the rail, where the top-right
	// corner of the old bar used to put them. Loaded only in rail mode: a second
	// UserPanel would otherwise sit here invisibly and repeat its organisation
	// requests alongside the one in TopPanel.
	Item {
		id: footer;

		anchors.bottom: parent.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;

		height: menuPanel.railMode ? footerLoader.height + 2 * Style.marginXS : 0;

		visible: menuPanel.railMode;
		clip: true;

		Rectangle {
			anchors.top: parent.top;
			anchors.left: parent.left;
			anchors.leftMargin: Style.marginS;
			anchors.right: parent.right;
			anchors.rightMargin: Style.marginS;

			height: 1;
			opacity: 0.5;
			color: Style.borderColor;
		}

		Loader {
			id: footerLoader;

			anchors.top: parent.top;
			anchors.topMargin: Style.marginXS;
			anchors.left: parent.left;

			width: menuPanel.rowWidth;

			active: menuPanel.railMode;
			sourceComponent: menuPanel.footerComp;
		}
	}

	property Component footerComp: Component {
		Column {
			id: footerColumn;

			width: menuPanel.rowWidth;

			UserPanel {
				id: userPanel;

				width: parent.width;
				height: menuPanel.userManagementAvailable ? menuPanel.rowHeight : 0;

				visible: menuPanel.userManagementAvailable;

				collapsed: menuPanel.collapsed;
				menuPanelRef: menuPanel;

				// Open, the gear rides along on the account row; folded there is no
				// room for two controls on one line, so it becomes a row of its own.
				settingsButtonVisible: menuPanel.settingsAvailable && !menuPanel.collapsed;

				onSettingsClicked: {
					Events.sendEvent("ShowPreferencePage");
				}
			}

			MenuPanelButton {
				id: settingsButton;
				objectName: "PreferenceButton";

				width: parent.width;
				height: menuPanel.rowHeight;

				// Without the account row the gear has nowhere to ride along, so it
				// keeps its own row also when the rail is open.
				visible: menuPanel.settingsAvailable && (menuPanel.collapsed || !menuPanel.userManagementAvailable);

				text: qsTr("Settings");
				textColor: Style.textColor;
				fontName: menuPanel.fontName;
				menuPanelRef: menuPanel;

				iconSource: highlighted ? "qrc:/" + Style.getIconPath("Icons/Settings", Icon.State.On, Icon.Mode.Selected)
										: "qrc:/" + Style.getIconPath("Icons/Settings", Icon.State.On, Icon.Mode.Normal);

				onClicked: {
					Events.sendEvent("ShowPreferencePage");
				}
			}
		}
	}

	Item {
		id: contentArea;

		anchors.top: header.bottom;
		anchors.bottom: footer.top;
		anchors.left: parent.left;
		anchors.right: parent.right;

		clip: true;

		Flickable{
			id: allPagesFlick;

			anchors.top: parent.top;
			anchors.topMargin: menuPanel.railMode ? Style.marginXS
				: Style.enableMenuPanelCollapse ? Style.marginXL + Style.marginXXXS : Style.marginM;
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

anchors.topMargin: menuPanel.railMode ? Style.marginXS
				: Style.menuPanelTopMargin !== undefined ? Style.menuPanelTopMargin
				: !menuPanel.centered ? (Style.enableMenuPanelCollapse ? Style.marginXL + Style.marginXXXS : Style.marginM)
				: parent.height - bottomAlignmentColumn.height - height > 0 ? (parent.height - bottomAlignmentColumn.height - height) / 2 : 0

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

		// Classic shell only - in rail mode the folding control lives in the header
		// next to the product name.
		Item{
			id: controlPanel
			width: parent.width
			height: Style.controlHeightS
			visible: Style.enableMenuPanelCollapse && !menuPanel.railMode
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

				visible: controlPanel.visible

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

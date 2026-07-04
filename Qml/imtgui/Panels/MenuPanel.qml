import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Rectangle {
	id: menuPanel;

	width: buttonWidth;
	objectName: "MenuPanel"

	clip: true;

	color: Style.backgroundColor;
	radius: Style.menuPanelRadius;

	property string textColor: Style.textColor;
	property string fontName: Style.fontFamily;
	property string activePageId;
	property string activePageName;
	property string activeIcon;
	property string firstElementImageSources: "";

	property int activePageIndex: -1;

	// Map of pageId -> pending item count, driving the per-page notification
	// badge (e.g. the Events / Activity indicator). Updated via the
	// "UpdatePageBadge" application event.
	property var pageBadges: ({});

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

	Component.onCompleted: {
		Events.subscribeEvent("MenuModelRequest", menuPanel.onMenuModelRequest);
		Events.subscribeEvent("UpdatePageVisualStatus", menuPanel.updateVisualStatus);
		Events.subscribeEvent("UpdatePageBadge", menuPanel.updateBadge);
		Events.subscribeEvent("ChangePage", menuPanel.setActivePage);
		Events.subscribeEvent("CollapseMenu", menuPanel.setCollapsed);
		Events.subscribeEvent("ExpandMenu", menuPanel.setCollapsed);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("MenuModelRequest", menuPanel.onMenuModelRequest);
		Events.unSubscribeEvent("UpdatePageVisualStatus", menuPanel.updateVisualStatus);
		Events.unSubscribeEvent("UpdatePageBadge", menuPanel.updateBadge);
		Events.unSubscribeEvent("ChangePage", menuPanel.setActivePage);
		Events.unSubscribeEvent("CollapseMenu", menuPanel.setCollapsed);
		Events.unSubscribeEvent("ExpandMenu", menuPanel.setCollapsed);

	}

	// data: { id: <pageId>, count: <int> }
	function updateBadge(data){
		if (!data || !('id' in data)){
			return;
		}

		let count = ('count' in data) ? data["count"] : 0;
		let badges = menuPanel.pageBadges;
		badges[data["id"]] = count;
		// Reassign so the property-change notification fires for bindings.
		menuPanel.pageBadges = badges;
	}

	onActivePageIdChanged: {
		if(activePageId !== ""){
			Events.sendEvent("ActivePageIdChanged", activePageId);
			
			NavigationController.push(activePageId)
		}
	}

	onWidthChanged: {
		Events.sendEvent("MenuWidthChanged", width)
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

	onModelChanged: {
		Events.sendEvent("MenuModelChanged", menuPanel.model);
		updateGui();
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

			badgeCount: menuPanel.pageBadges[model["id"]] ? menuPanel.pageBadges[model["id"]] : 0;

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

	ToolButton {
		id: collapseButton;

		anchors.top: parent.top
		anchors.right: parent.right
		anchors.topMargin: Style.marginXS
		anchors.rightMargin: Style.marginXS

		width: height;
		height: Style.buttonHeightXXS - 2;
		z:100

		visible: false//Style.enableMenuPanelCollapse && !collapsed

		//iconSource: "../../../" + Style.getIconPath(!menuPanel.collapsed ? "Icons/Collapse" : "Icons/Expand", Icon.State.On, Icon.Mode.Normal);
		iconSource: "../../../" + Style.getIconPath("Icons/Close" , Icon.State.On, Icon.Mode.Normal);

		//tooltipText: !menuPanel.collapsed ? qsTr("Collapse") : qsTr("Expand");
		property real menuDefaultWidth: 0
		property bool collapsed: menuPanel.collapsed
		onCollapsedChanged: {
			if(collapsed){
				menuDefaultWidth = menuPanel.width
				menuPanel.width = Style.menuPanelMinWidth
			}
			else {
				menuPanel.width = menuDefaultWidth
			}
		}
		onClicked: {
			//menuPanel.collapsed = !menuPanel.collapsed
			menuPanel.collapsed = true
		}
	}

	Loader{
		anchors.fill: parent
		sourceComponent: Style.menuPanelDecorator//backgroundComp
	}



	Flickable{
		id: allPagesFlick;

		anchors.top: menuPanel.top;
		anchors.left: menuPanel.left;
		anchors.right: menuPanel.right;
		anchors.bottom: menuPanel.bottom;

		boundsBehavior: Flickable.StopAtBounds;
		clip: true;
		contentWidth: allPagesColumn.width;
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

	Column{
		id: topAlignmentColumn;

		anchors.top: menuPanel.top;
		anchors.left: menuPanel.left;
		anchors.right: menuPanel.right;

		anchors.topMargin: Style.menuPanelTopMargin !==undefined ? Style.menuPanelTopMargin : !menuPanel.centered ? 0 : parent.height - bottomAlignmentColumn.height - height > 0 ? (parent.height - bottomAlignmentColumn.height - height)/2 : 0 ;

		visible: !allPagesFlick.visible;

		Repeater{
			id: topAlignmentPages;

			delegate: Component {
				MenuPanelButton {
					objectName: model["id"] + "Button"
					text:  model["name"];
					textColor: Style.textColor;
					menuPanelRef: menuPanel;
					iconSource: (highlighted || selected) ? "../../../" + Style.getIconPath(model["icon"], "On", "Selected"):
															"../../../" + Style.getIconPath(model["icon"], "On", "Normal");
					selected: menuPanel.activePageIndex <= topAlignmentPages.count - 1 ? model.index === menuPanel.activePageIndex : false;
					badgeCount: menuPanel.pageBadges[model["id"]] ? menuPanel.pageBadges[model["id"]] : 0;
					onClicked: {
						menuPanel.setActivePage(model.id)
					}
				}
			}
		}
	}

	Column{
		id: bottomAlignmentColumn;

		anchors.left: menuPanel.left;
		anchors.right: menuPanel.right;
		anchors.bottom: menuPanel.bottom;

		visible: !allPagesFlick.visible;

		Repeater{
			id: bottomAlignmentPages;

			delegate: Component {
				MenuPanelButton {
					objectName: model["id"] + "Button"
					text:  model["name"];
					textColor: Style.textColor;
					menuPanelRef: menuPanel;
					iconSource: (highlighted || selected) ? "../../../" + Style.getIconPath(model["icon"], "On", "Selected"):
															"../../../" + Style.getIconPath(model["icon"], "On", "Normal");
					selected: menuPanel.activePageIndex > topAlignmentPages.count - 1 ? menuPanel.activePageIndex - topAlignmentPages.count === model.index : false;
					badgeCount: menuPanel.pageBadges[model["id"]] ? menuPanel.pageBadges[model["id"]] : 0;
					onClicked: {
						menuPanel.setActivePage(model.id)
					}
				}
			}
		}

	}

}

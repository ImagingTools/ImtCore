import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Rectangle {
	id: menuPanel;

	width: buttonWidth;

	clip: true;

	color: Style.backgroundColor;
	radius: 7;

	property string textColor: Style.textColor;
	property string fontName: "Helvetica";
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

	Component.onCompleted: {
		Events.subscribeEvent("MenuModelRequest", menuPanel.onMenuModelRequest);
		Events.subscribeEvent("UpdatePageVisualStatus", menuPanel.updateVisualStatus);
		Events.subscribeEvent("ChangePage", menuPanel.setActivePage);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("MenuModelRequest", menuPanel.onMenuModelRequest);
		Events.unSubscribeEvent("UpdatePageVisualStatus", menuPanel.updateVisualStatus);
		Events.unSubscribeEvent("ChangePage", menuPanel.setActivePage);
	}

	onActivePageIdChanged: {
		if(menuPanel.activePageId !== ""){
			Events.sendEvent("ActivePageIdChanged", menuPanel.activePageId);
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

	property bool block: false;
	onActivePageIndexChanged: {
		if (block || activePageIndex < 0){
			return;
		}

		let index = menuPanel.activePageIndex
		NavigationController.push(menuPanel, function(menuPanel){
			menuPanel.block = true;
			menuPanel.activePageIndex = index;
			menuPanel.block = false;
		}
		);
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
		allPages.model = 0;
		topAlignmentPages.model = 0;
		bottomAlignmentPages.model = 0;
	}

	onModelChanged: {
		Events.sendEvent("MenuModelChanged", menuPanel.model);
		updateGui();
	}

	function updateGui(){
		if (!model){
			return;
		}

		topPagesModel.clear();
		bottomPagesModel.clear();

		allPages.model = 0;
		topAlignmentPages.model = 0;
		bottomAlignmentPages.model = 0;
		menuPanel.activePageIndex = 0;
		menuPanel.activePageId = "";

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

        if (model.getItemsCount() > 0){
            menuPanel.activePageId = model.getData("id",0);
        }
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
		}
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

		anchors.topMargin: Style.menuPanelTopMargin !== undefined ? Style.menuPanelTopMargin :
									!menuPanel.centered ? 0 : parent.height - bottomAlignmentPages.height - allPagesFlick.contentHeight > 0 ?
											(parent.height - bottomAlignmentPages.height - allPagesFlick.contentHeight)/2 : 0 ;

		visible: !allPagesFlick.visible;

		Repeater{
			id: topAlignmentPages;

			delegate: Component {
				MenuPanelButton {
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

		anchors.left: menuPanel.left;
		anchors.right: menuPanel.right;
		anchors.bottom: menuPanel.bottom;

		visible: !allPagesFlick.visible;

		Repeater{
			id: bottomAlignmentPages;

			delegate: Component {
				MenuPanelButton {
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

}


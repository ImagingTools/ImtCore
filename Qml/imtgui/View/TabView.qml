import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

Item {
	id: root;

	property ListModel tabModel: ListModel {
		dynamicRoles: true;
	}

	property alias currentIndex: tabPanel.selectedIndex;
	property alias closable: tabPanel.isCloseEnable;

	property int mainMargin: Style.sizeMainMargin;

	signal tabLoaded(int index, string tabId, var tabItem);
	signal tabClicked(var mouse, var tabItem, int index);
	signal startTabContentLoading(string tabId);
	signal stopTabContentLoading(string tabId);

	onStartTabContentLoading: {
		tabPanel.startTabContentLoading(tabId);
	}

	onStopTabContentLoading: {
		tabPanel.stopTabContentLoading(tabId);
	}

	function clear(){
		tabModel.clear();
	}

	function addTab(tabId, tabName, tabComp, icon, description, waitName){
		if (!waitName){
			waitName = false;
		}

		tabModel.append({Id: tabId, Name: tabName, SourceComponent: tabComp, Icon: icon, Description: description, WaitName: waitName})
	}

	function removeTab(tabId){
		let index = getIndexById(tabId);
		if (index >= 0){
			tabModel.remove(index);
		}

		if (index <= currentIndex && index > 0){
			currentIndex--;
		}
	}

	function getTabById(tabId){
		let index = getIndexById(tabId);
		if (index >= 0){
			return getTabByIndex(index);
		}

		return null;
	}

	function setTabDescription(tabId, description){
		let index = getIndexById(tabId);
		if (index < 0){
			return;
		}

		tabModel.setProperty(index, "Description", description);
	}

	function setTabIcon(tabId, icon){
		let index = getIndexById(tabId);
		if (index < 0){
			return;
		}

		tabModel.setProperty(index, "Icon", icon);
	}

	function setTabName(tabId, name){
		let index = getIndexById(tabId);
		if (index < 0){
			return;
		}

		tabModel.setProperty(index, "Name", name);
	}

	function getTabName(tabId){
		let index = getIndexById(tabId);
		if (index < 0){
			return "";
		}

		return tabModel.get(index).Name;
	}

	function getTabByIndex(index){
		if (index < 0 || bodyRepeater.count <= index){
			return null;
		}

		let loaderItem = bodyRepeater.itemAt(index)
		return loaderItem.item;
	}

	function getIndexById(tabId){
		for (let i = 0; i < tabModel.count; i++){
			let id = tabModel.get(i).Id;
			if (id === tabId){
				return i;
			}
		}

		return -1;
	}

	function getTabIdByIndex(index){
		if (index < 0 || index >= tabModel.count){
			return "";
		}

		return tabModel.get(index).Id;
	}

	function onCloseTab(index){
		let tabId = getTabIdByIndex(index);
		removeTab(tabId);
	}

	TabPanel {
		id: tabPanel;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		model: root.tabModel;
		isCloseEnable: false
		clip: true;
		onRightClicked: {
			if (tabPanel.selectedIndex < root.tabModel.count - 1){
				tabPanel.selectedIndex++;
			}
		}

		onLeftClicked: {
			if (tabPanel.selectedIndex > 0){
				tabPanel.selectedIndex--;
			}
		}

		onTabClicked: {
			root.tabClicked(mouse, tabItem, index);
		}

		onCloseItem: {
			root.onCloseTab(index);
		}
	}

	Rectangle {
		id: bodyTab;
		z: 5;
		anchors.left: parent.left;
		anchors.top: tabPanel.bottom;
		anchors.bottom: parent.bottom;
		anchors.right: parent.right;
		color: Style.backgroundColor2;

		Repeater {
			id: bodyRepeater;
			anchors.fill: parent;
			model: root.tabModel;
			delegate: Loader {
				id: bodyLoader;
				anchors.fill: parent;
				sourceComponent: model.SourceComponent;
				visible: tabPanel.selectedIndex == model.index;

				onLoaded: {
					root.tabLoaded(model.index, model.Id, item);
				}

				onStatusChanged: {
					if (status === Loader.Error){
						console.error("Unable to load component: ", bodyLoader.source);
					}
				}
			}
		}
	}
}

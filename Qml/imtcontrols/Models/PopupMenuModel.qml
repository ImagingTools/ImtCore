import QtQuick 2.12

ListModel {
	id: popupMenuModel;

	function addItem(id, name, icon, enabled){
		popupMenuModel.append({"id": id, "name": name, "icon": icon, "isEnabled": enabled});
	}

	function addSeparator(){
		popupMenuModel.append({"id": "", "name": "", "icon": "", "isEnabled": false});
	}
}


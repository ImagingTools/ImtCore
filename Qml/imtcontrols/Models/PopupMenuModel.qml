import QtQuick 2.12

ListModel {
	id: popupMenuModel;

	function addItem(id, name, icon, enabled){
		popupMenuModel.append({"Id": id, "Name": name, "Icon": icon, "IsEnabled": enabled});
	}

	function addSeparator(){
		popupMenuModel.append({"Id": "", "Name": "", "Icon": "", "IsEnabled": false});
	}
}


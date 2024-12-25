import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ControlBase {
	id: tabDelegate;

	property bool selected: tabPanel ? model.index === tabPanel.selectedIndex : false;
	property bool firstElement: model.index === 0;
	property bool lastElement: listView ? model.index === listView.count - 1 : false;
	property bool autoWidth: true;
	property bool isCloseEnable: true;

	property string text: model.Name ? model.Name: "";
	property string description: model.Description ? model.Description : "";
	property string icon: model.Icon ? model.Icon : "";
	property int index: model.index;
	property int selectedIndex:  tabPanel ? tabPanel.selectedIndex : -1;

	property int maxWidth: 250;
	property int minWidth: 100;

	property ListView listView: null;
	property Item tabPanel: null;
	property alias mouseArea: ma;

	decorator: Style.tabPanelDecorator

	widthFromDecorator: true;
	signal closeSignal;
	signal startContentLoading();
	signal stopContentLoading();

	Component.onCompleted: {
		if (model.WaitName){
			startContentLoading();
		}
	}

	onTextChanged: {
		if (text !== ""){
			stopContentLoading();
		}
	}

	Connections {
		target: tabDelegate.tabPanel;

		function onStartTabContentLoading(tabId){
			console.log("onStartTabContentLoading", tabId, model.Id);
			if (model.Id === tabId){
				tabDelegate.startContentLoading();
			}
		}

		function onStopTabContentLoading(tabId){
			if (model.Id === tabId){
				tabDelegate.stopContentLoading();
			}
		}
	}

	MouseArea {
		id: ma;
		anchors.fill: tabDelegate;
		cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
		hoverEnabled: true;
		acceptedButtons: Qt.LeftButton | Qt.RightButton;

		onClicked: {
			if (tabDelegate.tabPanel){
				tabDelegate.tabPanel.selectedIndex = model.index;
				tabDelegate.tabPanel.tabClicked(mouse, tabDelegate, model.index);
			}
		}
	}
}



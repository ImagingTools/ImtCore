import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ControlBase {
	id: tabDelegate;

	property bool selected: tabPanel ? model.index === tabPanel.selectedIndex : false;
	property bool firstElement: model.index === 0;
	property bool lastElement: listView ? model.index === listView.count - 1 : false;
	// property bool lastElement: model.index === model.count - 1;

	property bool autoWidth: true;
	property bool isCloseEnable: true;

	property string text: model.name ? model.name: "";
	property string description: model.description ? model.description : "";
	property string icon: model.icon ? model.icon : "";
	property int index: model.index;
	property int selectedIndex:  tabPanel ? tabPanel.selectedIndex : -1;

	property int maxWidth: 250;
	property int minWidth: Style.sizeHintBXS;

	property ListView listView: null;
	property Item tabPanel: null;
	property alias mouseArea: ma;

	decorator: Style.tabPanelDecorator

	widthFromDecorator: true;
	signal closeSignal;
	signal startContentLoading();
	signal stopContentLoading();

	Component.onCompleted: {
		if (model.waitName){
			startContentLoading();
		}
	}

	onTextChanged: {
		if (text !== ""){
			model.waitName = false;
			stopContentLoading();
		}
	}

	Connections {
		target: tabDelegate.tabPanel;

		function onStartTabContentLoading(tabId){
			if (model.id === tabId){
				tabDelegate.startContentLoading();
			}
		}

		function onStopTabContentLoading(tabId){
			if (model.id === tabId){
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



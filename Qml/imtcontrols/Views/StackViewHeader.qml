import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: root

    property string separation: "/"
    property bool initialItemTitleVisible: true

    signal closeClicked()

    ListModel {
        id: headersModel
    }
    
    function addHeader(id, name){
        headersModel.append({id: id, name: name})
    }
    
    function getIndexById(id){
        for (let i = 0; i < headersModel.count; i++){
            if (headersModel.get(i).id === id){
                return i;
            }
        }
        
        return -1;
    }

	function popHeader(){
		if (headersModel.count > 0){
			removeHeader(headersModel.get(headersModel.count - 1).id)
		}
	}
    
    function removeHeader(id){
        for (let i = 0; i < headersModel.count; i++){
            if (headersModel.get(i).id === id){
                headersModel.remove(i)
                return
            }
        }
    }
    
    function setHeaderName(id, name){
        for (let i = 0; i < headersModel.count; i++){
            if (headersModel.get(i).id === id){
                headersModel.setProperty(i, "name", name)
                return;
            }
        }
    }
    
    function getHeaderName(id){
        let index = getIndexById(id);
        if (index >= 0){
            return headersModel.get(index).name;
        }
        
        return "";
    }
    
    Item {
		id: buttonPanel;
		anchors.verticalCenter: parent.verticalCenter
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.leftMargin: visible ? Style.marginM : 0;
		width: visible ? closeButton.width: 0;
		height: headersListView.height;
		visible: headersModel.count > 1;

		ToolButton {
			id: closeButton;
			anchors.centerIn: buttonPanel;
			width: visible ? Style.buttonWidthM: 0;
			height: width;
			iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal);
			onClicked: {
                root.closeClicked()
			}
		}
	}
    
    ListView {
        id: headersListView;
		anchors.verticalCenter: parent.verticalCenter
        anchors.top: parent.top;
        anchors.left: buttonPanel.right;
        anchors.leftMargin: Style.marginM;
		anchors.right: parent.right;
		height: visible ? Style.controlHeightL : 0;
		orientation: ListView.Horizontal;
		boundsBehavior: Flickable.StopAtBounds;
		visible: count === 1 && !root.initialItemTitleVisible ? false : true;
		model: headersModel;
		delegate: Item {
			width: visible ? content.width + spacer.width: 0;
			height: headersListView.height;
			visible: model.index === 0 ? root.initialItemTitleVisible : true;

			Item {
				id: spacer;
				anchors.left: parent.left;
				width: visible ? Style.marginM : 0;
				visible: model.index === 1 && !root.initialItemTitleVisible ? false : true;
			}

			Row {
				id: content;
				anchors.left: spacer.right;
				height: parent.height;
				spacing: Style.marginM;

				Text {
					id: separator;
					anchors.verticalCenter: content.verticalCenter;
					font.pixelSize: Style.fontSizeXXL;
					font.family: Style.fontFamily;
					color: Style.titleColor;
					text: root.separation
					visible: model.index === 1 && !root.initialItemTitleVisible ? false : model.index > 0;
				}

				Text {
					id: headerText;
					anchors.verticalCenter: content.verticalCenter;
					font.pixelSize: Style.fontSizeXXL;
					font.family: Style.fontFamily;
					color: Style.titleColor;
					text: model.name;
				}
			}

			MouseArea {
				anchors.fill: parent;
				onClicked: {}
			}
		}
	}
}

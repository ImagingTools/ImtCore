import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: root

    property string separation: "/"
    property bool initialItemTitleVisible: true

    // Width the headers take without eliding, for a page that places its commands next to them
    readonly property real implicitContentWidth: (buttonPanel.visible ? buttonPanel.width + Style.marginM : 0) + measureRow.implicitWidth

    // A header that does not fit gets an equal share of the width and is elided, instead of running off the view
    readonly property int __shownCount: headersModel.count > 0 && !root.initialItemTitleVisible ? headersModel.count - 1 : headersModel.count
    readonly property real __itemMaxWidth: root.implicitContentWidth <= root.width || root.__shownCount <= 0 ? -1
        : Math.max(Style.sizeHintBXS, (headersListView.width - (root.__shownCount - 1) * (2 * Style.marginM + separatorMeasure.implicitWidth)) / root.__shownCount)

    signal closeClicked()
    signal headerItemClicked(int index, string headerId)

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
		// anchors.leftMargin: visible ? Style.marginM : 0;
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
    
    // The headers laid out as shown, unelided, to measure them; transparent, as a hidden row is not laid out
    Row {
        id: measureRow;
        opacity: 0;
        enabled: false;
        spacing: Style.marginM;

        Repeater {
            model: headersModel;
            delegate: Row {
                spacing: Style.marginM;
                visible: model.index === 0 ? root.initialItemTitleVisible : true;

                Text {
                    font.pixelSize: Style.fontSizeXXL;
                    font.family: Style.fontFamily;
                    text: root.separation;
                    visible: model.index === 1 && !root.initialItemTitleVisible ? false : model.index > 0;
                }

                Text {
                    font.pixelSize: Style.fontSizeXXL;
                    font.family: Style.fontFamily;
                    text: model.name;
                }
            }
        }
    }

    Text {
        id: separatorMeasure;
        opacity: 0;
        font.pixelSize: Style.fontSizeXXL;
        font.family: Style.fontFamily;
        text: root.separation;
    }

    ListView {
        id: headersListView;
		anchors.verticalCenter: parent.verticalCenter
        anchors.top: parent.top;
        anchors.left: buttonPanel.right;
		anchors.leftMargin: buttonPanel.visible ? Style.marginM : 0;
		anchors.right: parent.right;
		height: visible ? Style.controlHeightL : 0;
		orientation: ListView.Horizontal;
		boundsBehavior: Flickable.StopAtBounds;
		clip: true;
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
					visible: model.index === 0 ? false : (model.index === 1 && !root.initialItemTitleVisible ? false : true);
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
					width: root.__itemMaxWidth > 0 ? Math.min(implicitWidth, root.__itemMaxWidth) : implicitWidth;
					elide: Text.ElideRight;
					font.pixelSize: Style.fontSizeXXL;
					font.family: Style.fontFamily;
					font.underline: model.index < headersModel.count - 1 && headerItemMA.containsMouse
					color: model.index < headersModel.count - 1 && headerItemMA.containsMouse ? Style.linkColor : Style.titleColor;
					text: model.name;
				}
			}

			MouseArea {
				id: headerItemMA
				anchors.fill: parent;
				hoverEnabled: true
				cursorShape: model.index < headersModel.count - 1 ? Qt.PointingHandCursor : Qt.ArrowCursor
				onClicked: {
					if (model.index < headersModel.count - 1) {
						root.headerItemClicked(model.index, headersModel.get(model.index).id)
					}
				}
			}
		}
	}
}

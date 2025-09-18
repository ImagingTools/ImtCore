import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0

Rectangle {
	id: container;
	
	color: Style.backgroundColor2;
	
	clip: true;

	property ElementMetaInfo elementMetaInfo: ElementMetaInfo {}
	property int elementHeight: 20;
	property bool contentVisible: true;
	
	property var registeredViewDelegates: ({})
	
	function startLoading(){
		loading.start();
	}
	
	function stopLoading(){
		loading.stop();
	}

	function registerViewDelegate(parameterId, viewComp){
		registeredViewDelegates[parameterId] = viewComp
	}
	
	Component {
		id: metaInfoViewDelegateBaseComp
		MetaInfoTextDelegate {
		}
	}
	
	Flickable {
		id: collectionMetaInfo;
		anchors.fill: parent;
		contentWidth: width;
		contentHeight: column.height;
		boundsBehavior: Flickable.StopAtBounds;
		
		Column {
			id: column;
			anchors.top: parent.top;
			anchors.topMargin: Style.marginM;
			anchors.left: parent.left;
			anchors.leftMargin: Style.marginM;
			anchors.right: parent.right;
			anchors.rightMargin: Style.marginM;
			visible: container.contentVisible;
			spacing: Style.spacingM

			Repeater {
				id: repeaterColumn;
				model: container.elementMetaInfo ? container.elementMetaInfo.m_infoParams : 0
				delegate: Column {
					id: repeaterTitle;
					width: column.width;
					spacing: Style.spacingM
					visible: model.item.m_name !== "" && model.item.m_data.text !== ""

					Text {
						id: nameTitle;
						width: column.width;
						font.pixelSize: Style.fontSizeM;
						font.family: Style.fontFamilyBold;
						font.bold: true;
						color: Style.lightBlueColor;
						elide: Text.ElideRight;
						wrapMode: Text.WrapAnywhere;
						text: model.item.m_name;
					}

					Loader {
						sourceComponent:
							model.item.m_id in container.registeredViewDelegates ?
								container.registeredViewDelegates[model.item.m_id] : metaInfoViewDelegateBaseComp
						width: parent.width
						onLoaded: {
							item.metaInfoView = container
							item.viewData = model.item.m_data
						}
					}
				}
			}
		}//Column main
	}
	
	Rectangle {
		anchors.fill: parent;
		color: parent.color;
		visible: loading.visible;
	}
	
	Loading {
		id: loading;
		anchors.top: parent.top;
		width: parent.width;
		height: Style.sizeHintXXS;
		indicatorSize: Style.controlHeightS;
		color: container.color;
		visible: false;
	}
}


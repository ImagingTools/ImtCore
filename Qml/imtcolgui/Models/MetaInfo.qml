import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0

Rectangle {
	id: container;
	
	color: Style.baseColor;
	
	clip: true;

	property ElementMetaInfo elementMetaInfo: ElementMetaInfo {}
	property int elementHeight: 20;
	property bool contentVisible: true;
	
	property var registeredViewDelegates: ({})

	Component.onCompleted: {
		registerViewDelegate("Date", metaInfoViewDateDelegateComp)
		registerViewDelegate("Time", metaInfoViewDateDelegateComp)
		registerViewDelegate("DateTime", metaInfoViewDateDelegateComp)
		registerViewDelegate("StringList", metaInfoViewStringListDelegateComp)
		registerViewDelegate("ByteArrayList", metaInfoViewStringListDelegateComp)
	}
	
	function startLoading(){
		loading.start();
	}
	
	function stopLoading(){
		loading.stop();
	}

	function registerViewDelegate(parameterTypeId, viewComp){
		registeredViewDelegates[parameterTypeId] = viewComp
	}
	
	Loader{
		id: decaratorLoader
		anchors.fill: parent
		sourceComponent: Style.metaInfoPanelDecorator
		onItemChanged: {
			if(item && item.color !==undefined){
				container.color = item.color
			}
		}
	}

	Component {
		id: metaInfoViewDelegateBaseComp
		MetaInfoTextDelegate {
		}
	}

	Component {
		id: metaInfoViewStringListDelegateComp
		MetaInfoStringListDelegate {
		}
	}

	Component {
		id: metaInfoViewDateDelegateComp

		MetaInfoDateDelegate{
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
			spacing: Style.spacingL

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
						color: Style.textColor;
						elide: Text.ElideRight;
						wrapMode: Text.WrapAnywhere;
						text: model.item.m_name;
					}

					Loader {
						sourceComponent:
							model.item.m_typeId in container.registeredViewDelegates ?
								container.registeredViewDelegates[model.item.m_typeId] : metaInfoViewDelegateBaseComp
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
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginS;
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginS;
		color: parent.color;
		visible: loading.visible;
	}
	
	Loading {
		id: loading;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginS;
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginS;
		height: Style.sizeHintXXS;
		indicatorSize: Style.controlHeightS;
		background.color: container.color;
		visible: false;
	}
}


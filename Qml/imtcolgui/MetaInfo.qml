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
		registerViewDelegate("TextList", metaInfoViewListDelegateComp)
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
		id: metaInfoViewListDelegateComp
		MetaInfoListDelegate {
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
		contentHeight: column.height + 2 * Style.marginM;
		boundsBehavior: Flickable.StopAtBounds;
		
		Column {
			id: column;
			anchors.top: parent.top;
			anchors.topMargin: Style.marginM;
			anchors.left: parent.left;
			anchors.leftMargin: Style.marginM;
			anchors.right: parent.right;
			anchors.rightMargin: Style.marginM + metaInfoScrollbar.secondSize;
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

	CustomScrollbar {
		id: metaInfoScrollbar;
		z: collectionMetaInfo.z + 1;
		anchors.right: parent.right;
		anchors.top: collectionMetaInfo.top;
		anchors.bottom: collectionMetaInfo.bottom;
		secondSize: Style.marginS;
		targetItem: collectionMetaInfo;
		alwaysVisible: false;
	}
	
	// Skeleton in the panel's own layout: a name over its value, row after row.
	Rectangle {
		id: loading;

		property int phase: 0

		anchors.fill: parent;
		color: container.color;
		visible: false;

		// Shown only when the answer is slow; a quick one would just flash the skeleton.
		function start(){
			loading.phase = 0;
			showDelayTimer.restart();
		}

		function stop(){
			showDelayTimer.stop();
			loading.visible = false;
		}

		Timer {
			id: showDelayTimer;
			interval: 300;
			repeat: false;

			onTriggered: {
				loading.visible = true;
			}
		}

		Timer {
			interval: 80;
			repeat: true;
			running: loading.visible;

			onTriggered: {
				loading.phase = (loading.phase + 1) % 16;
			}
		}

		Column {
			id: skeletonColumn;

			anchors.top: parent.top;
			anchors.topMargin: Style.marginM;
			anchors.left: parent.left;
			anchors.leftMargin: Style.marginM;
			anchors.right: parent.right;
			anchors.rightMargin: Style.marginM;
			spacing: Style.spacingL;
			opacity: 0.45 + 0.55 * (loading.phase < 8 ? loading.phase / 8 : (16 - loading.phase) / 8);

			Repeater {
				model: 5;

				delegate: Column {
					width: skeletonColumn.width;
					spacing: Style.spacingM;

					SkeletonBlock {
						width: skeletonColumn.width * (index % 2 === 0 ? 0.35 : 0.5);
						height: Style.fontSizeM;
					}

					SkeletonBlock {
						width: skeletonColumn.width * (index % 3 === 0 ? 0.8 : 0.6);
						height: Style.iconSizeXS;
					}
				}
			}
		}
	}
}


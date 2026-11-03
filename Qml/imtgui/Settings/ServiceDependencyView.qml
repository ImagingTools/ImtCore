import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtappApplicationSdl 1.0

// Read-only row of a service the server depends on: runtime status (green - connected, red - not connected, gray - unknown) and version.
// m_version of the service info holds the text to show, see PageAboutProvider
ParamEditorBase {
	id: serviceDependencyView
	
	typeId: "ServiceDependency"
	readOnly: true
	
	editorModelComp: Component {
		ServiceInfo {}
	}
	
	property ServiceInfo serviceInfo: editorModel
	property string status: serviceDependencyView.serviceInfo ? serviceDependencyView.serviceInfo.m_status : ""
	property string versionText: serviceDependencyView.serviceInfo ? serviceDependencyView.serviceInfo.m_version : ""
	
	function getStatusColor(){
		if (serviceDependencyView.status === "CONNECTED"){
			return Style.successColor
		}
	
		if (serviceDependencyView.status === "DISCONNECTED"){
			return Style.dangerColor
		}
	
		return Style.inactiveTextColor
	}
	
	function getStatusText(){
		if (serviceDependencyView.status === "CONNECTED"){
			return qsTr("Connected")
		}
	
		if (serviceDependencyView.status === "DISCONNECTED"){
			return qsTr("Not connected")
		}
	
		return qsTr("Unknown")
	}
	
	sourceComp: Component {
		ElementView {
			id: elementView
			width: serviceDependencyView.width
			name: serviceDependencyView.name
			controlWidth: Style.sizeHintM
			
			controlComp: Component {
				Item {
					id: statusItem
					width: elementView.controlWidth
					height: Style.controlHeightM
					
					Rectangle {
						id: statusIndicator
						anchors.verticalCenter: statusItem.verticalCenter
						width: Style.iconSizeXS
						height: Style.iconSizeXS
						radius: Style.iconSizeXS / 2
						color: serviceDependencyView.getStatusColor()
					}
					
					BaseText {
						anchors.verticalCenter: statusItem.verticalCenter
						anchors.left: statusIndicator.right
						anchors.leftMargin: Style.spacingS
						anchors.right: statusItem.right
						elide: Text.ElideRight
						text: serviceDependencyView.versionText !== "" ? serviceDependencyView.versionText : serviceDependencyView.getStatusText()
					}
				}
			}
		}
	}
}

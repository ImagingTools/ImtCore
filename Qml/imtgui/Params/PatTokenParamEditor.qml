import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: patTokenEditor
	
	typeId: "PatTokenList"
	
	editorModelComp: Component {
		PatTokenListParam {}
	}
	
	property PatTokenListParam patTokenParam: editorModel
	
	sourceComp: Component {
		Item {
			id: tokenEditorView
			width: patTokenEditor.width
			height: Math.max(Style.sizeHintS, contentColumn.height)
			
			Column {
				id: contentColumn
				width: parent.width
				spacing: Style.marginM

				Item {
					width: contentColumn.width
					height: titleColumn.height
					Column {
						id: titleColumn
						width: parent.width
						spacing: Style.marginXS
						
						Text {
							text: patTokenEditor.name
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}
						
						Text {
							text: patTokenEditor.description
							font.pixelSize: Style.fontSizeS
							color: Style.hintTextColor
							visible: patTokenEditor.description !== ""
							wrapMode: Text.WordWrap
							width: parent.width
						}
					}

					Button {
						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter
						width: Style.sizeHintBXS
						height: Style.controlHeightM
						text: qsTr("New Token")
						onClicked: {
							ModalDialogManager.openDialog(addTokenDialogComp, {})
						}
					}
				}
				
				// Token list
				Rectangle {
					width: parent.width
					height: Math.max(Style.sizeHintXS, tokenListView.contentHeight + Style.marginM)
					color: Style.backgroundColor
					border.color: Style.borderColor
					border.width: 1
					radius: Style.radiusS
					
					ListView {
						id: tokenListView
						anchors.fill: parent
						anchors.margins: Style.marginS
						clip: true
						spacing: Style.marginS
						
						model: patTokenEditor.patTokenParam ? patTokenEditor.patTokenParam.tokens : null
						
						delegate: ElementView {
							id: delegateRoot
							width: tokenListView.width
							name: model.name
							description: model.description

							controlComp: Component {
								Button {
									anchors.right: parent.right
									anchors.verticalCenter: parent.verticalCenter
									width: Style.sizeHintBXS
									height: Style.controlHeightM
									text: qsTr("Delete")
									onClicked: {
										patTokenEditor.patTokenParam.removeToken(model.index)
									}
								}
							}
						}
						
						Text {
							anchors.centerIn: parent
							visible: tokenListView.count === 0
							text: qsTr("No PAT tokens configured")
							font.pixelSize: Style.fontSizeM
							color: Style.hintTextColor
						}
					}
				}
			}
			
			// Add/Edit Token Dialog
			Component {
				id: addTokenDialogComp
				
				Dialog {
					id: addDialog
					title: qsTr("Add PAT Token")
					width: Style.sizeHintXL
					
					property string tokenName: ""
					property string tokenValue: ""
					property string tokenDescription: ""
					
					Component.onCompleted: {
						clearButtons()
						addButton(Enums.apply, qsTr("Add"), true)
						addButton(Enums.cancel, qsTr("Cancel"), true)
					}
					
					onFinished: {
						if (buttonId === Enums.apply){
							if (tokenName.trim() !== "" && tokenValue.trim() !== ""){
								if (patTokenEditor.patTokenParam) {
									patTokenEditor.patTokenParam.addToken(
										tokenName.trim(),
										tokenValue.trim(),
										tokenDescription.trim()
									)
								}
							}
							else{
								ModalDialogManager.showInfoDialog(qsTr("Name and Token fields are required"))
							}
						}
					}
					
					contentComp: Component {
						GroupElementView {
							width: addDialog.width - Style.marginL
							TextInputElementView {
								width: parent.width
								name: qsTr("Token Name")
								description: qsTr("A descriptive name for this token")
								text: addDialog.tokenName
								onEditingFinished: addDialog.tokenName = text
							}
							
							TextInputElementView {
								width: parent.width
								name: qsTr("Token Value")
								description: qsTr("The actual PAT token string")
								text: addDialog.tokenValue
								echoMode: TextInput.Password
								onEditingFinished: addDialog.tokenValue = text
							}
							
							TextInputElementView {
								width: parent.width
								name: qsTr("Description")
								description: qsTr("Optional description for this token")
								text: addDialog.tokenDescription
								onEditingFinished: addDialog.tokenDescription = text
							}
						}
					}
				}
			}
		}
	}
}

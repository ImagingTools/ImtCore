import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import controlsgalleryContactInfosSdl 1.0
import controlsgalleryContactInfoCollectionDocumentManagerSdl 1.0
import imtbaseCollectionDocumentManagerSdl 1.0
import imtbaseUndoManagerSdl 1.0

ViewBase {
	id: root
	
	anchors.fill: parent

	property ContactInfoData contactInfoData: model
	function updateGui(){
		firstNameInput.text = contactInfoData.m_firstName
		lastNameInput.text = contactInfoData.m_lastName

		genderTypeCB.currentIndex = -1
		if (genderTypeCB.model){
			for (let i = 0; i < genderTypeCB.model.getItemsCount(); i++){
				let genderTypeId = genderTypeCB.model.getData("id", i)
				if (contactInfoData.m_genderType === genderTypeId){
					genderTypeCB.currentIndex = i
					break
				}
			}
		}
		
		// Default - MALE
		if (genderTypeCB.currentIndex < 0){
			genderTypeCB.currentIndex = 0
		}
	}
	
	function updateModel(){
		contactInfoData.m_firstName = firstNameInput.text
		contactInfoData.m_lastName = lastNameInput.text

		if (genderTypeCB.currentIndex < 0){
			contactInfoData.m_genderType = "MALE"
		}

		if (genderTypeCB.model && genderTypeCB.currentIndex >= 0){
			contactInfoData.m_genderType = genderTypeCB.model.getData("id", genderTypeCB.currentIndex)
		}
	}

	CustomScrollbar {
		id: scrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: flickable.top
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		targetItem: flickable
		visible: root.visible
	}
	
	CustomScrollbar{
		id: scrollHoriz
		z: parent.z + 1
		anchors.left: flickable.left
		anchors.right: flickable.right
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		vertical: false
		targetItem: flickable
	}

	Flickable {
		id: flickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: scrollbar.left
		anchors.rightMargin: Style.marginXL
		contentWidth: bodyColumn.width
		contentHeight: Math.max(bodyColumn.height + 2 * Style.marginXL + 100)
		boundsBehavior: Flickable.StopAtBounds
		clip: true
		
		Column {
			id: bodyColumn
			width: 700
			spacing: Style.marginXL

			TextInputElementView {
				id: firstNameInput
				
				name: qsTr("First Name")
				placeHolderText: qsTr("Enter the first name")
				textInputValidator: notEmptyRegexp
				showErrorWhenInvalid: true
			
				onEditingFinished: {
					root.doUpdateModel()
				}
			}
			
			RegularExpressionValidator {
				id: notEmptyRegexp
				regularExpression: /^(?!\s*$).+/;
			}

			TextInputElementView {
				id: lastNameInput
				
				name: qsTr("Last Name")
				placeHolderText: qsTr("Enter the last name")
				textInputValidator: notEmptyRegexp
				showErrorWhenInvalid: true

				onEditingFinished: {
					root.doUpdateModel()
				}
			}

			ComboBoxElementView {
				id: genderTypeCB
				name: qsTr("Gender")
				currentIndex: 0
				model: TreeItemModel {
					Component.onCompleted: {
						let index = insertNewItem()
						setData("id", "MALE", index)
						setData("name", "Male", index)
						
						index = insertNewItem()
						setData("id", "FEMALE", index)
						setData("name", "Female", index)
						
						index = insertNewItem()
						setData("id", "DIVERSE", index)
						setData("name", "Diverse", index)
					}
				}

				onCurrentIndexChanged: {
					root.doUpdateModel()
				}
			}
		}//Body column
	}//Flickable
}// Account Editor container

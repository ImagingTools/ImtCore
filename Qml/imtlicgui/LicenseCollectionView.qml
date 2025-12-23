import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtlicLicensesSdl 1.0

RemoteCollectionView {
	id: root;
	
	visibleMetaInfo: false;
	
	collectionId: "Licenses";
	documentCollectionFilter: null
	
	commandsDelegateComp: Component {DocumentCollectionViewDelegate {
			collectionView: root;
			documentTypeIds: ["License"]
			documentViewTypeIds: ["LicenseEditor"]
			documentViewsComp: [featureDocumentComp]
			documentDataControllersComp: [dataControllerComp]
		}
	}
	
	Component.onCompleted: {
		collectionFilter.setSortingInfo(LicenseItemTypeMetaInfo.s_lastModified, "DESC");
	}
	
	Component {
		id: featureDocumentComp;
		
		LicenseEditor {
			id: licenceEditor;
			
			commandsDelegateComp: Component {ViewCommandsDelegateBase {
					view: licenceEditor;
				}
			}
			
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "License";
				}
			}
		}
	}
	
	Component {
		id: dataControllerComp;
		
		GqlRequestDocumentDataController {
			id: requestDocumentDataController
			
			property LicenseDefinitionData licenseData: documentModel
			
			gqlGetCommandId: ImtlicLicensesSdlCommandIds.s_licenseItem;
			gqlUpdateCommandId: ImtlicLicensesSdlCommandIds.s_licenseUpdate;
			gqlAddCommandId: ImtlicLicensesSdlCommandIds.s_licenseAdd;
			
			typeId: "License";
			documentName: licenseData ? licenseData.m_licenseName: "";
			documentDescription: licenseData ? licenseData.m_description: "";
			
			documentModelComp: Component {
				LicenseDefinitionData {}
			}
		}
	}
}

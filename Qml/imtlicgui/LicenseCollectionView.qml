import QtQuick 2.12
import Acf 1.0
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

    commandsDelegateComp: Component {DocumentCollectionViewDelegate {
            collectionView: root;
			documentTypeIds: ["License"]
			documentViewTypeIds: ["LicenseEditor"]
			documentViewsComp: [featureDocumentComp]
			documentDataControllersComp: [dataControllerComp]
        }
    }

    Component.onCompleted: {
        collectionFilter.setSortingOrder("DESC");
        collectionFilter.setSortingInfoId(LicenseItemTypeMetaInfo.s_lastModified);
    }

    Component {
        id: featureDocumentComp;

        LicenseEditor {
            id: licenceEditor;

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                    view: licenceEditor;
                }
            }

            commandsControllerComp: Component {CommandsPanelController {
                    commandId: "License";
                    uuid: licenceEditor.viewId;
                    commandsView: licenceEditor.commandsView;
                }
            }
        }
    }

    Component {
        id: dataControllerComp;

        GqlRequestDocumentDataController {
            id: requestDocumentDataController

            gqlGetCommandId: ImtlicLicensesSdlCommandIds.s_licenseItem;
            gqlUpdateCommandId: ImtlicLicensesSdlCommandIds.s_licenseUpdate;
            gqlAddCommandId: ImtlicLicensesSdlCommandIds.s_licenseAdd;

            documentModelComp: Component {
                LicenseDefinitionData {}
            }

            payloadModel: LicenseDataPayload {
                onFinished: {
                    requestDocumentDataController.documentModel = m_licenseDefinitionData;
                }
            }
        }
    }
}

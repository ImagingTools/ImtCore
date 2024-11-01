import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtdocgui 1.0

DocumentDataController {
    id: container;

    property string gqlGetCommandId;
    property string gqlAddCommandId;
    property string gqlUpdateCommandId;

    property Component getInputObjectComp;
    property Component updateInputObjectComp;
    property Component addInputObjectComp;

    function updateDocumentModel(){
        getRequest.send();
    }

    function saveDocument(){
        setRequest.send();
    }

    function createDocumentModel(){

    }

    GqlSdlRequestSender {
        id: getRequest;
        gqlCommandId: container.gqlGetCommandId;
        sdlObjectComp: Component { ProfileData {}}
        inputObjectComp: container.getInputObjectComp;
        onFinished: {
            container.model = sdlObject;
        }
    }

    GqlSdlRequestSender {
        id: setRequest;
        gqlCommandId: container.gqlUpdateCommandId;
        sdlObjectComp: Component { SetProfileResponse {}}
        inputObjectComp: container.updateInputObjectComp;

        onFinished: {
            container.model = sdlObject;
        }
    }
}


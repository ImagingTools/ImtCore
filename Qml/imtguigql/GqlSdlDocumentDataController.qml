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
    }

    function saveDocument(){
    }

    function createDocumentModel(){
    }
}


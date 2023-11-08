const { QtObject } = require('./QtObject')
const { QVar, QBool, QString, QReal } = require('../utils/properties')
const { Qt } = require('../utils/Qt')

class Shortcut extends QtObject {
    static defaultProperties = {
        context: { type: QReal, value: Qt.WindowShortcut },
        autoRepeat: { type: QBool, value: true },
        enabled: { type: QBool, value: true },
        nativeText: { type: QString, value: '' },
        portableText: { type: QString, value: '' },
        sequence: { type: QString, value: '' },
        sequences: { type: QVar, value: [] },

    }

    static defaultSignals = {
        activated: { params: [] },
        activatedAmbiguously: { params: [] },
    }

    constructor(parent){
        super(parent)
    }


    $destroy(){

        super.$destroy()
    }
    
}

module.exports.Shortcut = Shortcut
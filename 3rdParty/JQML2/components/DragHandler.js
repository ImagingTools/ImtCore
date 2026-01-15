const { Item } = require('./Item')
const { QSignal } = require('../utils/signal')
const { Qt } = require('../utils/Qt')
const { QReal, QBool, QString, QInt, QPoint, QVar } = require('../utils/properties')

class DragHandler extends Item {
    static defaultProperties = {
        acceptedButtons: { type: QInt, value: Qt.LeftButton },
        acceptedDevices: { type: QInt, value: Qt.LeftButton },
        acceptedModifiers: { type: QInt, value: Qt.LeftButton },
        acceptedPointerTypes: { type: QInt, value: Qt.LeftButton },
        active: { type: QBool, value: true },
        activeTranslation: { type: QReal, value: 0 },
        blocking: { type: QBool, value: false },
        cursorShape: { type: QString, value: 'default' },
        dragThreshold: { type: QInt, value: 0 },
        grabPermissions: { type: QInt, value: 0 },
        snapMode: { type: QInt, value: 0 },
        margin: { type: QReal, value: 0 },
        orientation: { type: QReal, value: 0 },
        target: { type: QVar, value: undefined },
        translation: { type: QVar, value: undefined },
        persistentTranslation: { type: QVar, value: undefined },

    }

    static defaultSignals = {
        canceled: { params: ['point'] },
		grabChanged: { params: ['transition', 'point'] },

    }
}

module.exports.DragHandler = DragHandler
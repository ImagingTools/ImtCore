const QtObject = require("../QtQml/QtObject")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Bool = require("../QtQml/Bool")
const Signal = require("../QtQml/Signal")


class QWebSocket extends QtObject {
    static Connecting = 0
    static Open = 1
    static Closing = 2
    static Closed = 3
    static Error = 4

    static meta = Object.assign({}, QtObject.meta, {
        status: { type: Real, value: QWebSocket.Closed, signalName: 'statusChanged' },
        url: { type: String, value: '', signalName: 'urlChanged' },
        active: { type: Bool, value: false, signalName: 'activeChanged' },
        errorString: { type: String, value: '', signalName: 'errorStringChanged' },
        negotiatedSubprotocol: { type: String, value: '', signalName: 'negotiatedSubprotocolChanged' },
        requestedSubprotocols: { type: Var, value: undefined, signalName: 'requestedSubprotocolsChanged' },

        statusChanged: {type:Signal, slotName:'onStatusChanged', args:[]},
        urlChanged: {type:Signal, slotName:'onUrlChanged', args:[]},
        activeChanged: {type:Signal, slotName:'onActiveChanged', args:[]},
        errorStringChanged: {type:Signal, slotName:'onErrorStringChanged', args:[]},
        negotiatedSubprotocolChanged: {type:Signal, slotName:'onNegotiatedSubprotocolChanged', args:[]},
        requestedSubprotocolsChanged: {type:Signal, slotName:'onRequestedSubprotocolsChanged', args:[]},

        triggered: {type:Signal, slotName:'onTriggered', args:[]},
        binaryMessageReceived: {type:Signal, slotName:'onBinaryMessageReceived', args:['message']},
        textMessageReceived: {type:Signal, slotName:'onTextMessageReceived', args:['message']},
    })

    SLOT_statusChanged() {
        if (this.status !== QWebSocket.Error) {
            this.errorString = ''
        }
    }

    SLOT_urlChanged(oldValue, newValue){
        this.__reconnectSocket()
    }

    SLOT_activeChanged(oldValue, newValue){
        this.__reconnectSocket()
    }

    connectSocket() {
        this.__reconnect = false

        if (!this.url || !this.active) {
            return
        }

        this.status = QWebSocket.Connecting
        this.__socket = new WebSocket(this.url)
        this.__socket.onopen = () => {
            this.status = QWebSocket.Open
        }
        this.__socket.onclose = () => {
            this.status = QWebSocket.Closed
            if (this.__reconnect) {
                this.connectSocket()
            }
        }
        this.__socket.onerror = error => {
            this.errorString = error.message
            this.status = QWebSocket.Error
        }
        this.__socket.onmessage = message => {
            this.textMessageReceived(message.data)
        }
    }
    __reconnectSocket() {
        this.__reconnect = true
        if (this.status === QWebSocket.Open) {
            this.status = QWebSocket.Closing
            this.__socket.close()
            delete this.__socket
        } else if (this.status !== QWebSocket.Closing) {
            this.connectSocket()
        }
    }
    sendTextMessage(message) {
        if (this.status === QWebSocket.Open) {
            this.__socket.send(message)
        }
    }
    sendBinaryMessage(message) {
        if (this.status === QWebSocket.Open) {
            this.__socket.send(message)
        }
    }

    __destroy(){
        this.__socket.close()
        
        super.__destroy()
    }
}



module.exports = QWebSocket
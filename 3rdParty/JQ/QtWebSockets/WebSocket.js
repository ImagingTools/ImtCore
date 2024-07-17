const QtObject = require("../QtQml/QtObject")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Bool = require("../QtQml/Bool")
const Signal = require("../QtQml/Signal")

class WebSocket extends QtObject {
    static Connecting = 0
    static Open = 1
    static Closing = 2
    static Closed = 3
    static Error = 4

    static meta = Object.assign({}, QtObject.meta, {
        status: { type: Real, value: WebSocket.Closed, signalName: 'statusChanged' },
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

    $statusChanged() {
        if (this.getPropertyValue('status') !== WebSocket.Error) {
            this.getProperty('errorString').reset('')
        }
    }
    connectSocket() {
        this.$reconnect = false

        if (!this.getPropertyValue('url') || !this.getPropertyValue('active')) {
            return
        }

        this.getProperty('status').reset(WebSocket.Connecting)
        this.$socket = new OriginWebSocket(this.getPropertyValue('url'))
        this.$socket.onopen = () => {
            this.getProperty('status').reset(WebSocket.Open)
        }
        this.$socket.onclose = () => {
            this.getProperty('status').reset(WebSocket.Closed)
            if (this.$reconnect) {
                this.connectSocket()
            }
        }
        this.$socket.onerror = error => {
            this.getProperty('errorString').reset(error.message)
            this.getProperty('status').reset(WebSocket.Error)
        }
        this.$socket.onmessage = message => {
            this.textMessageReceived(message.data)
        }
    }
    $reconnectSocket() {
        this.$reconnect = true
        if (this.getPropertyValue('status') === WebSocket.Open) {
            this.getProperty('status').reset(WebSocket.Closing)
            this.$socket.close()
            delete this.$socket
        } else if (this.getPropertyValue('status') !== WebSocket.Closing) {
            this.connectSocket()
        }
    }
    sendTextMessage(message) {
        if (this.getPropertyValue('status') === WebSocket.Open) {
            this.$socket.send(message)
        }
    }
    sendBinaryMessage(message) {
        if (this.getPropertyValue('status') === WebSocket.Open) {
            this.$socket.send(message)
        }
    }
}

module.exports = WebSocket
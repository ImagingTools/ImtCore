const { QtObject } = require('./QtObject')
const { QReal, QBool, QString, QVar } = require('../utils/properties')

class WebSocket extends QtObject {
    static Connecting = 0
    static Open = 1
    static Closing = 2
    static Closed = 3
    static Error = 4

    static defaultProperties = {
        status: { type: QReal, value: WebSocket.Closed, changed: '$statusChanged' },
        url: { type: QString, value: '', changed: '$reconnectSocket' },
        active: { type: QBool, value: false, changed: '$reconnectSocket' },
        errorString: { type: QString, value: '' },
        negotiatedSubprotocol: { type: QString, value: '' },
        requestedSubprotocols: { type: QVar, value: [] },

    }

    static defaultSignals = {
        triggered: { params: [] },
        binaryMessageReceived: { params: ['message'] },
        textMessageReceived: { params: ['message'] },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)

        this.reconnect = false
    }

    $statusChanged() {
        if (this.getPropertyValue('status') !== WebSocket.Error) {
            this.getProperty('errorString').reset('')
        }
    }
    connectSocket() {
        this.reconnect = false

        if (!this.getPropertyValue('url') || !this.getPropertyValue('active')) {
            return
        }

        this.getProperty('status').reset(WebSocket.Connecting)
        this.$socket = new _WebSocket(this.getPropertyValue('url'))
        this.$socket.onopen = () => {
            this.getProperty('status').reset(WebSocket.Open)
        }
        this.$socket.onclose = () => {
            this.getProperty('status').reset(WebSocket.Closed)
            if (this.reconnect) {
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
        this.reconnect = true
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

    $destroy() {
        if(this.$socket){
            this.$socket.close()
            delete this.$socket
        }
        super.$destroy()
    }
    
}

module.exports.WebSocket = WebSocket
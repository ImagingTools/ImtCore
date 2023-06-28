import { QtObject } from './QtObject'

export class WebSocket extends QtObject {

    static Connecting = 0
    static Open = 1
    static Closing = 2
    static Closed = 3
    static Error = 4

    constructor(args) {
        super(args)
        this.$cP('url', '').connect(this.$reconnectSocket.bind(this))
        this.$cP('active', false).connect(this.$reconnectSocket.bind(this))
        this.$cP('errorString', '')
        this.$cP('negotiatedSubprotocol', '')
        this.$cP('requestedSubprotocols', [])
        this.$cP('status', WebSocket.Closed).connect(this.$onStatusChanged.bind(this))

        this.$cS('binaryMessageReceived', 'message')
        this.$cS('textMessageReceived', 'message')

        this.$socket = undefined
        this.$reconnect = false
    }

    $onStatusChanged() {
        if (this.status !== WebSocket.Error) {
            this.errorString = ""
        }
    }
    $connectSocket() {
        this.$reconnect = false

        if (!this.url || !this.active) {
            return
        }

        this.status = WebSocket.Connecting
        this.$socket = new window.WebSocket(this.url)
        this.$socket.onopen = () => {
            this.status = WebSocket.Open
        }
        this.$socket.onclose = () => {
            this.status = WebSocket.Closed
            if (this.$reconnect) {
                this.$connectSocket()
            }
        }
        this.$socket.onerror = error => {
            this.errorString = error.message
            this.status = WebSocket.Error
        }
        this.$socket.onmessage = message => {
            this.textMessageReceived(message.data)
        }
    }
    $reconnectSocket() {
        this.$reconnect = true
        if (this.status === WebSocket.Open) {
            this.status = WebSocket.Closing
            this.$socket.close()
            this.$socket = undefined
        } else if (this.status !== WebSocket.Closing) {
            this.$connectSocket()
        }
    }
    sendTextMessage(message) {
        if (this.status === WebSocket.Open) {
            this.$socket.send(message)
        }
    }
    sendBinaryMessage(message) {
        if (this.status === WebSocket.Open) {
            this.$socket.send(message)
        }
    }

    $destroy() {
        if(this.$socket){
            this.$socket.close()
            this.$socket = undefined
        }
        super.$destroy()
    }
}

QML.WebSocket = WebSocket
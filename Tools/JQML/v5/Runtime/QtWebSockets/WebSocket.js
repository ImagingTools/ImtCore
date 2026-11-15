const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

// ---- WebSocket (QtWebSockets) --------------------------------------------------------------------

// QQmlWebSocket: open when active and url are set, reopened on a url change, status follows the socket
class QmlWebSocket extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$socket = null
        engine.scheduleComponentComplete(this)
    }

    $componentComplete(){
        super.$componentComplete()
        this.$open()
    }

    $setStatus(status){
        if(status !== QmlWebSocket.Error) this.$write(QmlWebSocket.ERRORSTRING, '')
        this.$write(QmlWebSocket.STATUS, status)
    }

    $close(){
        let socket = this.$socket
        if(!socket) return
        this.$socket = null
        socket.onopen = socket.onclose = socket.onerror = socket.onmessage = null
        try { socket.close() } catch(error) {}
    }

    $open(){
        this.$close()
        let url = this.$v[QmlWebSocket.URL]
        if(!(this.$f & engine.COMPLETED) || !this.$v[QmlWebSocket.ACTIVE] || !url || typeof WebSocket === 'undefined'){
            if(this.$v[QmlWebSocket.STATUS] !== QmlWebSocket.Closed && !this.$v[QmlWebSocket.ACTIVE]) this.$setStatus(QmlWebSocket.Closed)
            return
        }
        let socket
        try {
            let protocols = this.$v[QmlWebSocket.REQUESTEDSUBPROTOCOLS]
            socket = Array.isArray(protocols) && protocols.length ? new WebSocket(String(url), protocols) : new WebSocket(String(url))
        } catch(error) {
            this.$write(QmlWebSocket.ERRORSTRING, String(error.message || error))
            this.$write(QmlWebSocket.STATUS, QmlWebSocket.Error)
            return
        }
        this.$socket = socket
        this.$setStatus(QmlWebSocket.Connecting)
        let alive = () => this.$socket === socket && !(this.$f & engine.DESTROYED) && !engine.isStopped()
        socket.onopen = () => { if(alive()) engine.batch(() => {
            this.$write(QmlWebSocket.NEGOTIATEDSUBPROTOCOL, socket.protocol || '')
            this.$setStatus(QmlWebSocket.Open)
        }) }
        socket.onclose = () => { if(alive()) engine.batch(() => {
            this.$socket = null
            this.$setStatus(QmlWebSocket.Closed)
        }) }
        socket.onerror = () => { if(alive()) engine.batch(() => {
            this.$write(QmlWebSocket.ERRORSTRING, 'WebSocket error')
            this.$write(QmlWebSocket.STATUS, QmlWebSocket.Error)
        }) }
        socket.onmessage = (event) => { if(alive()) engine.batch(() => {
            if(typeof event.data === 'string') engine.emitArgs(this, QmlWebSocket.SIGNAL_textMessageReceived, [event.data])
            else engine.emitArgs(this, QmlWebSocket.SIGNAL_binaryMessageReceived, [event.data])
        }) }
    }

    sendTextMessage(message){
        if(this.$socket && this.$v[QmlWebSocket.STATUS] === QmlWebSocket.Open){
            this.$socket.send(String(message))
            return String(message).length
        }
        return 0
    }

    sendBinaryMessage(message){
        if(this.$socket && this.$v[QmlWebSocket.STATUS] === QmlWebSocket.Open){
            this.$socket.send(message)
            return message.byteLength || 0
        }
        return 0
    }

    $disposeNative(){
        this.$close()
        super.$disposeNative()
    }
}
const reopen = (socket) => { if(socket.$f & engine.COMPLETED) socket.$open() }
defineType(QmlWebSocket, {
    props: {
        url: { kind: 'string', effect: reopen },
        active: { kind: 'bool', effect: reopen },
        status: { kind: 'int', initial: 3, readonly: true },
        errorString: { kind: 'string', readonly: true },
        requestedSubprotocols: { kind: 'var', initial: [] },
        negotiatedSubprotocol: { kind: 'string', readonly: true },
    },
    signals: ['textMessageReceived(message)', 'binaryMessageReceived(message)'],
})
Object.assign(QmlWebSocket, { Connecting: 0, Open: 1, Closing: 2, Closed: 3, Error: 4 })

module.exports = { QmlWebSocket }

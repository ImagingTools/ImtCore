class Emitter {
    constructor() {
        this.listeners = []
    }

    event(listener) {
        this.listeners.push(listener)
        return {
            dispose: () => {
                this.listeners = this.listeners.filter(item => item !== listener)
            },
        }
    }

    fire(message) {
        for (const listener of this.listeners.slice()) listener(message)
    }

    dispose() {
        this.listeners = []
    }
}

class JqmlDebugSession {
    constructor() {
        this.emitter = new Emitter()
        this.onDidSendMessage = this.emitter.event.bind(this.emitter)
        this.seq = 1
    }

    handleMessage(message) {
        if (!message || message.type !== 'request') return
        const command = message.command
        if (command === 'initialize') {
            this.reply(message, {
                supportsConfigurationDoneRequest: true,
                supportsSetVariable: false,
            })
            this.event('initialized')
            return
        }
        if (command === 'setBreakpoints') {
            const lines = (message.arguments && message.arguments.breakpoints) || []
            this.reply(message, {
                breakpoints: lines.map((item, index) => ({
                    id: index + 1,
                    verified: true,
                    line: item.line,
                    column: item.column,
                })),
            })
            return
        }
        if (command === 'threads') {
            this.reply(message, { threads: [{ id: 1, name: 'JQML' }] })
            return
        }
        this.reply(message, {})
    }

    reply(request, body) {
        this.send({
            type: 'response',
            request_seq: request.seq,
            success: true,
            command: request.command,
            body: body || {},
        })
    }

    event(name) {
        this.send({ type: 'event', event: name })
    }

    send(message) {
        message.seq = this.seq++
        this.emitter.fire(message)
    }

    dispose() {
        this.emitter.dispose()
    }
}

function writeMessage(message) {
    const json = Buffer.from(JSON.stringify(message), 'utf8')
    process.stdout.write('Content-Length: ' + json.length + '\r\n\r\n')
    process.stdout.write(json)
}

function readMessages(onMessage) {
    let buffer = Buffer.alloc(0)
    process.stdin.on('data', chunk => {
        buffer = Buffer.concat([buffer, chunk])
        while (true) {
            const headerEnd = buffer.indexOf('\r\n\r\n')
            if (headerEnd < 0) return
            const header = buffer.slice(0, headerEnd).toString('utf8')
            const match = header.match(/Content-Length:\s*(\d+)/i)
            if (!match) {
                buffer = buffer.slice(headerEnd + 4)
                continue
            }
            const length = Number(match[1])
            const start = headerEnd + 4
            if (buffer.length < start + length) return
            const body = buffer.slice(start, start + length).toString('utf8')
            buffer = buffer.slice(start + length)
            onMessage(JSON.parse(body))
        }
    })
}

if (require.main === module) {
    const session = new JqmlDebugSession()
    session.onDidSendMessage(writeMessage)
    readMessages(message => session.handleMessage(message))
}

module.exports = {
    JqmlDebugSession,
}

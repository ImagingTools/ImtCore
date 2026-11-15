const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

// ---- Process (ImtCore): no processes in a browser --------------------------------------------------

class Process extends QtObject {
    start(){ engine.queueCall(() => engine.emitArgs(this, Process.SIGNAL_finished, [-1, 1])) }
    setEnviroment(){}
    setEnvironment(){}
}
defineType(Process, { signals: ['finished(exitCode, exitStatus)', 'readyReadStandardOutput(data)', 'readyReadStandardError(data)'] })

module.exports = { Process }

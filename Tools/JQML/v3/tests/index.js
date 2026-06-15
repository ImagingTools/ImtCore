const { spawn, spawnSync } = require('child_process')
const fs = require('fs')
const os = require('os')
const { Builder, Capabilities, By, until } = require('selenium-webdriver')
const chrome = require('selenium-webdriver/chrome')
const path = require('path')
const chromedriverModule = require('chromedriver')
const chromedriverPath = (() => {
    const p = chromedriverModule.path
    if (fs.existsSync(p)) return p
    const globalBin = process.platform === 'win32' ? 'chromedriver.exe' : 'chromedriver'
    return require('child_process').execSync(`which ${globalBin}`).toString().trim()
})()

const colors = {
    reset: "\x1b[0m",
    red: "\x1b[31m",
    green: "\x1b[32m",
    yellow: "\x1b[33m",
    blue: "\x1b[34m",
    magenta: "\x1b[35m",
    cyan: "\x1b[36m",
    white: "\x1b[37m",
    gray: "\x1b[90m"
}

let compiler = null

function getCompiler() {
    if (compiler) return compiler

    const originalArgv = process.argv

    try {
        process.argv = [originalArgv[0], originalArgv[1]]
        compiler = require('../compiler/compiler')
        return compiler
    } finally {
        process.argv = originalArgv
    }
}

function getQmlLog(fullLogs) {
    const regex = /^qml:\s+(?!warning:)(.+)$/gm
    let matches = []
    let match

    while ((match = regex.exec(fullLogs)) !== null) {
        matches.push(match[1])
    }

    return matches
}

function getQmlDiagnostics(fullLogs) {
    if (!fullLogs) return []

    const rawLines = fullLogs
        .split(/\r?\n/g)
        .map(line => line.trim())
        .filter(Boolean)

    return rawLines.filter(line =>
        line.includes('QQmlApplicationEngine failed to load component') ||
        line.includes('Did not load any objects, exiting.') ||
        line.startsWith('file:///') ||
        line.includes(' is not installed') ||
        line.includes(' unavailable') ||
        line.includes(': error:')
    )
}

function getErrorMessage(error) {
    if (!error) return 'Unknown error'
    if (error.message) return error.message
    return String(error)
}

function envFill(source) {
    let result = String(source)
    for (const key of Object.keys(process.env)) {
        const value = (process.env[key] || '').replaceAll('\\', '\\\\').trim()
        result = result.replaceAll('${' + key + '}', value)
    }
    return result
}

function ensureDefaultTargetName() {
    if (!process.env.IMTCOREDIR_BUILD && process.env.IMTCOREDIR) {
        process.env.IMTCOREDIR_BUILD = process.env.IMTCOREDIR
    }

    if (process.env.TARGETNAME && process.env.TARGETNAME.trim()) return

    const roots = [process.env.IMTCOREDIR_BUILD, process.env.IMTCOREDIR, process.env.PROLIFEDIR].filter(Boolean)
    for (const root of roots) {
        const auxIncludePath = path.resolve(root, 'AuxInclude')
        if (!fs.existsSync(auxIncludePath) || !checkIsDirectorySync(auxIncludePath)) continue

        const targets = fs.readdirSync(auxIncludePath)
            .map(name => path.resolve(auxIncludePath, name))
            .filter(p => checkIsDirectorySync(p))
            .map(p => path.basename(p))

        if (targets.length > 0) {
            process.env.TARGETNAME = targets[0]
            return
        }
    }
}

function runCommandSync(command, args, cwd) {
    const result = spawnSync(command, args, {
        cwd,
        encoding: 'utf-8',
        windowsHide: true,
    })

    const stdout = result.stdout ? String(result.stdout) : ''
    const stderr = result.stderr ? String(result.stderr) : ''

    if (result.error) {
        return {
            ok: false,
            code: -1,
            output: `${stdout}\n${stderr}\n${result.error.message}`,
        }
    }

    return {
        ok: result.status === 0,
        code: result.status,
        output: `${stdout}\n${stderr}`,
    }
}

function runCmakeWithOptionalVsDev(cmakeArgs, cwd, vsDevCmdPath) {
    if (!vsDevCmdPath) {
        return runCommandSync('cmake', cmakeArgs, cwd)
    }

    const quoteForCmd = (value) => {
        const str = String(value)
            .replaceAll('%', '%%')
        return `"${str.replaceAll('"', '""')}"`
    }

    const scriptDir = fs.mkdtempSync(path.resolve(os.tmpdir(), 'jqml_vsdev_'))
    const scriptPath = path.resolve(scriptDir, 'run.cmd')

    const scriptLines = [
        '@echo off',
        `call ${quoteForCmd(vsDevCmdPath)} -no_logo -arch=x64 -host_arch=x64`,
        'if errorlevel 1 exit /b %errorlevel%',
        `cmake ${cmakeArgs.map(quoteForCmd).join(' ')}`,
        'exit /b %errorlevel%',
        '',
    ]

    fs.writeFileSync(scriptPath, scriptLines.join('\r\n'), 'utf-8')

    const result = spawnSync('cmd.exe', ['/d', '/v:off', '/c', scriptPath], {
        cwd,
        encoding: 'utf-8',
        windowsHide: true,
    })

    try {
        fs.rmSync(scriptDir, { recursive: true, force: true })
    } catch (e) {
        // ignore cleanup errors
    }

    const stdout = result.stdout ? String(result.stdout) : ''
    const stderr = result.stderr ? String(result.stderr) : ''

    if (result.error) {
        return {
            ok: false,
            code: -1,
            output: `${stdout}\n${stderr}\n${result.error.message}`,
        }
    }

    return {
        ok: result.status === 0,
        code: result.status,
        output: `${stdout}\n${stderr}`,
    }
}

function resolveVsDevCmdPath() {
    if (process.platform !== 'win32') return ''

    const fromEnv = (process.env.VSDEVCMD || '').trim()
    if (fromEnv && fs.existsSync(fromEnv)) return fromEnv

    const programFilesX86 = process.env['ProgramFiles(x86)'] || 'C:\\Program Files (x86)'
    const vswherePath = path.resolve(programFilesX86, 'Microsoft Visual Studio', 'Installer', 'vswhere.exe')

    if (fs.existsSync(vswherePath)) {
        try {
            const result = spawnSync(vswherePath, [
                '-latest',
                '-products', '*',
                '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
                '-find', 'Common7\\Tools\\VsDevCmd.bat',
            ], {
                encoding: 'utf-8',
                windowsHide: true,
            })

            if (result.status === 0) {
                const resolved = String(result.stdout || '').split(/\r?\n/g).map(s => s.trim()).find(Boolean)
                if (resolved && fs.existsSync(resolved)) return resolved
            }
        } catch (e) {
            // ignore and continue with static candidates
        }
    }

    const candidates = [
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '18', 'BuildTools', 'Common7', 'Tools', 'VsDevCmd.bat'),
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '18', 'Community', 'Common7', 'Tools', 'VsDevCmd.bat'),
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '18', 'Professional', 'Common7', 'Tools', 'VsDevCmd.bat'),
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '18', 'Enterprise', 'Common7', 'Tools', 'VsDevCmd.bat'),
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '2022', 'BuildTools', 'Common7', 'Tools', 'VsDevCmd.bat'),
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '2022', 'Community', 'Common7', 'Tools', 'VsDevCmd.bat'),
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '2022', 'Professional', 'Common7', 'Tools', 'VsDevCmd.bat'),
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '2022', 'Enterprise', 'Common7', 'Tools', 'VsDevCmd.bat'),
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '2019', 'BuildTools', 'Common7', 'Tools', 'VsDevCmd.bat'),
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '2019', 'Community', 'Common7', 'Tools', 'VsDevCmd.bat'),
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '2019', 'Professional', 'Common7', 'Tools', 'VsDevCmd.bat'),
        path.resolve(programFilesX86, 'Microsoft Visual Studio', '2019', 'Enterprise', 'Common7', 'Tools', 'VsDevCmd.bat'),
    ]

    for (const p of candidates) {
        if (fs.existsSync(p)) return p
    }

    return ''
}

function failAndExit(message) {
    console.error(`${colors.red}[Error] ${message}${colors.reset}`)
    process.exitCode = 1
}

function resolveDesktopHostExecutable(buildDir, configName) {
    const executableName = process.platform === 'win32' ? 'JqmlDesktopHost.exe' : 'JqmlDesktopHost'
    const candidates = [
        path.resolve(buildDir, executableName),
        path.resolve(buildDir, configName, executableName),
    ]

    for (const p of candidates) {
        if (fs.existsSync(p)) return p
    }

    return ''
}

function setupDesktopHostRunner() {
    if (process.env.IMT_DESKTOP_USE_HOST === '0') return

    const hostDir = path.resolve(__dirname, '../desktop_host')
    const hostCmakePath = path.resolve(hostDir, 'CMakeLists.txt')
    if (!fs.existsSync(hostCmakePath)) return

    const buildDir = (process.env.IMT_DESKTOP_HOST_BUILD_DIR || path.resolve(hostDir, 'build')).trim()
    const configName = (process.env.IMT_DESKTOP_HOST_CONFIG || 'Debug').trim()
    const shouldRebuild = process.env.IMT_DESKTOP_HOST_REBUILD !== '0'
    const hostRequired = process.env.IMT_DESKTOP_HOST_REQUIRED !== '0'
    const vsDevCmdPath = resolveVsDevCmdPath()
    const cleanBuildDir = process.env.IMT_DESKTOP_HOST_CLEAN !== '0'

    if (shouldRebuild) {
        if (cleanBuildDir && fs.existsSync(buildDir)) {
            try {
                fs.rmSync(buildDir, { recursive: true, force: true })
            } catch (e) {
                // ignore, configure step will report a concrete error if this matters
            }
        }

        const baseConfigureArgs = ['-S', hostDir, '-B', buildDir]
        const configuredGenerator = (process.env.IMT_DESKTOP_HOST_GENERATOR || '').trim()
        const envGenerator = (process.env.CMAKE_GENERATOR || '').trim()

        let generatorCandidates = []
        if (configuredGenerator) {
            generatorCandidates = [configuredGenerator]
        } else if (envGenerator) {
            generatorCandidates = [envGenerator]
        } else if (process.platform === 'win32') {
            generatorCandidates = [
                ...(vsDevCmdPath ? ['NMake Makefiles'] : []),
                'Ninja',
                'MinGW Makefiles',
                'NMake Makefiles',
                'Visual Studio 17 2022',
                'Visual Studio 16 2019',
            ]
        } else {
            generatorCandidates = ['Ninja', 'Unix Makefiles']
        }

        generatorCandidates = Array.from(new Set(generatorCandidates.filter(Boolean)))

        if (process.env.IMTCOREDIR && process.env.IMTCOREDIR.trim()) {
            baseConfigureArgs.push(`-DIMTCOREDIR=${process.env.IMTCOREDIR.trim()}`)
        }

        if (process.env.ACFDIR && process.env.ACFDIR.trim()) {
            baseConfigureArgs.push(`-DACFDIR=${process.env.ACFDIR.trim()}`)
        }

        if (process.env.TARGETNAME && process.env.TARGETNAME.trim()) {
            baseConfigureArgs.push(`-DTARGETNAME=${process.env.TARGETNAME.trim()}`)
        }

        let configureResult = null
        let selectedGenerator = ''
        const configureFailures = []

        for (const generator of generatorCandidates) {
            if (cleanBuildDir && fs.existsSync(buildDir)) {
                try {
                    fs.rmSync(buildDir, { recursive: true, force: true })
                } catch (e) {
                    // ignore
                }
            }

            const configureArgs = [...baseConfigureArgs, '-G', generator]
            const result = runCmakeWithOptionalVsDev(configureArgs, hostDir, generator === 'NMake Makefiles' ? vsDevCmdPath : '')

            if (result.ok) {
                configureResult = result
                selectedGenerator = generator
                break
            }

            configureFailures.push(`- ${generator}: ${result.output}`)
        }

        if (!configureResult) {
            throw new Error(`desktop_host configure failed for all generators:\n${configureFailures.join('\n\n')}`)
        }

        const buildArgs = ['--build', buildDir, '--config', configName, '--target', 'JqmlDesktopHost']
        const buildResult = runCmakeWithOptionalVsDev(buildArgs, hostDir, selectedGenerator === 'NMake Makefiles' ? vsDevCmdPath : '')
        if (!buildResult.ok) {
            throw new Error(`desktop_host build failed:\n${buildResult.output}`)
        }
    }

    const hostExePath = resolveDesktopHostExecutable(buildDir, configName)
    if (!hostExePath) {
        if (!hostRequired) {
            console.log(`${colors.yellow}[i] desktop_host executable not found, fallback to qml runner${colors.reset}`)
            return
        }
        throw new Error(`desktop_host executable not found in ${buildDir}`)
    }

    process.env.IMT_DESKTOP_RUNNER = hostExePath
    process.env.IMT_DESKTOP_PRECHECK = '0'
    if (!process.env.IMT_DESKTOP_RUNNER_ARGS_JSON || !process.env.IMT_DESKTOP_RUNNER_ARGS_JSON.trim()) {
        process.env.IMT_DESKTOP_RUNNER_ARGS_JSON = '["{QML_FILE}"]'
    }

    console.log(`${colors.gray}[Desktop host] ${hostExePath}${colors.reset}`)
}

function setupDesktopHostRunnerSafe() {
    const hostRequired = process.env.IMT_DESKTOP_HOST_REQUIRED === '1'

    try {
        setupDesktopHostRunner()
    } catch (err) {
        if (hostRequired) {
            throw err
        }

        const message = getErrorMessage(err)
        console.log(`${colors.yellow}[i] desktop_host unavailable, fallback to qml runner: ${message}${colors.reset}`)
        process.env.IMT_DESKTOP_USE_HOST = '0'
        delete process.env.IMT_DESKTOP_RUNNER
        delete process.env.IMT_DESKTOP_RUNNER_ARGS_JSON
    }
}

function getQmlImportPathsFromConfig(testDirPath) {
    const visitedConfigs = new Set()
    const resolvedDirs = new Set()

    function findModuleRootInTree(rootDir, moduleName, maxDepth = 6) {
        if (!rootDir || !fs.existsSync(rootDir) || !checkIsDirectorySync(rootDir)) return null

        const parts = moduleName.split('.')
        const queue = [{ dir: rootDir, depth: 0 }]
        const visited = new Set()

        while (queue.length > 0) {
            const current = queue.shift()
            if (visited.has(current.dir)) continue
            visited.add(current.dir)

            const candidateQmldir = path.resolve(current.dir, ...parts, 'qmldir')
            if (fs.existsSync(candidateQmldir) && qmldirDeclaresModule(candidateQmldir, moduleName)) {
                return current.dir
            }

            if (current.depth >= maxDepth) continue

            let children = []
            try {
                children = fs.readdirSync(current.dir)
            } catch (e) {
                continue
            }

            for (const child of children) {
                const childPath = path.resolve(current.dir, child)
                if (checkIsDirectorySync(childPath)) {
                    queue.push({ dir: childPath, depth: current.depth + 1 })
                }
            }
        }

        return null
    }

    function tryAppendBuiltImtcoreModule(moduleName) {
        const targetName = (process.env.TARGETNAME || '').trim()
        const root = process.env.IMTCOREDIR_BUILD || process.env.IMTCOREDIR
        if (!root) return

        const candidates = [
            path.resolve(root, 'Bin', `Debug_${targetName}`, 'qml'),
            path.resolve(root, 'Bin', `Release_${targetName}`, 'qml'),
            path.resolve(root, 'Bin', `RelWithDebInfo_${targetName}`, 'qml'),
            path.resolve(root, 'Install', `Debug_${targetName}`, 'qml'),
            path.resolve(root, 'Install', `Release_${targetName}`, 'qml'),
            path.resolve(root, 'Install', `RelWithDebInfo_${targetName}`, 'qml'),
            path.resolve(root, 'Bin'),
            path.resolve(root, 'Install')
        ]

        for (const c of candidates) {
            const moduleRoot = findModuleRootInTree(c, moduleName)
            if (moduleRoot) {
                resolvedDirs.add(moduleRoot)
                return
            }
        }
    }

    function collectConfig(configPath) {
        const normalizedConfigPath = path.resolve(configPath)

        if (visitedConfigs.has(normalizedConfigPath)) return
        visitedConfigs.add(normalizedConfigPath)

        if (!fs.existsSync(normalizedConfigPath)) return

        let config
        try {
            const content = envFill(fs.readFileSync(normalizedConfigPath, { encoding: 'utf8', flag: 'r' }))
            config = JSON.parse(content)
        } catch (err) {
            return
        }

        const configDirPath = path.dirname(normalizedConfigPath)

        if (Array.isArray(config.dirs)) {
            for (const dirPath of config.dirs) {
                const absoluteDirPath = path.resolve(configDirPath, envFill(dirPath))
                if (fs.existsSync(absoluteDirPath) && checkIsDirectorySync(absoluteDirPath)) {
                    resolvedDirs.add(absoluteDirPath)
                    resolvedDirs.add(path.dirname(absoluteDirPath))
                }
            }
        }

        if (Array.isArray(config.includes)) {
            for (const includePath of config.includes) {
                const absoluteIncludePath = path.resolve(configDirPath, envFill(includePath))
                collectConfig(absoluteIncludePath)
            }
        }
    }

    collectConfig(path.resolve(testDirPath, 'test.json'))

    tryAppendBuiltImtcoreModule('com.imtcore.imtqml')

    return Array.from(resolvedDirs)
}

function getQmlImportsFromFile(filePath) {
    if (!fs.existsSync(filePath)) return []

    const content = fs.readFileSync(filePath, 'utf-8')
    const regex = /^\s*import\s+([^\s;]+)\b/gm
    const imports = new Set()
    let match

    while ((match = regex.exec(content)) !== null) {
        const moduleName = String(match[1] || '').trim().replaceAll('"', '').replaceAll("'", '')

        if (!/^[A-Za-z_][\w\.]*$/.test(moduleName)) continue
        if (moduleName.toLowerCase().endsWith('.js')) continue

        // Skip built-in Qt modules and relative/local imports
        if (!moduleName || moduleName.startsWith('Qt')) continue
        if (moduleName === 'QML') continue

        imports.add(moduleName)
    }

    return Array.from(imports)
}

function resolveModuleDirectory(moduleName, qmlImportPaths) {
    const modulePath = moduleName.split('.')
    const moduleShortName = modulePath[modulePath.length - 1]

    for (const importPath of qmlImportPaths) {
        const nestedDir = path.resolve(importPath, ...modulePath)
        const nestedQmldir = path.resolve(nestedDir, 'qmldir')
        if (fs.existsSync(nestedQmldir) && qmldirDeclaresModule(nestedQmldir, moduleName)) {
            return nestedDir
        }

        const directDir = path.resolve(importPath, moduleShortName)
        const directQmldir = path.resolve(directDir, 'qmldir')
        if (fs.existsSync(directQmldir) && qmldirDeclaresModule(directQmldir, moduleName)) {
            return directDir
        }

        const currentQmldir = path.resolve(importPath, 'qmldir')
        if (fs.existsSync(currentQmldir) && qmldirDeclaresModule(currentQmldir, moduleName)) {
            return importPath
        }
    }

    return null
}

function getQmlFilesRecursively(rootDir, maxFiles = 2000) {
    if (!rootDir || !fs.existsSync(rootDir) || !checkIsDirectorySync(rootDir)) return []

    const result = []
    const stack = [rootDir]

    while (stack.length > 0 && result.length < maxFiles) {
        const currentDir = stack.pop()
        let entries = []

        try {
            entries = fs.readdirSync(currentDir)
        } catch (e) {
            continue
        }

        for (const name of entries) {
            const absolutePath = path.resolve(currentDir, name)

            if (checkIsDirectorySync(absolutePath)) {
                stack.push(absolutePath)
                continue
            }

            if (name.toLowerCase().endsWith('.qml')) {
                result.push(absolutePath)
                if (result.length >= maxFiles) break
            }
        }
    }

    return result
}

function qmldirDeclaresModule(qmldirPath, moduleName) {
    try {
        if (!fs.existsSync(qmldirPath)) return false

        const content = fs.readFileSync(qmldirPath, 'utf-8')
        const moduleRegex = /^\s*module\s+(.+)\s*$/m
        const match = content.match(moduleRegex)

        if (!match) return false

        return match[1].trim() === moduleName
    } catch (e) {
        return false
    }
}

function moduleExistsInImportPaths(moduleName, qmlImportPaths) {
    const modulePath = moduleName.split('.')
    const moduleShortName = modulePath[modulePath.length - 1]

    for (const importPath of qmlImportPaths) {
        const nestedQmldir = path.resolve(importPath, ...modulePath, 'qmldir')
        if (fs.existsSync(nestedQmldir) && qmldirDeclaresModule(nestedQmldir, moduleName)) {
            return true
        }

        const directQmldir = path.resolve(importPath, moduleShortName, 'qmldir')
        if (fs.existsSync(directQmldir) && qmldirDeclaresModule(directQmldir, moduleName)) {
            return true
        }

        const currentQmldir = path.resolve(importPath, 'qmldir')
        if (fs.existsSync(currentQmldir)) {
            if (qmldirDeclaresModule(currentQmldir, moduleName)) {
                return true
            }

            if (path.basename(importPath) === moduleShortName) {
                return true
            }
        }
    }

    return false
}

function findMissingQmlModules(entryFilePath, qmlImportPaths) {
    const modules = getQmlImportsFromFile(entryFilePath)
    return modules.filter(moduleName => !moduleExistsInImportPaths(moduleName, qmlImportPaths))
}

function getDesktopRunnerSpec(filePath, qmlImportPaths) {
    const customRunner = (process.env.IMT_DESKTOP_RUNNER || '').trim()

    if (!customRunner) {
        const importArgs = []
        for (const importPath of qmlImportPaths) {
            importArgs.push('-I', importPath)
        }

        return {
            command: 'qml',
            args: ['-platform', 'offscreen', ...importArgs, filePath],
            skipPrecheck: false,
        }
    }

    const pathSeparator = process.platform === 'win32' ? ';' : ':'
    const placeholders = {
        '{QML_FILE}': filePath,
        '{QML_DIR}': path.dirname(filePath),
        '{IMPORT_PATHS}': qmlImportPaths.join(pathSeparator),
    }

    let args = []
    const argsJson = (process.env.IMT_DESKTOP_RUNNER_ARGS_JSON || '').trim()
    const argsRaw = (process.env.IMT_DESKTOP_RUNNER_ARGS || '').trim()

    if (argsJson) {
        try {
            const parsed = JSON.parse(argsJson)
            if (Array.isArray(parsed)) {
                args = parsed.map(item => String(item))
            }
        } catch (e) {
            args = []
        }
    } else if (argsRaw) {
        args = argsRaw.split(/\s+/g).filter(Boolean)
    }

    args = args.map(arg => {
        let result = arg
        for (const key of Object.keys(placeholders)) {
            result = result.replaceAll(key, placeholders[key])
        }
        return result
    })

    if (args.length === 0 || process.env.IMT_DESKTOP_RUNNER_APPEND_QML === '1') {
        args.push(filePath)
    }

    return {
        command: customRunner,
        args,
        skipPrecheck: process.env.IMT_DESKTOP_PRECHECK !== '1',
    }
}

function runQmlTest(filePath, timeout = 5000, qmlImportPaths = []) {
    return new Promise((resolve, reject) => {
        const desktopRunner = getDesktopRunnerSpec(filePath, qmlImportPaths)

        if (!desktopRunner.skipPrecheck) {
            const missingModules = findMissingQmlModules(filePath, qmlImportPaths)
            if (missingModules.length > 0) {
                reject(new Error(`Missing QML modules in import paths: ${missingModules.join(', ')}`))
                return
            }
        }

        const pathSeparator = process.platform === 'win32' ? ';' : ':'
        const mergedImportPath = qmlImportPaths.join(pathSeparator)

        const env = { ...process.env, QT_LOGGING_TO_CONSOLE: "1" }
        if (mergedImportPath) {
            env.QML2_IMPORT_PATH = env.QML2_IMPORT_PATH
                ? `${mergedImportPath}${pathSeparator}${env.QML2_IMPORT_PATH}`
                : mergedImportPath
            env.QML_IMPORT_PATH = env.QML_IMPORT_PATH
                ? `${mergedImportPath}${pathSeparator}${env.QML_IMPORT_PATH}`
                : mergedImportPath
        }

        // Запуск desktop-раннера
        const child = spawn(desktopRunner.command, desktopRunner.args, {
            cwd: path.dirname(filePath),
            env,
        })

        let rawOutput = ''

        child.stdout.on('data', (data) => {
            rawOutput += data.toString()
        })
        child.stderr.on('data', (data) => {
            rawOutput += data.toString()
        })

        // Тайм-аут на случай зависания теста
        const timer = setTimeout(() => {
            child.kill()
            reject(new Error(`Test timed out: ${filePath}`))
        }, timeout)

        child.on('close', (code) => {
            clearTimeout(timer)

            let logs = getQmlLog(rawOutput)
            const diagnostics = getQmlDiagnostics(rawOutput)

            if (diagnostics.length > 0) {
                const missingDiagnostics = diagnostics.filter(diag => !logs.some(log => log.includes(diag) || diag.includes(log)))

                if (missingDiagnostics.length > 0) {
                    console.log(`${colors.gray}[Desktop details] ${missingDiagnostics.join(' ; ')}${colors.reset}`)
                }
            }

            if (logs.length === 0 && rawOutput) {
                if (diagnostics.length > 0) {
                    logs.push(...diagnostics)
                } else {
                    const rawLines = rawOutput
                        .split(/\r?\n/g)
                        .map(line => line.trim())
                        .filter(Boolean)

                    logs.push(...rawLines.slice(0, 20))
                }
            }

            resolve(logs)
        })
    })
}

function checkIsDirectorySync(path) {
    try {
        return fs.statSync(path).isDirectory()
    } catch (e) {
        return false
    }
}

function parseCliOptions() {
    const args = process.argv.slice(2)
    let testName = ''

    for (let i = 0; i < args.length; i++) {
        const arg = args[i]

        if (arg.startsWith('--test=')) {
            testName = arg.slice('--test='.length).trim()
            continue
        }

        if (arg === '--test' || arg === '-t') {
            testName = (args[i + 1] || '').trim()
            i++
            continue
        }

        if (!arg.startsWith('-') && !testName) {
            testName = arg.trim()
        }
    }

    return { testName }
}

function getTestsToRun(allEntries, selectedTestName) {
    const allTests = allEntries.filter(testdir => {
        const testDirPath = path.resolve(__dirname, `./${testdir}`)
        const mainFilePath = path.resolve(__dirname, `./${testdir}/Main.qml`)

        return checkIsDirectorySync(testDirPath) && fs.existsSync(mainFilePath)
    })

    if (!selectedTestName) return allTests

    return allTests.filter(testName => testName === selectedTestName)
}

async function runTests() {
    ensureDefaultTargetName()
    setupDesktopHostRunnerSafe()

    const tests = fs.readdirSync('./tests')
    const options = parseCliOptions()
    const testsToRun = getTestsToRun(tests, options.testName)

    if (options.testName && testsToRun.length === 0) {
        console.error(`${colors.red}[Error] Test not found: ${options.testName}${colors.reset}`)
        process.exitCode = 1
        return
    }

    let completedTests = 0
    let testCount = 0

    const driver = await createWebDriver()

    for (let testdir of testsToRun) {
        let mainFilePath = path.resolve(__dirname, `./${testdir}/Main.qml`)
        let testDirPath = path.resolve(__dirname, `./${testdir}`)
        const qmlImportPaths = getQmlImportPathsFromConfig(testDirPath)

        if (checkIsDirectorySync(testDirPath)) {
            console.log(`${colors.yellow}[i] Started test: ${testdir}${colors.reset}`)
            testCount++
            let resultDesktop
            let resultWeb

            try {
                resultDesktop = await runQmlTest(mainFilePath, 5000, qmlImportPaths)
            } catch (err) {
                console.error(`${colors.red}[Error] Error running test on desktop: ${getErrorMessage(err)}${colors.reset}`)
            }

            try {
                resultWeb = await runWebTest(driver, testDirPath)
            } catch (err) {
                console.error(`${colors.red}[Error] Error running web test: ${getErrorMessage(err)}${colors.reset}`)
            }

            console.log(`${colors.cyan}[Desktop] ${resultDesktop}${colors.reset}`)
            console.log(`${colors.cyan}[Web] ${resultWeb}${colors.reset}`)

            if (resultDesktop && resultWeb && resultDesktop.join('') === resultWeb.join('')) {
                completedTests++
                console.log(`${colors.green}[+] Completed test: ${testdir}${colors.reset}`)
            } else {
                console.log(`${colors.red}[-] Failed test: ${testdir}${colors.reset}`)
            }
        }
    }

    await closeWebDriver(driver)

    console.log(`${colors.yellow}[i] Completed tests: ${completedTests}/${testCount}${colors.reset}`)
}

async function runWebTest(driver, testDirPath, timeout = 5000) {
    try {
        fs.mkdirSync(path.resolve(testDirPath, '_web'), { recursive: true })
    } catch (err) {
        console.error(`${colors.red}[Error] Error creating _web folder: ${err.message}${colors.reset}`)
    }

    try {
        const compiler = getCompiler()

        compiler.compile({
            config: path.resolve(testDirPath, 'test.json'),
            output: path.resolve(testDirPath, '_web'),
            entry: path.resolve(testDirPath, 'Main.qml'),
            name: 'test',
            root: '/',
            mode: 'js',
        })

        compiler.compile({
            config: path.resolve(testDirPath, 'test.json'),
            output: path.resolve(testDirPath, '_web'),
            entry: path.resolve(testDirPath, 'Main.qml'),
            name: 'test',
            root: '/',
            mode: 'html',
        })

        // Copy image assets to _web for file:// access
        const imageExts = ['.svg', '.png', '.jpg', '.jpeg', '.gif', '.webp', '.bmp', '.ico']
        const testFiles = fs.readdirSync(testDirPath)
        for(const f of testFiles){
            if(imageExts.some(ext => f.toLowerCase().endsWith(ext))){
                fs.copyFileSync(path.resolve(testDirPath, f), path.resolve(testDirPath, '_web', f))
            }
        }
    } catch (err) {
        console.error(`${colors.red}[Error] Error during compilation: ${getErrorMessage(err)}${colors.reset}`)
    }

    try {
        await driver.get('file://' + path.resolve(testDirPath, './_web/test.html'))

        await driver.wait(async () => {
            const readyState = await driver.executeScript('return document.readyState')
            return readyState === 'complete'
        }, timeout) // таймаут 10 секунд

        let mainQml = fs.readFileSync(path.resolve(testDirPath, 'Main.qml'), 'utf-8')
        let containTimer = mainQml.indexOf('Timer') >= 0
        let containAsync = mainQml.indexOf('onStatusChanged') >= 0 || mainQml.indexOf('Image {') >= 0 || mainQml.indexOf('Image{') >= 0

        // Always wait for Qt.quit() signal (handles microtask-deferred signals like focus)
        try {
            await driver.wait(async () => {
                return await driver.executeScript('return !!window.__jqmlQuit')
            }, timeout)
        } catch(e) {
            // timeout - proceed anyway
        }

        if (containTimer || containAsync) {
            // Extra wait for async operations after Qt.quit()
            await driver.sleep(200)
        }

        const logs = await driver.manage().logs().get('browser')

        let messages = []

        logs.forEach(log => {
            if (log.level.name === 'INFO') {
                messages.push(`${log.message.split(' ').slice(2).join(' ').replaceAll('"', '')}`) // Удаляем первые 2 слова
            }
        })

        return messages
    } finally {
        try {
            fs.rmSync(path.resolve(testDirPath, '_web'), { recursive: true, force: true });
        } catch (err) {
            console.error(`${colors.red}[Error] Error deleting _web folder: ${err.message}${colors.reset}`);
        }
    }


}

async function createWebDriver() {
    try {
        // Настройка Chrome (опционально: запуск без окна)
        let options = new chrome.Options()
        options.addArguments('--headless', '--window-size=800,600', '--no-sandbox', '--disable-dev-shm-usage', '--allow-file-access-from-files') // Раскомментировать для headless-режима
        options.setLoggingPrefs({
            browser: 'ALL', // Собирать все типы сообщений (INFO, WARNING, SEVERE)
            driver: 'WARNING'
        })

        let service = new chrome.ServiceBuilder(chromedriverPath)

        let driver = await new Builder()
            .forBrowser('chrome')
            .setChromeOptions(options)
            .setChromeService(service)
            .build()
        
        return driver
    } catch (err) {
        console.error(`${colors.red}[Error] Error creating WebDriver: ${err.message}${colors.reset}`)
    }
}


async function closeWebDriver(driver) {
    await driver.quit() // Закрыть браузер
}


runTests().catch(err => {
    if (!process.exitCode) {
        failAndExit(getErrorMessage(err))
    }
})
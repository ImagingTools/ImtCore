import argparse
import copy
import json
from pathlib import Path
import shutil
import subprocess
import tempfile


def load_modules(manifest, source_root):
    modules = json.loads(manifest.read_text(encoding="utf-8"))
    exports = set()
    uris = set()
    for module in modules:
        if module["uri"] in uris or not all(part.isidentifier() for part in module["uri"].split(".")):
            raise ValueError("Duplicate or invalid module URI: " + module["uri"])
        uris.add(module["uri"])
        major, minor = module["version"].split(".")
        if not major.isdigit() or not minor.isdigit():
            raise ValueError("Invalid module version: " + module["version"])
        classes = set()
        for entry in module["types"]:
            key = (module["uri"], entry["name"])
            if key in exports:
                raise ValueError("Duplicate QML export: " + ".".join(key))
            exports.add(key)
            if entry["class"] in classes:
                raise ValueError("Duplicate C++ class in module: " + entry["class"])
            classes.add(entry["class"])
            if not (source_root / entry["header"]).is_file():
                raise ValueError("Missing header: " + entry["header"])
        if "qml_directory" in module and not (source_root / module["qml_directory"] / "qmldir").is_file():
            raise ValueError("Missing source qmldir: " + module["qml_directory"])
    return modules


def annotate(document, entry):
    document = copy.deepcopy(document)
    for class_info in document.get("classes", []):
        if class_info["qualifiedClassName"] != entry["class"]:
            continue
        annotations = [{"name": "QML.Element", "value": entry["name"]}]
        if entry.get("singleton", False):
            annotations.append({"name": "QML.Singleton", "value": "true"})
        if entry.get("uncreatable", False):
            annotations.append({"name": "QML.Creatable", "value": "false"})
        class_info["classInfos"] = [
            info for info in class_info.get("classInfos", [])
            if not info["name"].startswith("QML.")
        ] + annotations
        return document
    raise ValueError("MOC did not describe " + entry["class"])


def write_qmldir(module, source_root, module_output):
    if "qml_directory" in module:
        qml_source = source_root / module["qml_directory"]
        shutil.copytree(qml_source, module_output, dirs_exist_ok=True)
        lines = (qml_source / "qmldir").read_text(encoding="utf-8").splitlines()
        lines = [line for line in lines if not line.split() or line.split()[0] != "typeinfo"]
    else:
        lines = ["module " + module["uri"]]
    (module_output / "qmldir").write_text("\n".join(lines) + "\ntypeinfo plugins.qmltypes\n", encoding="utf-8")


def find_metatypes(qt_root, module):
    directory = qt_root / "metatypes"
    prefix = "qt6" + module.lower()
    for configuration in ("", "_release", "_relwithdebinfo", "_minsizerel", "_debug"):
        candidate = directory / (prefix + configuration + "_metatypes.json")
        if candidate.is_file():
            return candidate
    candidates = sorted(directory.glob(prefix + "_*_metatypes.json"))
    if len(candidates) == 1:
        return candidates[0]
    raise ValueError("Cannot select Qt metatypes for " + module + " in " + str(directory))


def generate(manifest, source_root, qt_root, output_root):
    modules = load_modules(manifest, source_root)
    executable_suffix = ".exe" if (qt_root / "bin/moc.exe").exists() else ""
    tool_dir = qt_root / "libexec"
    if not (tool_dir / ("moc" + executable_suffix)).exists():
        tool_dir = qt_root / "bin"
    moc = tool_dir / ("moc" + executable_suffix)
    registrar = tool_dir / ("qmltyperegistrar" + executable_suffix)
    qt_modules = {"Core", "Gui", "Qml", "Quick", "Network", "WebSockets"}
    for module in modules:
        qt_modules.update(module.get("qt_modules", []))
    foreign_types = [
        find_metatypes(qt_root, name)
        for name in sorted(qt_modules)
    ]
    if not moc.is_file() or not registrar.is_file() or not all(path.is_file() for path in foreign_types):
        raise ValueError("Qt installation must provide moc, qmltyperegistrar and metatypes: " + str(qt_root))

    with tempfile.TemporaryDirectory(prefix="qml-tooling-") as temporary:
        temporary_root = Path(temporary)
        documents = {}
        for module in modules:
            for entry in module["types"]:
                if entry["header"] in documents:
                    continue
                output = temporary_root / (str(len(documents)) + ".cpp")
                subprocess.run([
                    str(moc), "--output-json", "-o", str(output),
                    str(source_root / entry["header"])
                ], check=True)
                documents[entry["header"]] = json.loads(
                    Path(str(output) + ".json").read_text(encoding="utf-8")
                )

        for module_index, module in enumerate(modules):
            module_output = output_root.joinpath(*module["uri"].split("."))
            module_output.mkdir(parents=True, exist_ok=True)
            write_qmldir(module, source_root, module_output)
            inputs = []
            module_documents = {}
            for entry in module["types"]:
                document = module_documents.get(entry["header"], documents[entry["header"]])
                module_documents[entry["header"]] = annotate(document, entry)
            for entry_index, document in enumerate(module_documents.values()):
                input_path = temporary_root / (str(module_index) + "-" + str(entry_index) + ".json")
                input_path.write_text(json.dumps(document), encoding="utf-8")
                inputs.append(str(input_path))
            major, minor = module["version"].split(".")
            subprocess.run([
                str(registrar), "--import-name", module["uri"],
                "--major-version", major, "--minor-version", minor,
                "--foreign-types", ",".join(str(path) for path in foreign_types),
                "--generate-qmltypes", str(module_output / "plugins.qmltypes"),
                "-o", str(temporary_root / "unused-registration.cpp"),
                *inputs
            ], check=True)
            print("Generated " + module["uri"] + " (" + str(len(module["types"])) + " types)")


def main():
    parser = argparse.ArgumentParser(description="Generate editor-only QML type metadata without compiling C++.")
    parser.add_argument("--source-root", type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument("--manifest", type=Path, default=Path(__file__).with_name("modules.json"))
    parser.add_argument("--qt-root", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    arguments = parser.parse_args()
    try:
        generate(arguments.manifest, arguments.source_root, arguments.qt_root, arguments.output)
    except (ValueError, OSError, subprocess.CalledProcessError) as error:
        parser.exit(1, "QML metadata generation failed: " + str(error) + "\n")


if __name__ == "__main__":
    main()
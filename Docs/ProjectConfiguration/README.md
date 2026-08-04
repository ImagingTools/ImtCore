# Project Configuration Documentation

Documentation for the target-based CMake configuration used by ImtCore and every
product built on top of it, files
`*LibraryDependencies.cmake` - the link-scope variables, the `find_package`
package export, and how to configure a **new** end product.

Документация по target-based конфигурации CMake, используемой ImtCore и каждым
продуктом на его основе файлы
`*LibraryDependencies.cmake`, переменные link-scope, экспорт пакета
`find_package` и как настроить **новый** конечный продукт.

## Contents / Содержание

| Document | Язык |
|---|---|
| [Target-Based Library Dependencies](LibraryDependencies.en.md) | 🇬🇧 English |
| [Целевые зависимости библиотек (target-based)](LibraryDependencies.ru.md) | 🇷🇺 Русский |

Both documents have identical structure and cover:

- What the `<Project>LibraryDependencies.cmake` files are and **why** they exist.
- The link-scope variables (`ACF_*_LINK_SCOPE`) and the plain-vs-keyword
  `target_link_libraries` signatures.
- The anatomy of a dependency file (helper function, guards, the SDL
  `imtbasesdl → imtgql` augmentation).
- How the file is wired into `Build/CMake/CMakeLists.txt` and the full package
  file-set (`*Env.cmake`, `*PackageExport.cmake`, `*Config.cmake.in`).
- **A step-by-step recipe for creating a new file for a future product.**
- The unified in-tree build (super-build) and a migration checklist.
- Troubleshooting the real configuration/generation errors.

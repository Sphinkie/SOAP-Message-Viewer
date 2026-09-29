# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

SOAP Message Viewer is a Qt Widgets desktop app (Windows) that opens WCF trace files (`web_messages.svclog`, produced by the `<Diagnostics>` section of a `web.config`) and displays the SOAP messages they contain. It is a fast, simpler alternative to Microsoft's SvcTraceViewer and needs only the messages file, not the trace file. Code comments, Doxygen docs and the README are in French; UI strings are mostly in English.

## Build

- qmake project: `SOAP-Message-Viewer.pro`, normally built from Qt Creator. Current kit: Qt 6.11.1 MinGW 64-bit (the project originally targeted Qt 5.15 with MSVC).
- Command line: `export PATH="/c/Qt/6.11.1/mingw_64/bin:/c/Qt/Tools/mingw1310_64/bin:$PATH"`, then `qmake SOAP-Message-Viewer.pro CONFIG+=debug` and `mingw32-make -j8`. Build out-of-source.
- Expat is compiled into the app from `Expat/src/*.c` as a static build (`XML_STATIC`), using the hand-written `Expat/include/expat_config.h`. There are no prebuilt Expat binaries; don't reintroduce a DLL or `.lib` dependency. The only deployment step is `windeployqt` on the exe.
- Keep the code portable between MinGW and MSVC: don't use MSVC-only CRT `_s` functions, and include `<cstring>` explicitly (MSVC includes it implicitly, MinGW doesn't).
- There are no automated tests and no linter.
- Version number: update it in three places together: `VERSION` in the `.pro`, `MainWindow::version` in `Sources/mainwindow.h`, and `PROJECT_NUMBER` in `docs/doxygen/Doxyfile`.
- Docs: run `docs/doxygen/doxygen.bat` (it calls `C:\Program Files\doxygen\bin\doxygen.exe Doxyfile`). The HTML output in `docs/doxygen/html` is committed and published through GitHub Pages (`docs/`).

## Architecture

All sources are in `Sources/`. Every parser uses **Expat** in SAX mode, with static C callbacks (`startElementHandler`, `endElementHandler`, `dataHandler`). Because the parsing is streaming, truncated or damaged svclog files can still be read up to the point of damage. Many classes rely on static members, because the Expat callbacks need them to share state.

**File-level processing** (reads the whole file in `BUFFER_SIZE` chunks):
- `FileParser`: the main loader. It scans an `.svclog` file (or an `.xml`/"faster" file via Import, with `useCRLF=true`) and fills `Messages` with one entry per SOAP message. For each message it records the byte offsets where the `<Body>` starts and ends (`XML_GetCurrentByteIndex`), not the body itself. It calls `MainWindow::addListItem` for each message, and stops at `MAX_MESSAGES` (32000, the QListWidget limit), returning `XML_ERROR_ABORTED`.
- `FileConverter`: rewrites an svclog as `*.faster.xml`, keeping only the useful tags.
- `FileRepair`: works on raw bytes rather than as an XML parser. It fixes a truncated svclog by dropping the partial line, closing any open tags, removing `i:nil`/`xsi:nil`, and wrapping the result in `<root>`. The output is `*.repaired.xml`.
- `FileSplitter`: splits a huge svclog into files of 20,000 messages each.

**Message model**:
- `Messages` is a fully static class that wraps a file-level `std::vector<Message*>` plus a `currentMessage` pointer. `FileParser` fills it through setters that act on the current message. The body is loaded lazily: `getMessageBody(n)` seeks to the stored offsets in the original file and caches the result as a `char*` in `Message::body`. The source file must therefore stay unchanged on disk while the app is using it.
- Query/response pairs are linked by `activityId`. `getNext/PreviousCorrelatedMessageIndex` drive the "jump" button in `MainWindow`.
- The header comment in `messages.h` documents which svclog tags feed each field, for queries and for responses.

**Body renderers**: `MainWindow::displayMessageBody` picks one by `tabWidget` index. All of them re-parse the body blob with Expat:
- `BodyTextParser` (tab 0): builds a Qt rich-text HTML string with indentation and syntax coloring.
- `BodyTreeParser` (tab 1): fills a `QTreeWidget`.
- `BodyTableParser` (tab 2): builds a table view.
- `BodyPrintableParser`: writes the marked messages to a file ("Save").
All of them accept a `hideNamespaces` flag, driven by the `actionHideNS` menu toggle.

**UI**: `Sources/mainwindow.ui` (Qt Designer) plus `MainWindow`. Slots are auto-connected through the `on_<object>_<signal>` naming convention, so renaming a widget in the `.ui` silently breaks its slot. Icons are in `resources/` and listed in `resources/resources.qrc`, under the `/resources` prefix. The code loads them as `":/resources/<file>"`, so a new icon must be added to the `.qrc` as a bare filename.

## Notes

- Line endings: a raw svclog is one line that may contain LF characters, while XML files use CRLF. A file that mixes both cannot be parsed correctly (see the `useCRLF` parameter of `FileParser::parse`).
- `*.svclog`, `*.xml` and `*.user` are git-ignored. Do not commit sample trace files.
- Expat setup and upgrade procedure: `docs/expat/readme.md`.

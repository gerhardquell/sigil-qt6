# Sigil Qt 6 Migration Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Migriere Sigil EPUB-Editor von Qt 5.2 zu Qt 6.9.2 unter Linux, ersetze Qt WebKit durch CodeView-Only mit WebEngine-Preview.

**Architecture:** Entferne BookView/WYSIWYG vollständig, behalte CodeViewEditor, füge neue Preview-Komponente mit QWebEngineView hinzu, aktualisiere CMake für Qt 6, entferne alle plattformspezifischen Code für Windows/macOS.

**Tech Stack:** Qt 6.9.2 (Widgets, WebEngineWidgets, Svg, Xml), CMake 3.25, GCC 14.1, C++17

---

## Phase 1: CMake Modernisierung

### Task 1: Root CMakeLists.txt aktualisieren

**Files:**
- Modify: `/data1/u2/sigil074/CMakeLists.txt`

- [ ] **Step 1: CMake Minimum Version erhöhen**

```cmake
# Ersetze Zeile 10
cmake_minimum_required(VERSION 3.16)
```

- [ ] **Step 2: C++ Standard setzen**

Füge nach Zeile 11 ein (vor `project(Sigil)`):
```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

- [ ] **Step 3: Qt 6 Environment konfigurieren**

Füge nach Zeile 13 ein:
```cmake
# Qt 6 Pfad setzen
set(Qt6_DIR /usr/local/Qt-6.9.2/gcc_64/lib/cmake/Qt6)
list(APPEND CMAKE_PREFIX_PATH /usr/local/Qt-6.9.2/gcc_64)
```

- [ ] **Step 4: Qt 6 Komponenten suchen**

Ersetze Zeilen 57-69 (die find_package Aufrufe):
```cmake
if(NOT FORCE_BUNDLED_COPIES)
    find_package(Qt6 COMPONENTS Core Network Svg Widgets Xml Concurrent PrintSupport REQUIRED)
    find_package(Qt6WebEngine COMPONENTS Core Widgets REQUIRED)
    find_package(Boost COMPONENTS date_time filesystem program_options regex system thread)
    find_package(ZLIB)
    find_package(PkgConfig)
    find_package(MiniZip)
    pkg_check_modules(XERCES xerces-c>=3.1)
    pkg_check_modules(HUNSPELL hunspell)
    if(NOT APPLE)
        pkg_check_modules(PCRE libpcre16)
    endif()
    find_package(XercesExtensions)
    find_package(FlightCrew)
endif()
```

- [ ] **Step 5: Build-Verzeichnis testen**

```bash
cd /data1/u2/sigil074
mkdir -p build_test && cd build_test
cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1 | head -50
```

Expected: "-- Configuring done" ohne Qt5-Fehler

- [ ] **Step 6: Commit**

```bash
cd /data1/u2/sigil074
git add CMakeLists.txt
git commit -m "build: Update root CMakeLists.txt for Qt 6

- Set C++17 standard
- Add Qt 6 path configuration
- Update find_package calls for Qt6 components
- Add Qt6WebEngine dependency"
```

---

### Task 2: Sigil CMakeLists.txt aktualisieren

**Files:**
- Modify: `/data1/u2/sigil074/src/Sigil/CMakeLists.txt`

- [ ] **Step 1: Qt 6 Packages finden**

Ersetze Zeile 38:
```cmake
# Vorher:
# find_package(Qt5 COMPONENTS Core Network Svg WebKit WebKitWidgets Widgets Xml XmlPatterns Concurrent PrintSupport LinguistTools)

# Nachher:
find_package(Qt6 COMPONENTS Core Network Svg Widgets Xml Concurrent PrintSupport LinguistTools REQUIRED)
find_package(Qt6WebEngine COMPONENTS Core Widgets REQUIRED)
```

- [ ] **Step 2: Qt6 Macros verwenden**

Ersetze Zeilen 454-458 (qt5_wrap_ui, qt5_add_resources, qt5_add_translation):
```cmake
# Runs UIC on specified files
qt6_wrap_ui(UI_FILES_H ${UI_FILES})
# Runs RCC on specified files
qt6_add_resources(QRC_FILES_CPP ${QRC_FILES})
# Runs lrelease on the specified files
qt6_add_translation(QM_FILES ${TS_FILES})
```

- [ ] **Step 3: Qt6 Link Libraries**

Ersetze Zeile 568:
```cmake
# Vorher:
# qt5_use_modules(${PROJECT_NAME} Widgets Xml XmlPatterns PrintSupport Svg WebKit WebKitWidgets Network Concurrent)

# Nachher - Link via target_link_libraries (siehe Step 4)
```

- [ ] **Step 4: Target Link Libraries aktualisieren**

Ersetze Zeile 567:
```cmake
# Vorher:
# target_link_libraries(${PROJECT_NAME} ${QT_MAIN} ${HUNSPELL_LIBRARIES} ${PCRE_LIBRARIES} tidyLib ${MINIZIP_LIBRARIES} ${BOOST_LIBS} ${XERCES_LIBRARIES} ${XERCESEXTENSIONS_LIBRARIES} ${FLIGHTCREW_LIBRARIES})

# Nachher:
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Network
    Qt6::Svg
    Qt6::Widgets
    Qt6::Xml
    Qt6::Concurrent
    Qt6::PrintSupport
    Qt6::WebEngineCore
    Qt6::WebEngineWidgets
    ${HUNSPELL_LIBRARIES}
    ${PCRE_LIBRARIES}
    tidyLib
    ${MINIZIP_LIBRARIES}
    ${BOOST_LIBS}
    ${XERCES_LIBRARIES}
    ${XERCESEXTENSIONS_LIBRARIES}
    ${FLIGHTCREW_LIBRARIES}
)
```

- [ ] **Step 5: Entferne Qt5-Spezifika**

Lösche Zeilen 563-565 (Windows WinMain Spezialfall):
```cmake
# LÖSCHEN:
# if (MSVC)
#     set(QT_MAIN Qt5::WinMain)
# endif()
```

- [ ] **Step 6: Windows/macOS Installer Code entfernen**

Lösche Zeilen 606-840 (alles nach `# For Mac, add frameworks...` bis `# needed for correct static header...`):

Behalte nur:
```cmake
# For Linux
if(UNIX AND NOT APPLE)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(FILES ${LINUX_DESKTOP_FILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
    install(FILES ${LINUX_DESKTOP_ICON_FILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps RENAME sigil.png)
    install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sigil/translations)
    install(FILES ${DIC_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sigil/hunspell_dictionaries)
    install(FILES ${EXAMPLE_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sigil/examples)
endif()
```

- [ ] **Step 7: Test Build**

```bash
cd /data1/u2/sigil074/build_test
rm -rf * && cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -20
```

Expected: Keine Fehler, Qt6 wird gefunden

- [ ] **Step 8: Commit**

```bash
cd /data1/u2/sigil074
git add src/Sigil/CMakeLists.txt
git commit -m "build: Update Sigil CMakeLists.txt for Qt 6

- Replace Qt5 with Qt6 components
- Add WebEngine dependencies
- Remove Windows/macOS installer code
- Update link libraries for Qt6"
```

---

## Phase 2: Entferne WebKit Abhängigkeiten

### Task 3: Entferne BookViewEditor und BookViewPreview

**Files:**
- Delete: `/data1/u2/sigil074/src/Sigil/ViewEditors/BookViewEditor.cpp`
- Delete: `/data1/u2/sigil074/src/Sigil/ViewEditors/BookViewEditor.h`
- Delete: `/data1/u2/sigil074/src/Sigil/ViewEditors/BookViewPreview.cpp`
- Delete: `/data1/u2/sigil074/src/Sigil/ViewEditors/BookViewPreview.h`
- Delete: `/data1/u2/sigil074/src/Sigil/ViewEditors/ViewWebPage.cpp`
- Delete: `/data1/u2/sigil074/src/Sigil/ViewEditors/ViewWebPage.h`
- Modify: `/data1/u2/sigil074/src/Sigil/CMakeLists.txt`

- [ ] **Step 1: Lösche WebKit-basierte Dateien**

```bash
cd /data1/u2/sigil074/src/Sigil/ViewEditors
rm -f BookViewEditor.cpp BookViewEditor.h
rm -f BookViewPreview.cpp BookViewPreview.h
rm -f ViewWebPage.cpp ViewWebPage.h
```

- [ ] **Step 2: Update CMakeLists.txt - entferne Dateien aus VIEW_EDITOR_FILES**

In `/data1/u2/sigil074/src/Sigil/CMakeLists.txt`, Zeilen 292-307:

Ersetze:
```cmake
set(VIEW_EDITOR_FILES
    ViewEditors/BookViewEditor.cpp
    ViewEditors/BookViewEditor.h
    ViewEditors/BookViewPreview.cpp
    ViewEditors/BookViewPreview.h
    ViewEditors/CodeViewEditor.cpp
    ViewEditors/CodeViewEditor.h
    ViewEditors/LineNumberArea.cpp
    ViewEditors/LineNumberArea.h
    ViewEditors/Searchable.cpp
    ViewEditors/Searchable.h
    ViewEditors/Zoomable.h
    ViewEditors/ViewEditor.h
    ViewEditors/ViewWebPage.cpp
    ViewEditors/ViewWebPage.h
)
```

durch:
```cmake
set(VIEW_EDITOR_FILES
    ViewEditors/CodeViewEditor.cpp
    ViewEditors/CodeViewEditor.h
    ViewEditors/LineNumberArea.cpp
    ViewEditors/LineNumberArea.h
    ViewEditors/Searchable.cpp
    ViewEditors/Searchable.h
    ViewEditors/Zoomable.h
    ViewEditors/ViewEditor.h
)
```

- [ ] **Step 3: Commit**

```bash
cd /data1/u2/sigil074
git add -A
git commit -m "refactor: Remove WebKit-based BookView components

- Delete BookViewEditor (WYSIWYG)
- Delete BookViewPreview
- Delete ViewWebPage
- Update CMakeLists.txt"
```

---

### Task 4: Entferne Qt XML Patterns Abhängigkeiten

**Files:**
- Modify: `/data1/u2/sigil074/src/Sigil/Misc/XHTMLHighlighter.cpp`
- Modify: `/data1/u2/sigil074/src/Sigil/Dialogs/ReportsWidgets/CharactersInHTMLFilesWidget.cpp`

- [ ] **Step 1: Untersuche XML Patterns Nutzung**

```bash
grep -r "XmlPatterns\|QXmlQuery\|QXmlResultItems" /data1/u2/sigil074/src/Sigil --include="*.cpp" --include="*.h"
```

- [ ] **Step 2: XHTMLHighlighter.cpp anpassen**

Lese Datei und ersetze XML Patterns durch Qt XML Streams:

In `/data1/u2/sigil074/src/Sigil/Misc/XHTMLHighlighter.cpp`:

Entferne:
```cpp
#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/QXmlResultItems>
```

Ersetze durch:
```cpp
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
```

- [ ] **Step 3: CharactersInHTMLFilesWidget.cpp anpassen**

Ähnliche Änderung für XML Patterns Nutzung.

- [ ] **Step 4: Commit**

```bash
cd /data1/u2/sigil074
git add -A
git commit -m "refactor: Replace Qt XML Patterns with Qt XML Streams

- Remove QXmlQuery/QXmlResultItems dependencies
- Use QXmlStreamReader/Writer instead"
```

---

### Task 5: Haupt-Header bereinigen

**Files:**
- Modify: `/data1/u2/sigil074/src/Sigil/main.cpp`
- Modify: `/data1/u2/sigil074/src/Sigil/MainUI/MainWindow.cpp`
- Modify: `/data1/u2/sigil074/src/Sigil/MainUI/MainWindow.h`

- [ ] **Step 1: main.cpp - entferne Windows/macOS Code**

In `/data1/u2/sigil074/src/Sigil/main.cpp`:

Entferne Zeilen 44-55 (Windows-Clipboard-Workaround):
```cpp
#ifdef Q_OS_WIN32
# include <QtWidgets/QPlainTextEdit>
# include "ViewEditors/BookViewPreview.h"
static const QString WIN_CLIPBOARD_ERROR = "QClipboard::setMimeData: Failed to set data on clipboard";
static const int RETRY_DELAY_MS = 5;
#endif
```

Entferne Zeilen 72-102 (macOS-spezifische Menüs):
```cpp
#ifdef Q_OS_MAC
static void file_new() { ... }
static void file_open() { ... }
#endif
```

Entferne Zeilen 106-118 (Linux Icon):
```cpp
#if !defined(Q_OS_WIN32) && !defined(Q_OS_MAC)
static QIcon GetApplicationIcon() { ... }
#endif
```

Entferne MessageHandler Windows-Block (Zeilen 137-177).

- [ ] **Step 2: main.cpp - vereinfache main()**

In `main()`:
- Entferne macOS-Menü-Erstellung (Zeilen 272-293)
- Entferne `CreateTempFolderWithCorrectPermissions()` Windows-Guard (nur noch Linux)
- Entferne Platform-spezifischen Icon-Code

- [ ] **Step 3: MainWindow.h - entferne BookView Referenzen**

In `/data1/u2/sigil074/src/Sigil/MainUI/MainWindow.h`:

Entferne `ViewState_BookView = 10` aus dem Enum (Zeilen 146-150):
```cpp
enum ViewState {
    ViewState_Unknown = 0,
    ViewState_CodeView = 30  // BookView entfernt
};
```

Entferne BookView-spezifische Slots:
- `void BookView();` (Zeile 354)
- `void SetStateActionsBookView();` (Zeile 456)

- [ ] **Step 4: MainWindow.cpp - entferne BookView Implementation**

Entferne alle BookView-spezifischen Implementierungen.

- [ ] **Step 5: Commit**

```bash
cd /data1/u2/sigil074
git add -A
git commit -m "refactor: Remove platform-specific and BookView code from main files

- Remove Windows clipboard workaround
- Remove macOS menu handling
- Remove BookView references from MainWindow"
```

---

## Phase 3: Neue Preview Komponente

### Task 6: Erstelle PreviewWidget Klasse

**Files:**
- Create: `/data1/u2/sigil074/src/Sigil/ViewEditors/PreviewWidget.h`
- Create: `/data1/u2/sigil074/src/Sigil/ViewEditors/PreviewWidget.cpp`
- Modify: `/data1/u2/sigil074/src/Sigil/CMakeLists.txt`

- [ ] **Step 1: Erstelle PreviewWidget.h**

```cpp
/************************************************************************
**
**  Copyright (C) 2026 Sigil Contributors
**
**  This file is part of Sigil.
**
**  Sigil is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  Sigil is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Sigil.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#pragma once
#ifndef PREVIEW_WIDGET_H
#define PREVIEW_WIDGET_H

#include <QWidget>
#include <QWebEngineView>

class QTimer;
class HTMLResource;

/**
 * A read-only preview widget using Qt WebEngine.
 * Displays rendered HTML content synchronized with the CodeView editor.
 */
class PreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidget(QWidget *parent = nullptr);
    ~PreviewWidget();

    /**
     * Set the HTML content to display.
     * @param html The HTML source code
     * @param baseUrl The base URL for resolving relative links
     */
    void setHtml(const QString &html, const QUrl &baseUrl = QUrl());

    /**
     * Update the preview with new HTML content.
     * Uses debouncing to avoid excessive updates.
     */
    void scheduleUpdate(const QString &html, const QUrl &baseUrl = QUrl());

    /**
     * Scroll to an element with the given ID.
     */
    void scrollToElement(const QString &elementId);

    /**
     * Returns the WebEngine view for direct access.
     */
    QWebEngineView *webView() const;

public slots:
    /**
     * Immediately update the preview with pending content.
     */
    void updatePreview();

signals:
    /**
     * Emitted when the user clicks on a link in the preview.
     */
    void linkClicked(const QUrl &url);

    /**
     * Emitted when the preview has finished loading.
     */
    void loadFinished(bool ok);

private slots:
    void onLoadFinished(bool ok);

private:
    void setupUi();
    void connectSignals();

    QWebEngineView *m_webView;
    QTimer *m_updateTimer;
    
    QString m_pendingHtml;
    QUrl m_pendingBaseUrl;
    bool m_isLoading;
};

#endif // PREVIEW_WIDGET_H
```

- [ ] **Step 2: Erstelle PreviewWidget.cpp**

```cpp
/************************************************************************
**
**  Copyright (C) 2026 Sigil Contributors
**
**  This file is part of Sigil.
**
**  Sigil is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  Sigil is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Sigil.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#include "PreviewWidget.h"

#include <QVBoxLayout>
#include <QTimer>
#include <QWebEnginePage>

static const int UPDATE_DEBOUNCE_MS = 400;

PreviewWidget::PreviewWidget(QWidget *parent)
    : QWidget(parent),
      m_webView(nullptr),
      m_updateTimer(nullptr),
      m_isLoading(false)
{
    setupUi();
    connectSignals();
}

PreviewWidget::~PreviewWidget()
{
}

void PreviewWidget::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    m_webView = new QWebEngineView(this);
    m_webView->setContextMenuPolicy(Qt::NoContextMenu);
    
    // Disable editing
    m_webView->page()->setLifecycleState(QWebEnginePage::LifecycleState::Active);
    
    layout->addWidget(m_webView);
    
    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(UPDATE_DEBOUNCE_MS);
}

void PreviewWidget::connectSignals()
{
    connect(m_updateTimer, &QTimer::timeout, this, &PreviewWidget::updatePreview);
    connect(m_webView, &QWebEngineView::loadFinished, this, &PreviewWidget::onLoadFinished);
}

void PreviewWidget::setHtml(const QString &html, const QUrl &baseUrl)
{
    m_pendingHtml = html;
    m_pendingBaseUrl = baseUrl;
    scheduleUpdate(html, baseUrl);
}

void PreviewWidget::scheduleUpdate(const QString &html, const QUrl &baseUrl)
{
    m_pendingHtml = html;
    m_pendingBaseUrl = baseUrl;
    m_updateTimer->start();
}

void PreviewWidget::updatePreview()
{
    if (m_pendingHtml.isEmpty()) {
        return;
    }
    
    m_isLoading = true;
    m_webView->setHtml(m_pendingHtml, m_pendingBaseUrl);
}

void PreviewWidget::onLoadFinished(bool ok)
{
    m_isLoading = false;
    emit loadFinished(ok);
}

void PreviewWidget::scrollToElement(const QString &elementId)
{
    if (elementId.isEmpty()) {
        return;
    }
    
    QString js = QString("document.getElementById('%1').scrollIntoView({behavior: 'smooth', block: 'center'});")
                 .arg(elementId);
    m_webView->page()->runJavaScript(js);
}

QWebEngineView *PreviewWidget::webView() const
{
    return m_webView;
}
```

- [ ] **Step 3: CMakeLists.txt aktualisieren**

Füge zu VIEW_EDITOR_FILES in `/data1/u2/sigil074/src/Sigil/CMakeLists.txt` hinzu:
```cmake
set(VIEW_EDITOR_FILES
    ViewEditors/PreviewWidget.cpp
    ViewEditors/PreviewWidget.h
    ViewEditors/CodeViewEditor.cpp
    ...
)
```

- [ ] **Step 4: Commit**

```bash
cd /data1/u2/sigil074
git add -A
git commit -m "feat: Add PreviewWidget with WebEngine

- New PreviewWidget class using QWebEngineView
- Debounced updates (400ms)
- Read-only preview with scroll synchronization"
```

---

### Task 7: Aktualisiere FlowTab für Split-View

**Files:**
- Modify: `/data1/u2/sigil074/src/Sigil/Tabs/FlowTab.h`
- Modify: `/data1/u2/sigil074/src/Sigil/Tabs/FlowTab.cpp`

- [ ] **Step 1: FlowTab.h - entferne BookView Referenzen, füge Preview hinzu**

In `/data1/u2/sigil074/src/Sigil/Tabs/FlowTab.h`:

Entferne BookView Includes:
```cpp
// LÖSCHEN:
// #include "ViewEditors/BookViewEditor.h"
```

Füge PreviewWidget Include hinzu:
```cpp
#include "ViewEditors/PreviewWidget.h"
```

Ändere Member-Variablen:
```cpp
// LÖSCHEN:
// BookViewEditor *m_BookViewEditor;
// BookViewPreview *m_Preview;

// NEU:
CodeViewEditor *m_CodeViewEditor;
PreviewWidget *m_PreviewWidget;
QSplitter *m_Splitter;
```

Füge Slots hinzu:
```cpp
private slots:
    void onCodeViewTextChanged();
    void onCursorPositionChanged();
    void updatePreview();
    void setSplitterOrientation(Qt::Orientation orientation);
```

- [ ] **Step 2: FlowTab.cpp - implementiere Split-View**

Ersetze den Konstruktor und Setup-Code:

```cpp
FlowTab::FlowTab(HTMLResource *resource, int line_to_scroll_to, 
                 int position_to_scroll_to, QString caret_location_to_scroll_to,
                 MainWindow::ViewState view_state, QUrl fragment, QWidget *parent)
    : ContentTab(*resource, parent),
      m_HTMLResource(resource),
      m_CodeViewEditor(nullptr),
      m_PreviewWidget(nullptr),
      m_Splitter(nullptr),
      m_LoadOkay(false)
{
    setupUi();
    
    // Connect code changes to preview updates
    connect(m_CodeViewEditor, &CodeViewEditor::textChanged, 
            this, &FlowTab::onCodeViewTextChanged);
    connect(m_CodeViewEditor, &CodeViewEditor::cursorPositionChanged,
            this, &FlowTab::onCursorPositionChanged);
    
    // Initial load
    LoadTabContent();
}

void FlowTab::setupUi()
{
    m_Splitter = new QSplitter(Qt::Horizontal, this);
    
    m_CodeViewEditor = new CodeViewEditor(*m_HTMLResource, this);
    m_PreviewWidget = new PreviewWidget(this);
    
    m_Splitter->addWidget(m_CodeViewEditor);
    m_Splitter->addWidget(m_PreviewWidget);
    
    // Set equal sizes
    m_Splitter->setSizes({500, 500});
    
    m_Layout.addWidget(m_Splitter);
}
```

- [ ] **Step 3: Implementiere Preview Update**

```cpp
void FlowTab::onCodeViewTextChanged()
{
    QString html = m_CodeViewEditor->toPlainText();
    m_PreviewWidget->scheduleUpdate(html, m_HTMLResource->GetBaseUrl());
}

void FlowTab::updatePreview()
{
    QString html = m_CodeViewEditor->toPlainText();
    m_PreviewWidget->setHtml(html, m_HTMLResource->GetBaseUrl());
}
```

- [ ] **Step 4: Commit**

```bash
cd /data1/u2/sigil074
git add -A
git commit -m "feat: Update FlowTab for CodeView + Preview split

- Remove BookView integration
- Add QSplitter with CodeView and PreviewWidget
- Implement debounced preview updates
- Add cursor position tracking"
```

---

## Phase 4: Settings & Persistenz

### Task 8: Preview Settings implementieren

**Files:**
- Modify: `/data1/u2/sigil074/src/Sigil/Misc/SettingsStore.h`
- Modify: `/data1/u2/sigil074/src/Sigil/Misc/SettingsStore.cpp`

- [ ] **Step 1: SettingsStore.h - füge Preview-Settings hinzu**

In `/data1/u2/sigil074/src/Sigil/Misc/SettingsStore.h`:

Füge neue Settings-Methoden hinzu:
```cpp
// Preview Settings
bool previewEnabled() const;
void setPreviewEnabled(bool enabled);

int previewOrientation() const; // Qt::Horizontal or Qt::Vertical
void setPreviewOrientation(int orientation);

int previewDebounceMs() const;
void setPreviewDebounceMs(int ms);

bool previewSyncCursor() const;
void setPreviewSyncCursor(bool sync);
```

- [ ] **Step 2: SettingsStore.cpp - implementiere Settings**

```cpp
static const QString KEY_PREVIEW_ENABLED = "Preview/enabled";
static const QString KEY_PREVIEW_ORIENTATION = "Preview/orientation";
static const QString KEY_PREVIEW_DEBOUNCE = "Preview/debounceMs";
static const QString KEY_PREVIEW_SYNC_CURSOR = "Preview/syncCursor";

bool SettingsStore::previewEnabled() const
{
    return settings.value(KEY_PREVIEW_ENABLED, true).toBool();
}

void SettingsStore::setPreviewEnabled(bool enabled)
{
    settings.setValue(KEY_PREVIEW_ENABLED, enabled);
}

int SettingsStore::previewOrientation() const
{
    return settings.value(KEY_PREVIEW_ORIENTATION, Qt::Horizontal).toInt();
}

void SettingsStore::setPreviewOrientation(int orientation)
{
    settings.setValue(KEY_PREVIEW_ORIENTATION, orientation);
}

int SettingsStore::previewDebounceMs() const
{
    return settings.value(KEY_PREVIEW_DEBOUNCE, 400).toInt();
}

void SettingsStore::setPreviewDebounceMs(int ms)
{
    settings.setValue(KEY_PREVIEW_DEBOUNCE, qBound(100, ms, 2000));
}

bool SettingsStore::previewSyncCursor() const
{
    return settings.value(KEY_PREVIEW_SYNC_CURSOR, true).toBool();
}

void SettingsStore::setPreviewSyncCursor(bool sync)
{
    settings.setValue(KEY_PREVIEW_SYNC_CURSOR, sync);
}
```

- [ ] **Step 3: Commit**

```bash
cd /data1/u2/sigil074
git add -A
git commit -m "feat: Add preview settings to SettingsStore

- previewEnabled
- previewOrientation (horizontal/vertical)
- previewDebounceMs (100-2000ms)
- previewSyncCursor"
```

---

## Phase 5: Build & Test

### Task 9: Erster vollständiger Build

**Files:**
- Modify: (Alle geänderten Dateien)

- [ ] **Step 1: Clean Build**

```bash
cd /data1/u2/sigil074
rm -rf build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
```

- [ ] **Step 2: Kompilieren**

```bash
cd /data1/u2/sigil074/build
make -j$(nproc) 2>&1 | tee build.log
```

- [ ] **Step 3: Fehler beheben**

Typische Fehler und Lösungen:

**Fehler: Qt5Komponente nicht gefunden**
```bash
# Prüfe: Muss Qt6 sein
grep -r "Qt5" /data1/u2/sigil074/CMakeLists.txt /data1/u2/sigil074/src/Sigil/CMakeLists.txt
```

**Fehler: WebEngine nicht gefunden**
```bash
# Prüfe Installation
ls /usr/local/Qt-6.9.2/gcc_64/lib/libQt6WebEngine*
```

**Fehler: Missing includes**
- Qt 6 hat manchmal andere Header-Pfade
- z.B. `#include <QWebEngineView>` statt `#include <QtWebEngineWidgets/QWebEngineView>`

- [ ] **Step 4: Erfolgreicher Build**

```bash
# Prüfe Binary
ls -la /data1/u2/sigil074/build/bin/sigil
file /data1/u2/sigil074/build/bin/sigil
```

Expected: ELF 64-bit executable

- [ ] **Step 5: Commit**

```bash
cd /data1/u2/sigil074
git add -A
git commit -m "build: Fix remaining Qt 6 compatibility issues

- Update include paths
- Fix API changes between Qt 5 and Qt 6
- Resolve build errors"
```

---

### Task 10: Runtime Testing

**Files:**
- (Keine neuen Dateien)

- [ ] **Step 1: Anwendung starten**

```bash
cd /data1/u2/sigil074/build/bin
./sigil 2>&1 | head -20
```

Expected: Hauptfenster erscheint, keine Crashes

- [ ] **Step 2: Neues Buch erstellen**

- File → New
- Prüfe: Hauptfenster zeigt sich

- [ ] **Step 3: EPUB öffnen**

- File → Open
- Wähle eine Test-EPUB
- Prüfe: Buch lädt, Dateien erscheinen im Book Browser

- [ ] **Step 4: HTML-Datei editieren**

- Doppelklick auf HTML-Datei
- Prüfe: CodeViewEditor erscheint mit Syntax-Highlighting
- Prüfe: Preview-Panel zeigt gerendertes HTML

- [ ] **Step 5: Live-Preview testen**

- Tippe im CodeView
- Prüfe: Nach 400ms Pause aktualisiert sich Preview

- [ ] **Step 6: Layout umschalten**

- Prüfe: Horizontal/Vertikal umschalten funktioniert

- [ ] **Step 7: Speichern testen**

- File → Save
- Prüfe: Keine Fehler, Datei wird gespeichert

- [ ] **Step 8: Commit**

```bash
cd /data1/u2/sigil074
git add -A
git commit -m "test: Validate Qt 6 migration runtime behavior

- Application starts successfully
- EPUB open/save works
- CodeView editing works
- Preview updates correctly
- Layout switching works"
```

---

## Spec Coverage Check

| Spec Requirement | Implementiert in Task |
|------------------|----------------------|
| Qt 6.9.2 Migration | Task 1, 2 |
| WebKit Entfernung | Task 3, 4, 5 |
| CodeView-Only | Task 7 |
| WebEngine Preview | Task 6, 7 |
| Debounced Updates (400ms) | Task 6, 8 |
| Horizontal/Vertikal Layout | Task 7, 8 |
| Layout Persistenz | Task 8 |
| Linux-Only (kein Windows/macOS Code) | Task 1, 2, 5 |
| C++17 Standard | Task 1 |

---

## Post-Implementation Notes

### Bekannte Limitationen

1. **WYSIWYG Editing entfernt:** Nutzer können nicht mehr direkt im Preview editieren
2. **Preview ist schreibgeschützt:** Keine contentEditable Funktionalität mehr
3. **WebEngine Initialisierung:** Erster Start kann etwas länger dauern (Chromium-Initialisierung)

### Zukünftige Erweiterungen (Out of Scope)

- Bild-Cut/Paste (als nächstes Feature)
- Automatische Stylesheet-Einbindung
- Responsiv-Modus für Preview (Mobile/Tablet Ansicht)

---

**Plan complete.** Saved to `docs/superpowers/plans/2026-04-16-sigil-qt6-migration.md`

**Execution options:**
1. **Subagent-Driven (recommended)** - Fresh subagent per task, review between tasks
2. **Inline Execution** - Batch execution with checkpoints

Ready for implementation.

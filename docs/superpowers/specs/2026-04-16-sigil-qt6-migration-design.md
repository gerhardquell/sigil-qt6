# Sigil Qt 6 Migration Design

**Datum:** 2026-04-16  
**Autor:** Gerhard  
**Scope:** Migration von Qt 5.2 zu Qt 6.9.2, Linux-Only

---

## Zusammenfassung

Migration des Sigil EPUB-Editors von Qt 5.2 auf Qt 6.9.2. Die größte Änderung: Entfernung von Qt WebKit (nicht mehr in Qt 6 verfügbar) und Ersatz durch eine CodeView-Only Architektur mit optionaler WebEngine-Preview.

---

## Tech Stack

### Vorher
- Qt 5.2 (Widgets, WebKit, XML Patterns, SVG)
- CMake 2.8
- C++11

### Nachher
- Qt 6.9.2 (Widgets, WebEngineWidgets, SVG, Network)
- CMake 3.16+ (für Qt 6 erforderlich)
- C++17 (empfohlen für Qt 6)

### Entfernte Qt-Module
- Qt WebKit → nicht mehr verfügbar
- Qt XML Patterns → nicht mehr verfügbar, Code anpassen

### Neue Qt-Module
- Qt WebEngineWidgets → für Preview-Only Ansicht

---

## Architektur-Änderungen

### 1. BookView Entfernung

**Entfernte Klassen:**
- `BookViewEditor` (WYSIWYG Editor)
- `BookViewPreview` (WebKit-basierte Vorschau)
- `ViewWebPage` (WebKit Page Wrapper)

**Betroffene Dateien:** 30 Dateien mit WebKit-Abhängigkeiten

### 2. Neue FlowTab Architektur

```
┌─────────────────────────────────────────────┐
│             FlowTab                         │
│  ┌──────────────────┬───────────────────┐   │
│  │                  │                   │   │
│  │ CodeViewEditor   │ QWebEngineView    │   │
│  │ (QPlainTextEdit) │ (Preview-Only)    │   │
│  │                  │                   │   │
│  │ • Syntax-HL      │ • Schreibgeschützt│   │
│  │ • Auto-Complete  │ • Live-Refresh    │   │
│  │ • Suche/Ersetzen │ • Sync-Scroll     │   │
│  │                  │                   │   │
│  └──────────────────┴───────────────────┘   │
│         ↑ QSplitter (H/V umschaltbar)       │
└─────────────────────────────────────────────┘
```

### 3. Synchronisation Editor ↔ Preview

**Live-Update:**
- Debounce-Timer: 400ms nach letzter Tastatureingabe
- Preview nur aktualisieren wenn Tab sichtbar
- Background-Highlight bei aktivem Parsing

**Positions-Sync:**
- Cursor im Editor → Scroll Preview zu entsprechendem Element
- Klick in Preview → Springe zu Position im Editor
- Element-Highlighting bei Hover

### 4. Layout-Konfiguration

**Umschaltbar:**
- Horizontal (Standard): Editor | Preview
- Vertikal: Editor über Preview

**Persistenz:**
- Layout-Zustand in QSettings speichern
- Pro Dateityp unterschiedliche Defaults möglich

---

## Code-Anpassungen

### CMakeLists.txt Änderungen

```cmake
# Vorher
find_package(Qt5 COMPONENTS Core Network Svg WebKit WebKitWidgets Widgets 
             Xml XmlPatterns Concurrent PrintSupport)

# Nachher  
find_package(Qt6 COMPONENTS Core Network Svg Widgets 
             Xml Concurrent PrintSupport WebEngineWidgets REQUIRED)
```

### C++ Code Änderungen

**Entfernen:**
```cpp
// Alle WebKit Includes
#include <QtWebKit/QWebElement>
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebPage>
#include <QtWebKitWidgets/QWebFrame>

// Alle XML Patterns Includes
#include <QtXmlPatterns/QXmlQuery>
```

**Ersetzen durch:**
```cpp
// WebEngine für Preview
#include <QWebEngineView>
#include <QWebEnginePage>

// XML durch Qt XML (Core) ersetzen
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
```

---

## Linux-Only Vereinfachungen

### Entfallende Build-Code

| Komponente | Zeilen gespart | Grund |
|------------|----------------|-------|
| macOS Bundle | ~100 | Keine .app Erstellung |
| Windows RC | ~50 | Keine Windows-Resourcen |
| macdeployqt | ~30 | Kein Deployment-Script |
| windeployqt | ~100 | Kein Deployment-Script |
| Installer (ISS) | ~200 | Kein Windows-Installer |
| Plattform-Workarounds | ~50 | Keine OS-spezifischen Hacks |

### Vereinfachte main.cpp

**Entfällt:**
- Windows Clipboard-Retry-Workaround
- macOS-spezifische Menü-Behandlung
- `GetApplicationIcon()` für Linux (Qt macht das automatisch)

---

## Settings & Persistenz

### Neue Settings-Schlüssel

```cpp
// Layout
"Preview/enabled" → bool (true/false)
"Preview/orientation" → enum (Horizontal/Vertical)
"Preview/visible" → bool (Preview sichtbar oder ausgeblendet)
"Preview/debounceMs" → int (300-500ms)

// Sync
"Preview/syncCursor" → bool (Cursor-Sync aktiv)
"Preview/syncScroll" → bool (Scroll-Sync aktiv)
```

### Migrierte Settings

Vorhandene Settings bleiben kompatibel:
- Editor-Preferences
- Tastaturkürzel
- Spracheinstellungen

---

## Performance-Überlegungen

### Debounce-Strategie

```
Tastendruck ─┐
             ├──[400ms Timer]──► HTML generieren ──► WebEngine setHtml()
Tastendruck ─┘        (Reset)
```

### Optimierungen

- Preview nur bei sichtbarem Tab aktualisieren
- HTML-Caching für unveränderte Inhalte
- Lazy-Loading der WebEngine (erst bei erster Anzeige initialisieren)

---

## Testing-Strategie

### Manuelle Tests

1. **Build:** CMake konfiguriert und kompiliert ohne Fehler
2. **Start:** Anwendung startet, zeigt Hauptfenster
3. **Öffnen:** EPUB lädt, alle Tabs funktionieren
4. **Editing:** Text bearbeiten, speichern
5. **Preview:** Aktualisiert sich nach 400ms
6. **Layout:** Horizontal/Vertikal umschalten funktioniert
7. **Sync:** Cursor-Position ↔ Preview funktioniert

### Automatisierte Tests (wenn vorhanden)

- Unit-Tests für HTML-Parsing
- Integration-Tests für EPUB Load/Save

---

## Risiken & Mitigation

| Risiko | Wahrscheinlichkeit | Impact | Mitigation |
|--------|-------------------|--------|------------|
| Qt 6 API Breaking Changes | Hoch | Mittel | Kompilieren und iterativ fixen |
| WebEngine fehlt auf System | Niedrig | Hoch | Als REQUIRED in CMake markieren |
| XML Patterns Ersatz komplex | Mittel | Mittel | Schrittweise migrieren |
| Performance-Regression | Niedrig | Mittel | Profiling, Debounce-Optimierung |

---

## Nächste Schritte

1. Implementation Plan erstellen
2. CMake-Modernisierung (Phase 1)
3. Qt 6 Compatibility Layer
4. BookView-Entfernung (Phase 2)
5. WebEngine-Integration (Phase 3)
6. Synchronisation implementieren (Phase 4)
7. Testing & Polish (Phase 5)

---

## Anmerkungen

- Linux-Only vereinfacht Deployment erheblich
- Keine Cross-Platform-Test notwendig
- Fokus auf stabile, performante Linux-Version
- Preview-Only Ansatz reduziert Komplexität massiv

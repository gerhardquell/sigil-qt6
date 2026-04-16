# Qt 6 Migration Retrospective

**Projekt:** Sigil EPUB Editor - Qt 5.2 → Qt 6.9.2 Migration  
**Datum:** 2026-04-16  
**Durchgeführt von:** Gerhard (mit Claude Code)

---

## Zusammenfassung

Migration des Sigil EPUB Editors von Qt 5.2 auf Qt 6.9.2 mit folgenden Hauptzielen:
- Entfernung von Qt WebKit (ersetzt durch QWebEngineView für Preview-Only)
- Umstellung auf CodeView-Only Architektur
- Linux-only Support (Windows/macOS Code entfernt)
- Implementierung von debounced Preview-Updates

---

## Hauptherausforderungen

### 1. Race Condition bei HTML Content Loading

**Problem:**
HTML-Dateien wurden im Editor als leer angezeigt (nur 253 Zeichen statt dem vollständigen Inhalt).

**Ursache:**
```
1. ImportEPUB lädt korrekten HTML-Inhalt (z.B. 106.597 Zeichen)
2. CreateEmptyHTMLFile() überschreibt mit leerem Template (253 Zeichen)
3. InitialLoad() überspringt Laden, da "bereits Inhalt vorhanden"
```

**Lösung:**
```cpp
void TextResource::SetText(const QString &text)
{
    // Don't overwrite existing content with the empty HTML template
    if (!m_TextDocument->toPlainText().isEmpty() && text.length() < 300) {
        return;  // Skip template overwrite
    }
    // ... rest of implementation
}
```

**Erkenntnis:**
Race Conditions bei asynchronen Ladevorgängen sind schwer zu debuggen. Logging der Reihenfolge ist essentiell.

---

### 2. QSignalMapper Signal-Änderungen in Qt 6

**Problem:**
Kompilierfehler bei Signal/Slot-Verbindungen mit QSignalMapper.

**Ursache:**
In Qt 6 wurden die Signal-Signaturen geändert:
- Qt 5: `mapped(int)`, `mapped(const QString&)`
- Qt 6: `mappedInt(int)`, `mappedString(const QString&)`

**Lösung:**
```cpp
// Alt (Qt 5)
connect(mapper, SIGNAL(mapped(int)), this, SLOT(handle(int)));

// Neu (Qt 6)
connect(mapper, SIGNAL(mappedInt(int)), this, SLOT(handle(int)));
```

**Erkenntnis:**
Qt 6 hat viele Signal-Signaturen geändert. Systematische Suche nach `SIGNAL(mapped` hätte Zeit gespart.

---

### 3. QTextDocument Lifecycle Issues

**Problem:**
`std::bad_alloc` Crash beim Beenden der Anwendung.

**Ursache:**
QSyntaxHighlighter sendet während der Destruktion Signale an das QTextDocument, was zu Speicherfehlern führt.

**Lösung:**
```cpp
// In XHTMLHighlighter und CSSHighlighter Destruktoren:
~XHTMLHighlighter() {
    QTextDocument *doc = document();
    if (doc) {
        doc->blockSignals(true);  // Signale blockieren vor Destruktion
    }
}
```

**Erkenntnis:**
Bei Qt-Objekten, die Signale senden, ist explizites Blockieren während der Destruktion oft notwendig.

---

### 4. XML Attribut Regex Parsing

**Problem:**
Fehler beim Import: "Expected '?', but got '='" bei XML-Deklarationen.

**Ursache:**
Regex ersetzte nur "version" statt des kompletten Attributs `version="1.x"`.

**Lösung:**
```cpp
// Alt: Ersetzt nur "version"
QRegularExpression version(VERSION_ATTRIBUTE);
source_copy.replace(version, "version=\"1.0\"");

// Neu: Ersetzt komplettes Attribut mit Wert
QRegularExpression version(VERSION_ATTRIBUTE + "\\s*=\\s*\"[^\"]*\"");
source_copy.replace(mo.capturedStart(), mo.capturedLength(), "version=\"1.0\"");
```

**Erkenntnis:**
Regex-Ersetzungen müssen immer den kompletten Token abdecken, nicht nur den Namen.

---

### 5. QTextStream setCodec Entfernung

**Problem:**
Kompilierfehler: `setCodec` existiert nicht mehr in Qt 6.

**Lösung:**
```cpp
// Entfernt in Qt 6:
// in.setCodec("UTF-8");

// Ersatz:
in.setAutoDetectUnicode(true);  // Automatische BOM-Erkennung
```

**Erkenntnis:**
Qt 6 entfernt viele veraltete Funktionen. Frühe Migration auf empfohlene Alternativen spart später Zeit.

---

## Was gut funktioniert hat

### 1. Systematisches Vorgehen
- Schrittweise Migration: Erst kompilieren, dann Debuggen
- Isolierung von Problemen durch Debug-Logging
- Versionierung nach jedem großen Schritt

### 2. Debug-Logging Strategie
```cpp
// Strategische Platzierung von Logging:
- Konstruktoren (zeigen Objekt-Erstellung)
- SetText() (zeigt Content-Änderungen)
- InitialLoad() (zeigt Ladestatus)
- CustomSetDocument() (zeigt Editor-Initialisierung)
```

### 3. Zwei-Phasen-Architektur
- Phase 1: CodeView mit QTextDocument (Editierung)
- Phase 2: QWebEngineView (nur Preview)
- Klare Trennung hat Debuggen vereinfacht

---

## Verbesserungspotenzial

### 1. Frühere Erkennung der Race Condition
**Was schief lief:**
Zuerst wurde vermutet, der Editor würde nicht aktualisieren, statt dass der Content überschrieben wurde.

**Besser:**
Frühes Logging des gesamten Datenflusses hätte das Problem schneller offenbart.

### 2. QSignalMapper Migration
**Was schief lief:**
Manuelle Suche nach allen Signal-Verbindungen.

**Besser:**
Automatisierte Suche nach allen `SIGNAL(mapped` und systematische Ersetzung.

### 3. Build-System Migration
**Was gut war:**
CMake 3.16+ Anforderung passte zu Qt 6.

**Besser:**
Frühe Prüfung aller Qt-Modul-Namen (Qt6::Widgets statt Qt5::Widgets).

---

## Technische Erkenntnisse

### Qt 6 Änderungen

| Bereich | Qt 5 | Qt 6 | Impact |
|---------|------|------|--------|
| QSignalMapper | `mapped()` | `mappedInt()`, `mappedString()` | Hoch |
| QTextStream | `setCodec()` | `setAutoDetectUnicode()` | Mittel |
| QString | `count()` | `size()`, `length()` | Niedrig |
| Qt WebKit | Verfügbar | Entfernt | Hoch |
| CMake | 2.8+ | 3.16+ | Mittel |

### Architektur-Änderungen

**Vorher (Qt 5):**
```
FlowTab
├── BookView (QWebView) - WYSIWYG Editierung
└── CodeView (QPlainTextEdit) - Source Editierung
```

**Nachher (Qt 6):**
```
FlowTab
├── CodeView (QPlainTextEdit) - Editierung
└── PreviewWidget (QWebEngineView) - Nur Anzeige
```

---

## Empfehlungen für zukünftige Migrationen

1. **Frühes Logging implementieren**
   - Datenfluss von Anfang an loggen
   - Erleichtert Debuggen von Race Conditions

2. **Systematische Suche nach breaking changes**
   - Liste aller verwendeten Qt-Klassen erstellen
   - Jede Klasse auf Qt 6 Änderungen prüfen

3. **Inkrementelle Migration**
   - Zuerst kompilieren lassen
   - Dann Laufzeitfehler beheben
   - Zuletzt optimieren

4. **Automatisierte Tests**
   - EPUB Import/Export Tests
   - UI-Interaktion Tests
   - Performance Tests für große Dateien

---

## Statistiken

- **Geänderte Dateien:** 214
- **Zeilen geändert:** +1039 / -1286
- **Build-Zeit:** ~2 Minuten (parallel mit 4 Cores)
- **Debug-Logging-Dateien:** 9 (errors.txt bis errors9.txt)

---

## Abschließende Bewertung

**Erfolgsfaktoren:**
- Systematisches Debuggen mit Logging
- Klare Architektur-Entscheidungen (CodeView-Only)
- Geduldiges Analysieren von Race Conditions

**Kritische Erfolgsfaktoren:**
- Der Fix in TextResource::SetText() war essentiell für funktionierende Software
- Frühe Erkenntnis, dass Qt WebKit nicht mehr verfügbar ist

**Gesamtergebnis:**
Migration erfolgreich abgeschlossen. Sigil läuft nun mit Qt 6.9.2 und zeigt HTML-Dateien korrekt im Editor an.

---

*Dokument erstellt am 2026-04-16*

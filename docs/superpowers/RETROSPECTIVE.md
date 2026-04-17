# Qt 6 Migration Retrospective

**Projekt:** Sigil EPUB Editor - Qt 5.2 → Qt 6.9.2 Migration
**Durchgeführt von:** Gerhard (mit Claude Code)

---

## Zusammenfassung

Migration des Sigil EPUB Editors von Qt 5.2 auf Qt 6.9.2 mit folgenden Hauptzielen:
- Entfernung von Qt WebKit (ersetzt durch QWebEngineView für Preview-Only)
- Umstellung auf CodeView-Only Architektur
- Linux-only Support (Windows/macOS Code entfernt)
- Implementierung von debounced Preview-Updates

---

## Phase 1: Kompilierung und Basis-Funktionalität (2026-04-16)

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

## Phase 2: Preview-Panel Funktionalität (2026-04-17)

### 6. QWebEngineView im Splitter zusammengeklappt

**Problem:**
Das Preview-Panel war unsichtbar — der QSplitter gab dem Preview-Widget 0 Pixel Breite.

**Ursache:**
QWebEngineView hat eine winzige Default-sizeHint (QSize(0,0) in Qt 6). Ohne explizite SizePolicy und MinimumSize kollabiert das Widget im Splitter.

**Lösung:**
```cpp
m_webView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
m_webView->setMinimumSize(200, 200);
```

**Erkenntnis:**
QWebEngineView verhält sich anders als QWebView — es hat keine sinnvolle Default-Größe. Immer SizePolicy + MinimumSize setzen.

---

### 7. sigil:// Custom URL Scheme als Base URL funktioniert nicht

**Problem:**
`setHtml(html, QUrl("sigil://book/OEBPS/Text/"))` führte dazu, dass loadFinished nie gefeuert wurde und die Seite leer blieb.

**Ursache:**
Chromium behandelt Custom URL Schemes als "opaque origins". Das bedeutet:
- Keine Sub-Resource-Loading (CSS, Bilder)
- loadFinished wird nie gefeuert
- JavaScript wird nicht ausgeführt

**Lösung:**
Statt sigil:// wird file:// als Base URL verwendet:
```cpp
QUrl baseUrl = QUrl::fromLocalFile(m_basePath + "/dummy.xhtml");
m_webView->setHtml(m_pendingHtml, baseUrl);
```
Chromium resolved relative URLs (z.B. `../Styles/stylesheet.css`) korrekt gegen file://.

**Erkenntnis:**
Custom URL Schemes in QWebEngineView sind nur für QWebEngineUrlSchemeHandler nutzbar, NICHT als Base URL für setHtml(). Für Preview ist file:// die richtige Wahl.

---

### 8. m_isLoading Deadlock

**Problem:**
Preview wurde nie aktualisiert — Log zeigte endlos "skipped (already loading), rescheduling".

**Ursache:**
Weil sigil:// als opaque origin loadFinished nie feuerte, blieb `m_isLoading` für immer `true`. Der Gate in updatePreview() blockierte alle Updates.

**Lösung:**
`m_isLoading` als Blocking-Gate entfernt. Mit file:// als Base URL feuert loadFinished korrekt, daher ist der Gate nicht mehr nötig.

**Erkenntnis:**
Blocking-Gates, die von asynchronen Signalen zurückgesetzt werden, sind fehleranfällig. Wenn das Signal nie kommt, blockiert alles.

---

### 9. convertUrlsToScheme() Position-Invalidierung

**Problem:**
URL-Konvertierung in convertUrlsToScheme() produzierte falsche Ergebnisse — manche URLs wurden doppelt ersetzt, andere gar nicht.

**Ursache:**
Vorwärts-Iteration mit `QString::replace()` invalidierte die Positionen der noch nicht verarbeiteten Matches. Sobald ein Match ersetzt wurde, verschoben sich alle folgenden Positionen.

**Lösung:**
Reverse-Order-Replacement: Alle Matches sammeln, dann in umgekehrter Reihenfolge ersetzen:
```cpp
QList<QRegularExpressionMatch> matches;
QRegularExpressionMatchIterator it = regex.globalMatch(result);
while (it.hasNext()) {
    matches.append(it.next());
}
for (int i = matches.size() - 1; i >= 0; --i) {
    result.replace(match.capturedStart(), match.capturedLength(), ...);
}
```

**Erkenntnis:**
Bei Multi-Match-Replacement immer von hinten nach vorne arbeiten. Allgemein bekannt, aber leicht zu vergessen.

---

### 10. linkHovered als linkClicked — Maus-Hover löst Navigation aus

**Problem:**
Beim Darüberfahren mit der Maus über Links im Preview wurde die Navigation ausgelöst, als hätte man geklickt.

**Ursache:**
`QWebEngineView::linkHovered` war mit `PreviewWidget::linkClicked` verbunden. In Qt WebEngine ist linkHovered ein Hover-Event, kein Click-Event.

**Lösung:**
PreviewPage-Subclass von QWebEnginePage mit acceptNavigationRequest() Override:
```cpp
class PreviewPage : public QWebEnginePage {
    Q_OBJECT
signals:
    void linkClicked(const QUrl &url);
protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override
    {
        if (type == NavigationTypeLinkClicked) {
            emit linkClicked(url);
            return false;  // Block navigation
        }
        return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
    }
};
```

**Erkenntnis:**
In Qt WebEngine ist acceptNavigationRequest() der einzige zuverlässige Weg, Link-Klicks abzufangen. linkHovered ist nur für Tooltips/Statusleiste.

---

### 11. QComboBox activated → textActivated in Qt 6

**Problem:**
HeadingSelector: Signal `activated(const QString &)` existiert nicht mehr in Qt 6.

**Ursache:**
Qt 6 hat die überladenen Signale aufgetrennt:
- Qt 5: `activated(int)` und `activated(const QString &)`
- Qt 6: `activated(int)` und `textActivated(const QString &)`

**Lösung:**
```cpp
// Alt (Qt 5)
SIGNAL(activated(const QString &))

// Neu (Qt 6)
SIGNAL(textActivated(const QString &))
```

**Erkenntnis:**
Qt 6 hat viele überladene Signale umbenannt. Systematische Suche nach `SIGNAL(activated(` wäre hilfreich.

---

### 12. Doppelte Preview-Updates

**Problem:**
Bei jeder Code-Änderung wurde das Preview zweimal aktualisiert.

**Ursache:**
Mehrere Signal-Pfade (DocumentSet, CursorMoved, PageClicked) lösten unabhängig updatePreview() aus.

**Lösung:**
Alle Preview-Updates laufen über den debounced onCodeViewTextChanged() (400ms Timer). Separate Verbindungen entfernt.

**Erkenntnis:**
Debouncing ist bei QWebEngineView essentiell — jeder setHtml()-Aufruf ist teuer. Zentraler Update-Pfad verhindert Duplikation.

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
- SetText() (zeigen Content-Änderungen)
- InitialLoad() (zeigen Ladestatus)
- CustomSetDocument() (zeigen Editor-Initialisierung)
// Stderr mit grep-freundlichem Prefix:
#define DBG_PREFIX "[SIGIL-PREVIEW] "
fprintf(stderr, DBG_PREFIX "updatePreview: HTML %d chars\n", html.size());
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
| QWebEngineView | — | sizeHint=0x0, SizePolicy nötig | Hoch |
| Custom URL Schemes | — | Opaque origins, nicht als Base URL nutzbar | Hoch |
| Link-Interception | QWebView::linkClicked | QWebEnginePage::acceptNavigationRequest() | Hoch |
| QComboBox | `activated(QString)` | `textActivated(QString)` | Mittel |

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
└── PreviewWidget
    └── PreviewPage (QWebEnginePage)
        └── QWebEngineView - Nur Anzeige
            ├── file:// Base URL für Ressource-Loading
            ├── acceptNavigationRequest() für Link-Interception
            └── 400ms Debounce-Timer für Updates
```

### Preview Ressource-Loading: sigil:// vs file://

| Ansatz | sigil:// Scheme | file:// Base URL |
|--------|-----------------|-------------------|
| Sub-Resources (CSS, Bilder) | Nur via SchemeHandler | Nativ durch Chromium |
| loadFinished Signal | Nie (opaque origin) | Korrekt |
| JavaScript | Blockiert | Funktioniert |
| Komplexität | Hoch (Handler registrieren) | Niedrig (nur Pfad setzen) |
| Sicherheit | Eingeschränkter Zugriff | Zugriff auf lokale Dateien |

---

## Empfehlungen für zukünftige Qt WebEngine Arbeit

1. **file:// Base URL bevorzugen** — Chromium resolved relative URLs nativ, kein Custom Scheme nötig
2. **SizePolicy immer setzen** — QWebEngineView hat keine sinnvolle Default-Größe
3. **acceptNavigationRequest() für Link-Handling** — Nicht linkHovered oder urlChanged
4. **Debouncing ist Pflicht** — setHtml() ist teuer, 400ms Timer verhindert Überlastung
5. **Debug-Output über stderr** — `fprintf(stderr, "[PREFIX] ...")` ist grep-freundlich

---

## Statistiken

- **Phase 1 Geänderte Dateien:** 214 (+1039 / -1286)
- **Phase 2 Geänderte Dateien:** 7 (+258 / -20)
- **Build-Zeit:** ~2 Minuten (parallel mit 4 Cores)
- **Debug-Logging-Dateien:** 9 (errors.txt bis errors9.txt)

---

## Abschließende Bewertung

**Erfolgsfaktoren:**
- Systematisches Debuggen mit Logging
- Klare Architektur-Entscheidungen (CodeView-Only)
- Geduldiges Analysieren von Race Conditions
- Debounced Preview-Updates verhindern Überlastung

**Kritische Erkenntnisse:**
- Qt WebEngine ≠ Qt WebKit — grundlegend andere Architektur
- Custom URL Schemes sind in Chromium opaque origins
- QWebEngineView braucht explizite Size-Konfiguration
- acceptNavigationRequest() ist der einzige zuverlässige Weg für Link-Interception

**Gesamtergebnis:**
Migration erfolgreich abgeschlossen. Sigil läuft mit Qt 6.9.2 — Editor und Preview funktionieren korrekt.

---

*Dokument erstellt am 2026-04-16, aktualisiert am 2026-04-17*

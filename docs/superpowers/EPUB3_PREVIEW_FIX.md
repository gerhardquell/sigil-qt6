# EPUB3 Preview Fix - Arbeitsprotokoll

**Datum:** 2026-04-17  
**Status:** In Bearbeitung - Preview nicht sichtbar  
**Ziel:** CSS, Bilder, Audio, Video, Fonts in der Preview korrekt laden

---

## Was wurde bisher implementiert:

### 1. SigilUrlSchemeHandler (NEU)
- **Dateien:** `src/Sigil/ViewEditors/SigilUrlSchemeHandler.h/.cpp`
- **Zweck:** Custom URL Scheme `sigil://` für WebEngine Preview
- **Unterstützt:** CSS, Bilder (jpg/png/gif/svg), Audio (mp3/aac/ogg), Video (mp4/webm), Fonts (woff/woff2/ttf/otf), JavaScript
- **Registrierung:** In `main.cpp` vor QApplication-Erstellung

### 2. PreviewWidget Erweiterungen
- **URL-Konvertierung:** `convertUrlsToScheme()` wandelt relative URLs in `sigil://book/path` um
- **Setup-Handler:** `setupUrlHandler()` registriert den Handler beim WebEngine-Profil
- **Debounced Updates:** 400ms Timer für Preview-Updates

### 3. FlowTab Integration
- **Includes:** `QFileInfo`, `QDir` hinzugefügt
- **DelayedInitialization:** Ruft `setupUrlHandler()` mit OEBPS-Pfad auf

### 4. CMakeLists.txt
- **Neue Dateien:** `SigilUrlSchemeHandler.cpp/.cpp` zu `VIEW_EDITOR_FILES` hinzugefügt

---

## Bekannte Probleme:

### KRITISCH: Preview wird nicht angezeigt
- **Symptom:** Kein Preview-Panel sichtbar
- **Ursache (vermutet):** 
  - Sichtbarkeit des Splitters/Widgets wird nicht korrekt gesetzt
  - ODER: WebEngine initialisiert nicht korrekt
  - ODER: Problem im Konstruktor/DelayedInitialization

**Debug-Logs zeigen:**
```
Splitter visible: false
Preview visible: false
Preview size: QSize(200, 30)
```

---

## Nächste Schritte (TODO):

### Option 1: Minimaler Test
1. `git checkout HEAD -- src/Sigil/Tabs/FlowTab.cpp` (Original wiederherstellen)
2. Nur die Includes für QFileInfo/QDir behalten
3. Nur `setupUrlHandler()` Aufruf in DelayedInitialization hinzufügen
4. Testen ob Preview angezeigt wird

### Option 2: Debugging
1. In `PreviewWidget::setupUi()` prüfen ob `m_webView` erstellt wird
2. In `FlowTab::CreateEditors()` prüfen ob Widgets zum Splitter hinzugefügt werden
3. `qDebug()` hinzufügen um Sichtbarkeit zu tracken

### Option 3: Alternative Implementierung
Statt `sigil://` URL Scheme:
- HTML-Inhalt parsen und CSS inline injizieren
- Bilder als Data-URLs einbetten
- Kein URL Handler nötig

---

## Relevante Dateien:

| Datei | Status | Änderung |
|-------|--------|----------|
| `src/Sigil/ViewEditors/SigilUrlSchemeHandler.h` | NEU | URL Scheme Handler |
| `src/Sigil/ViewEditors/SigilUrlSchemeHandler.cpp` | NEU | Implementation |
| `src/Sigil/ViewEditors/PreviewWidget.h` | MOD | `setupUrlHandler()` Methode |
| `src/Sigil/ViewEditors/PreviewWidget.cpp` | MOD | URL-Konvertierung |
| `src/Sigil/Tabs/FlowTab.cpp` | MOD | `setupUrlHandler()` Aufruf |
| `src/Sigil/CMakeLists.txt` | MOD | Neue Dateien hinzugefügt |
| `src/Sigil/main.cpp` | MOD | Scheme-Registrierung |

---

## Letzter Build:
- **Status:** ✅ Erfolgreich
- **Warnungen:** Nur deprecated `QString::count()`
- **Binary:** `build/bin/sigil`

---

## Test-Befehl:
```bash
cd /data1/u2/sigil074/build/bin
./sigil 2>&1 | tee errors1.txt
```

---

## Wichtige Erkenntnis:
Am Anfang des Tages funktionierte die Preview-Anzeige, aber CSS/Bilder wurden nicht geladen. Irgendwo auf dem Weg wurde die Anzeige "zerstört". Empfohlener Ansatz: Auf den letzten funktionierenden Stand zurücksetzen und nur das URL-Handling hinzufügen.

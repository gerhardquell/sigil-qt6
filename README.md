# Sigil Qt 6

Sigil EPUB Editor — Qt 6 Migration based on Sigil 0.7.4

## About

This is a Qt 6.9.2 port of [Sigil](https://github.com/user-none/Sigil) (version 0.7.4), the free open-source EPUB editor. The original Sigil 0.7.4 was built with Qt 5.2 and Qt WebKit. This fork replaces WebKit with QWebEngineView and modernizes the codebase for Qt 6.

## Changes from Original

- **Qt 6.9.2** — Full migration from Qt 5.2
- **CodeView-Only** — BookView (WebKit WYSIWYG) replaced with read-only Preview panel (QWebEngineView)
- **Preview Panel** — Split view: CodeView editor + live Preview, debounced updates (400ms)
- **User Templates** — Customizable templates from `~/.sigil/`:
  - `default.xhtml` — HTML template for new sections
  - `style.css` — Default stylesheet
  - `impressum.xhtml` — Automatically added as first page in new EPUBs
- **Paste Image** — Paste images from clipboard as PNG via right-click in Images folder
- **13pt UI Font** — Application-wide font size for readability
- **Linux-only** — Windows/macOS platform code removed

## Requirements

- Qt 6.9.2 (QtWidgets, QtCore, QtWebEngineWidgets)
- CMake 3.16+
- C++11 compiler (GCC 12+)
- Xerces-C++ 3.1+
- Apache Xerces (bundled)

## Build

```bash
mkdir build && cd build
cmake ..
make -j4
```

The binary is at `build/bin/sigil`.

## User Templates

Create `~/.sigil/` and place template files:

```bash
mkdir -p ~/.sigil
```

| File | Purpose | Fallback |
|------|---------|----------|
| `~/.sigil/default.xhtml` | HTML template for new sections | Hardcoded XHTML 1.1 template |
| `~/.sigil/style.css` | Default stylesheet | Empty |
| `~/.sigil/impressum.xhtml` | Impressum page (full XHTML) | No impressum created |

CSS links in XHTML templates are automatically adjusted to match the EPUB-internal filename (e.g. `Style0001.css`).

## License

GNU General Public License v3 — see [COPYING.txt](COPYING.txt)

Original Sigil Copyright (C) 2009-2013 Strahinja Markovic and contributors.
Qt 6 migration Copyright (C) 2026 Gerhard Quell.

#include "UserTemplates.h"
#include "Misc/Utility.h"

#include <QDir>
#include <QFile>
#include <QRegularExpression>

static const QString FALLBACK_HTML =
    "<?xml version=\"1.0\" encoding=\"utf-8\"?> "
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" "
    "    \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">  "
    "<html xmlns=\"http://www.w3.org/1999/xhtml\"> "
    "<head> "
    "<title></title> "
    "</head> "
    "<body> "
    "<p>&#160;</p> "
    "</body> "
    "</html>";

UserTemplates &UserTemplates::instance()
{
    static UserTemplates inst;
    return inst;
}

QString UserTemplates::configDir()
{
    return QDir::homePath() + "/.sigil";
}

UserTemplates::UserTemplates()
    : m_defaultHtml(FALLBACK_HTML),
      m_defaultCss(""),
      m_impressumHtml(""),
      m_hasImpressum(false)
{
    QString dir = configDir();
    QDir config(dir);

    if (!config.exists()) {
        return;
    }

    QString html = readFile("default.xhtml");
    if (!html.isEmpty()) {
        m_defaultHtml = html;
    }

    QString css = readFile("style.css");
    if (!css.isEmpty()) {
        m_defaultCss = css;
    }

    QString impressum = readFile("impressum.xhtml");
    if (!impressum.isEmpty()) {
        m_impressumHtml = impressum;
        m_hasImpressum = true;
    }
}

QString UserTemplates::defaultHtml() const
{
    return m_defaultHtml;
}

QString UserTemplates::defaultCss() const
{
    return m_defaultCss;
}

QString UserTemplates::impressumHtml() const
{
    return m_impressumHtml;
}

bool UserTemplates::hasImpressum() const
{
    return m_hasImpressum;
}

QString UserTemplates::adjustCssLinks(const QString &html, const QString &cssFilename) const
{
    QString result = html;

    // Match href attributes in <link> tags pointing to .css files
    // e.g. href="../Styles/Style.css" -> href="../Styles/Style0001.css"
    static const QRegularExpression cssHrefRegex(
        R"((href\s*=\s*["'])([^"']*?/)([^/"']+\.css)(["']))",
        QRegularExpression::CaseInsensitiveOption);

    QList<QRegularExpressionMatch> matches;
    QRegularExpressionMatchIterator it = cssHrefRegex.globalMatch(result);
    while (it.hasNext()) {
        matches.append(it.next());
    }

    for (int i = matches.size() - 1; i >= 0; --i) {
        const QRegularExpressionMatch &match = matches[i];
        QString prefix = match.captured(1);   // href="
        QString path = match.captured(2);      // ../Styles/
        QString oldName = match.captured(3);   // Style.css
        QString suffix = match.captured(4);    // "

        result.replace(match.capturedStart(), match.capturedLength(),
                       prefix + path + cssFilename + suffix);
    }

    return result;
}

QString UserTemplates::readFile(const QString &filename) const
{
    QString filepath = configDir() + "/" + filename;
    QFile file(filepath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    return content;
}

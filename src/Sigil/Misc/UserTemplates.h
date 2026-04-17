#ifndef USER_TEMPLATES_H
#define USER_TEMPLATES_H

#include <QString>

class UserTemplates
{
public:
    static UserTemplates& instance();

    QString defaultHtml() const;
    QString defaultCss() const;
    QString impressumHtml() const;
    bool hasImpressum() const;

    QString adjustCssLinks(const QString &html, const QString &cssFilename) const;

    static QString configDir();

private:
    UserTemplates();
    UserTemplates(const UserTemplates&) = delete;
    UserTemplates& operator=(const UserTemplates&) = delete;

    QString readFile(const QString &filename) const;

    QString m_defaultHtml;
    QString m_defaultCss;
    QString m_impressumHtml;
    bool m_hasImpressum;
};

#endif

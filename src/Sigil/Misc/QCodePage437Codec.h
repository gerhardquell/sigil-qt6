/****************************************************************************
** Qt 6 STUB: QTextCodec removed
** This file is a stub for CodePage 437 codec support
** TODO: Reimplement using QStringConverter if needed
****************************************************************************/

#pragma once
#ifndef QCODEPAGE437CODEC_H
#define QCODEPAGE437CODEC_H

#include <QString>

// Qt 6: QTextCodec removed, this is a stub
class QCodePage437Codec
{
public:
    QCodePage437Codec() {}
    ~QCodePage437Codec() {}

    // Stub: just return the input without conversion
    QString toUnicode(const QByteArray &input) const {
        return QString::fromLatin1(input);
    }

    QByteArray fromUnicode(const QString &input) const {
        return input.toLatin1();
    }
};

#endif // QCODEPAGE437CODEC_H

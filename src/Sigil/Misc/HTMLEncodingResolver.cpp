/************************************************************************
**
**  Copyright (C) 2013              John Schember <john@nachtimwald.com>
**  Copyright (C) 2009, 2010, 2011  Strahinja Markovic  <strahinja.markovic@gmail.com>
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

#include <QFile>
#include <QString>
#include <QRegularExpression>

#include "Misc/HTMLEncodingResolver.h"
#include "Misc/Utility.h"
#include "sigil_constants.h"
#include "sigil_exception.h"


const QString CHARSET_ATTRIBUTE    = "charset\\s*=\\s*(?:\"|')([^\"']+)(?:\"|')";


// Accepts a full path to an HTML file.
// Reads the file, detects the encoding
// and returns the text converted to Unicode.
QString HTMLEncodingResolver::ReadHTMLFile(const QString &fullfilepath)
{
    QFile file(fullfilepath);

    // Check if we can open the file
    if (!file.open(QFile::ReadOnly)) {
        boost_throw(CannotOpenFile()
                    << errinfo_file_fullpath(file.fileName().toStdString())
                    << errinfo_file_errorstring(file.errorString().toStdString())
                   );
    }

    QByteArray data = file.readAll();

    if (IsValidUtf8(data)) {
        data.replace("\xC2\xA0", "&#160;");
    }

    // Qt 6: QTextCodec removed, using QString::fromUtf8
    // TODO: Reimplement proper encoding detection using QStringConverter
    return Utility::ConvertLineEndings(QString::fromUtf8(data));
}


// Qt 6 STUB: QTextCodec removed
// Accepts an HTML stream and tries to determine its encoding;
// Returns "UTF-8" always for now.
QString HTMLEncodingResolver::GetCodecForHTML(const QByteArray &raw_text)
{
    Q_UNUSED(raw_text)
    // TODO: Reimplement encoding detection
    // For now, assume UTF-8
    return QString("UTF-8");
}


// This function goes through the entire byte array
// and tries to see whether this is a valid UTF-8 sequence.
// If it's valid, this is probably a UTF-8 string.
bool HTMLEncodingResolver::IsValidUtf8(const QByteArray &string)
{
    // %x00-7F - all plain ASCII characters are valid
    // %xC0-DF one byte from the lowest UTF-8 block follows
    // %xE0-EF two bytes from the middle UTF-8 block follow
    // %xF0-F7 three bytes from the highest UTF-8 block follow
    // %x80-BF the bytes following the beginning of a UTF-8 block
    // %xC0, %xC1: these are technically overlong encodings
    //             but we will let them through
    // %xF8-FF: these are invalid UTF-8 bytes
    QRegularExpression utf8_checker(
        "^(?:[\\x00-\\x7F]|"                                    // ASCII
        "[\\xC0-\\xDF][\\x80-\\xBF]|"                         // 2-byte UTF-8
        "[\\xE0-\\xEF][\\x80-\\xBF]{2}|"                      // 3-byte UTF-8
        "[\\xF0-\\xF7][\\x80-\\xBF]{3})*$");                   // 4-byte UTF-8
    QRegularExpressionMatch match = utf8_checker.match(string);
    return match.hasMatch();
}

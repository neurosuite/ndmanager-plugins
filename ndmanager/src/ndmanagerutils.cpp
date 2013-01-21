/* code from Qt5 qstandardpaths.cpp
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
*/

/* code from Qt5 qstandardpaths.cpp */

#include "ndmanagerutils.h"
#include <QFileInfo>
#include <QDir>

static QString checkExecutable(const QString &path)
{
    const QFileInfo info(path);
    if (info.isBundle())
        return info.bundleName();
    if (info.isFile() && info.isExecutable())
        return QDir::cleanPath(path);
    return QString();
}

#ifdef Q_OS_WIN
static QStringList executableExtensions()
{
    QStringList ret = QString::fromLocal8Bit(qgetenv("PATHEXT")).split(QLatin1Char(';'));
    if (!ret.contains(QLatin1String(".exe"), Qt::CaseInsensitive)) {
        // If %PATHEXT% does not contain .exe, it is either empty, malformed, or distorted in ways that we cannot support, anyway.
        ret.clear();
        ret << QLatin1String(".exe")
            << QLatin1String(".com")
            << QLatin1String(".bat")
            << QLatin1String(".cmd");
    }
    return ret;
}
#endif

QString NdManagerUtils::findExecutable(const QString &executableName, const QStringList &paths)
{
    if (QFileInfo(executableName).isAbsolute())
        return checkExecutable(executableName);

    QStringList searchPaths = paths;
    if (paths.isEmpty()) {
        QByteArray pEnv = qgetenv("PATH");
#if defined(Q_OS_WIN)
        const QLatin1Char pathSep(';');
#else
        const QLatin1Char pathSep(':');
#endif
        searchPaths = QString::fromLocal8Bit(pEnv.constData()).split(pathSep, QString::SkipEmptyParts);
    }

    QDir currentDir = QDir::current();
    QString absPath;
#ifdef Q_OS_WIN
    static QStringList executable_extensions = executableExtensions();
#endif

    for (QStringList::const_iterator p = searchPaths.constBegin(); p != searchPaths.constEnd(); ++p) {
        const QString candidate = currentDir.absoluteFilePath(*p + QLatin1Char('/') + executableName);
#ifdef Q_OS_WIN
        const QString extension = QLatin1Char('.') + QFileInfo(executableName).suffix();
        if (!executable_extensions.contains(extension, Qt::CaseInsensitive)) {
            foreach (const QString &extension, executable_extensions) {
                absPath = checkExecutable(candidate + extension.toLower());
                if (!absPath.isEmpty())
                    break;
            }
        }
#endif
        absPath = checkExecutable(candidate);
        if (!absPath.isEmpty()) {
            break;
        }
    }
    return absPath;

}

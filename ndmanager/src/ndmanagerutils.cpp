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
#include <QDebug>

#ifdef Q_OS_WIN
static QStringList executableExtensions()
{
    // If %PATHEXT% does not contain .exe, it is either empty, malformed, or distorted in ways that we cannot support, anyway.
    const QStringList pathExt = QString::fromLocal8Bit(qgetenv("PATHEXT")).toLower().split(QLatin1Char(';'));
    return pathExt.contains(QLatin1String(".exe"), Qt::CaseInsensitive) ?
           pathExt :
           QStringList() << QLatin1String(".exe") << QLatin1String(".com")
                         << QLatin1String(".bat") << QLatin1String(".cmd");
}
#endif

static QString checkExecutable(const QString &path)
{
    const QFileInfo info(path);
    if (info.isBundle())
        return info.bundleName();
    if (info.isFile() && info.isExecutable())
        return QDir::cleanPath(path);
    return QString();
}

static inline QString searchExecutable(const QStringList &searchPaths,
                                       const QString &executableName)
{
    const QDir currentDir = QDir::current();
    foreach (const QString &searchPath, searchPaths) {
        const QString candidate = currentDir.absoluteFilePath(searchPath + QLatin1Char('/') + executableName);
        const QString absPath = checkExecutable(candidate);
        if (!absPath.isEmpty())
            return absPath;
    }
    return QString();
}
#ifdef Q_OS_WIN

// Find executable appending candidate suffixes, used for suffix-less executables
// on Windows.
static inline QString
    searchExecutableAppendSuffix(const QStringList &searchPaths,
                                 const QString &executableName,
                                 const QStringList &suffixes)
{
    const QDir currentDir = QDir::current();
    foreach (const QString &searchPath, searchPaths) {
        const QString candidateRoot = currentDir.absoluteFilePath(searchPath + QLatin1Char('/') + executableName);
        foreach (const QString &suffix, suffixes) {
            const QString absPath = checkExecutable(candidateRoot + suffix);
            if (!absPath.isEmpty())
                return absPath;
        }
    }
    return QString();
}

#endif // Q_OS_WIN

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
        // Remove trailing slashes, which occur on Windows.
        const QStringList rawPaths = QString::fromLocal8Bit(pEnv.constData()).split(pathSep, QString::SkipEmptyParts);
        searchPaths.reserve(rawPaths.size());
        foreach (const QString &rawPath, rawPaths) {
            QString cleanPath = QDir::cleanPath(rawPath);
            if (cleanPath.size() > 1 && cleanPath.endsWith(QLatin1Char('/')))
                cleanPath.truncate(cleanPath.size() - 1);
            searchPaths.push_back(cleanPath);
        }
    }

#ifdef Q_OS_WIN
    // On Windows, if the name does not have a suffix or a suffix not
    // in PATHEXT ("xx.foo"), append suffixes from PATHEXT.
    static const QStringList executable_extensions = executableExtensions();
    if (executableName.contains(QLatin1Char('.'))) {
        const QString suffix = QFileInfo(executableName).suffix();
        if (suffix.isEmpty() || !executable_extensions.contains(QLatin1Char('.') + suffix, Qt::CaseInsensitive))
            return searchExecutableAppendSuffix(searchPaths, executableName, executable_extensions);
    } else {
        return searchExecutableAppendSuffix(searchPaths, executableName, executable_extensions);
    }
#endif
    return searchExecutable(searchPaths, executableName);
}

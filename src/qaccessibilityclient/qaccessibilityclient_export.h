/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef QACCESSIBILITYCLIENT_EXPORT_H
#define QACCESSIBILITYCLIENT_EXPORT_H

#include <QtGlobal>

#ifndef QACCESSIBILITYCLIENT_EXPORT
# if defined(QACCESSIBILITYCLIENT_LIBRARY)
/* We are building this library */
#  define QACCESSIBILITYCLIENT_EXPORT Q_DECL_EXPORT
# else
/* We are using this library */
#  define QACCESSIBILITYCLIENT_EXPORT Q_DECL_IMPORT
# endif
#endif

#ifndef QACCESSIBILITYCLIENT_EXPORT_DEPRECATED
# define QACCESSIBILITYCLIENT_EXPORT_DEPRECATED Q_DECL_DEPRECATED QACCESSIBILITYCLIENT_EXPORT
#endif

#endif


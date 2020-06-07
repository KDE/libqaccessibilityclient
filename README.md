QAccessibilityClient
====================

This library is used when writing accessibility clients such as screen readers.
It comes with some examples demonstrating the API. These small helpers may be useful when testing accessibility.
One of them writes all accessibiliy interfaces an application provides as text output.
The other, more advanced application shows a tree of objects and allows some interaction and exploration.

Coding style: <https://community.kde.org/Policies/Frameworks_Coding_Style>
Generally this library follows <https://community.kde.org/Policies/Library_Code_Policy>

Be aware that it is in an early stage of development right now so no compatibility is guaranteered.

All public API is in:
src/qaccessibilityclient

The abstraction of DBus and related at-spi tools should go into
src/atspi
but not exported.

You can generate documentation (see docs folder README).

Using CMake with QAccessibilityClient
-------------------------------------

Use:

```cmake
find_package(QAccessibilityClient)
set_package_properties(QAccessibilityClient PROPERTIES
    DESCRIPTION "Client-side accessibility library for Qt"
    URL "https://commits.kde.org/libqaccessibilityclient"
    TYPE REQUIRED
    PURPOSE "Required to enable accessibility features."
)

target_link_libraries(targetname qaccessibilityclient)
```

In your code `#include <qaccessibilityclient/accessibleobject.h>` etc.

Creating releases
-----------------

There are no translations for QAccessibilityClient since the library does not contain any strings that should be shown to users.
The examples are not translated, that could potentially be considered.
Follow <https://community.kde.org/ReleasingSoftware>, skip the translation parts.

* Set the project version in the top-level CMakeLists.txt, push.
* `./tarme.rb --version 0.3.0 --origin stable libqaccessibilityclient`
* tarme.rb has nice output that gives instructions.
* `./tagme.rb --version 0.3.0 libqaccessibilityclient`
* upload as directed
* file sysadmin ticket
* `./logme.rb` to create changelog
* `./tagme --version 0.3.0` to push tag
* blog/send mail

# QtUtils

A collection of utilities for (not only) Qt development. At this moment it contains only a few header-only classes which can be used anywhere in your code. The code is intended to be simple, light-weight, pragmatic and easy to understand. More stuff will be added later on...

Layouts
-------
File: [`layouts.h`](qtutils/layouts.h)<br>
Dependency: QtWidgets<br>
License: MIT

I hate creating complicated layouts in QtDesigner as much as you do. If you tried it at least once, you know what I am talking about. But I also hate writing and reading complex and verbose procedural code. So I created a few utility classes in [`layouts.h`](qtutils/layouts.h) which will allow writing more concise layout-related code in somewhat declarative style. Currently it only contains helpers for `QVBoxLayout` and `QHBoxLayout`, which however account for more than 90 % of all layouts in my code.

There are two templated wrappers `VBox` and `HBox` around pointers to `QVBoxLayout` and `QHBoxLayout`. These wrappers allow implicit conversions to the undelying layout pointers, they provide `<<` operators with which you can add child widgets or child layouts in a declarative way. You can also add stretches (using `Stretch`) or spacings (using `Spacing`). You can also easily define margins and default spacing for the layout in wrapper constructors. And if you need more, you can always access the layout pointer with `->` operator.

An example with easy-to-read layout hierarchy, coded in a declarative-like style:

```cpp
auto dialog = new QDialog();
auto icon = new QLabel();
auto text = new QLabel("A message ...");
auto ok = new QPushButton("Ok");
auto cancel = new QPushButton("Cancel");

HBox(dialog)
    << (VBox() 
        << icon
        << Stretch())
    << Spacing(16)
    << (VBox() 
        << text 
        << Stretch() 
        << (HBox()
            << Stretch()
            << ok
            << cancel));
```

More examples are in [`layouts.h`](qtutils/layouts.h).

safeConnect
-----------
File: [`safeconnect.h`](qtutils/safeconnect.h)<br>
Dependency: QtCore<br>
License: MIT

SafePointer
-----------
File: [`safepointer.h`](qtutils/safepointer.h)<br>
Dependency: QtCore<br>
License: MIT


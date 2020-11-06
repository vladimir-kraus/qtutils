# QtUtils

A collection of utilities for (not only) Qt development. At this moment it contains only a few header-only classes which can be used anywhere in your code. More stuff will be added later on...

Layouts
-------
File: [`layouts.h`](qtutils/layouts.h)<br>
Dependency: QtWidgets<br>
License: MIT

I hate creating complicated layouts in QtDesigner as much as you do. If you tried it at least once, you know what I am talking about. But I also hate writing and reading complex and verbose procedural code. So I created a few utility classes in [`layouts.h`](qtutils/layouts.h) which will allow writing more concise layout-related code in somewhat declarative style. Currently it only contains helpers for `QVBoxLayout` and `QHBoxLayout`, which however account for more than 90 % of all layouts in my code.

There are two templated wrappers `VBox` and `HBox` around pointers to `QVBoxLayout` and `QHBoxLayout`. These wrappers allow implicit conversions to the undelying layout pointers, they provide `<<` operators with which you can add child widgets or child layouts in a declarative way. You can also add stretches (using `Stretch`) or spacings (using `Spacing`). You can also easily define margins and default spacing for the layout in wrapper constructors. And if you need more, you can always access the layout pointer by `->`.

Example with easy to read layout hierarchy, coded in a declarative-like style:

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

More exmaples are in [`layouts.h`](qtutils/layouts.h).

Singleton
---------
File: [`singleton.h`](qtutils/singleton.h)<br>
Dependency: none<br>
License: MIT

Yes, I know that singletons are evil. Everybody says that. Nevertheless I must confess that I like to use singletons time to time because they are so pragmatic, helpful and can save lots of code. The only thing you need to remember when using singletons is to use them sparsely and only when you have a very good reason. There are many possible implementation of singletons but after trying several of them I created my own which you can find in [`singleton.h`](qtutils/singleton.h). Like many other singleton implementations also this uses [CTRP pattern](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern). Unlike many other implementations, which are implicitly instantiated at first use, my implementation requires explicit instantiation. Which I think is a good thing because I like to have control over the time when things are instantiated and destroyed. Unlike many other implementations in which the instances live until the very end of your program, this implementation allows destroying the singleton instances and then instantiating them again. This singleton implementation is not dependent on Qt so it can be used in any C++ code. But it complements with all Qt classes extremely well.

Here I would like to mention why I think that singletons are so cool (sometimes). The main benefit of singletons is that they are accessible from anywhere, you do not need to pass and keep their references all around your application. Yes, there is certain price to pay. You hide code dependencies (well, this not cool, in general), testing may be a bit more complicated (but is possible), you must not forget to instantiate them and instantiate them only once (if the instatiation is not automatic).

For example I am using singleton for keeping my application's preferences. There are so many small objects throughout my application which need to get some information from preferences (e.g. about customized data formatting) that it is not practical to pass explicitly reference to preferences to each of these little objects individually. My `Preferences` class use multiple inheritance from `QObject` and `Singleton<Preferences>` (CRTP pattern). Inheriting from `QObject` allows defining for example signal `changed()` to inform the rest of the application about the fact that something in the preferences was changed and let all observers connected to this signal to be updated and reflect these changes. Inheriting from `Singleton<>` provides static `instance()` method and asserts that it is instantiated at most once (in debug mode only).

A snippet from `preferences.h` (with implementations from `preferences.cpp`):
```cpp
#include "singleton.h"
// ...

class Preferences : public QObject, public Singleton<Preferences> // note that Singleton is mentioned only here
{
    Q_OBJECT
    
public:
    Preferences()
    {
        load(); // loaded via QSettings
    }
    
    ~Preferences() override
    {
        save(); // saved via QSettings
    }
    
    // ... some public setters and getters, e.g.
    void setDateFormat(const QString &format)
    {
        m_dateFormat = format;
        emit changed();
    }
    
signals:
    void changed();
    
private:
    // ... some private members, e.g.
    QString m_dateFormat;
};
```

A snippet from any place in the code, e.g. `datatable.cpp` which needs to be updated whenever the preferences change:
```cpp
#include "preferences.h"
// ...

DataTable::DataTable(QWidget *parent) : 
    QWidget(parent)
{
    // ... constructor code
    connect(Preferences::instance(), &Preferences::changed, this, &DataTable::update);
}

// ...
```

A snippet from `main.cpp`:
```cpp
// ...

int main(int argc, char *argv[]) 
{
    QApplication application(argc, argv);
    // ... setup application name etc. so that we can use QSettings
    Preferences preferences; // preferences are instantiated and loaded
    MainWindow window;
    window.show();
    return application.exec();
    // preferences are saved and destroyed
}
```

Another usecase for a singleton in my application is a window which may have only one instance `preferenceswindow.h`:
```cpp
#include "singleton.h"
// ...

class PreferencesWindow : public QWidget, public Singleton<PreferencesWindow> // note that Singleton is mentioned only here
{
// ... 
};
```

And this is how the window is displayed from `mainwindow.cpp`:
```cpp
#include "preferenceswindow.h"
// ...

void MainWindow::openPreferencesWindow()
{
    if (PreferencesWindow::instance() == nullptr)
    {
        auto wnd = new PreferencesWindow(); // it is a top level window so it has no parent
        wnd->setAttribute(Qt::WA_DeleteOnClose); // to be deleted when it is closed by user
    }
    else
    {
        PreferencesWindow::instance()->activateWindow(); // it may have been hidden behind the main window
    }
}
// ...
```

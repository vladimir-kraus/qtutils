#pragma once

#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

//
// Simple wrappers around QVBoxLayout, QHBoxLayout and QFormLayout, which allow
// easy to read and easy to maintain "declarative" style of creating layouts in
// your QtWidgets code. It also supports simple setting of margins, spacing,
// stretch and alignment.
//
// The main advantage is that defining complex hierarchical layouts can be done
// in one expression which can be formatted with a clear visual hierarchy.
// Creation of content of a window and dialog therefore can be split to two
// separated steps, making the code more simple to follow:
//
// 1) creation and initialization of all widgets
// 2) organizing the widgets in a layout hierarchy using layout wrappers
//
// Note that layout wrappers are non-owning in relation to child layouts or
// widgets. Therefore if a layout wrapper is instantiated without a parent and
// then is not used in a parent layout or layout wrapper, the caller must take
// care of deleting all dynamically allocated content, or memory leak will be
// caused. This is based on the principle that widgets and layouts are assigned
// their parent after they are added to a parent layout. Copying of layout
// wrappers (e.g. passing it out from functions) is safe as this does not
// allocate any new memory.
//

//
// Here is a very simple example of a vertical layout with a title, text,
// stretched space and a button in a dialog:
//
// auto dialog = new QDialog();
// auto title = new QLabel("Title");
// auto text = new QLabel("Text");
// auto ok = new QPushButton("OK");
//
// VBox(dialog, Margins())
//     << title
//     << text
//     << Stretch()
//     << ok;
//

//
// The main power of layout wrappers is that they can be nested in a hierarchy.
// An example with a "declarative" style layout hierarchy:
//
// auto dialog = new QDialog();
// auto icon = new QLabel();
// auto text = new QLabel("A message ...");
// auto okBtn = new QPushButton("OK");
// auto cancelBtn = new QPushButton("Cancel");
//
// HBox(dialog, Margins())
//     << (VBox()
//         << icon
//         << Stretch())
//     << Spacing(8)
//     << (VBox()
//         << text
//         << Stretch()
//         << (HBox()
//             << Stretch()
//             << okBtn
//             << cancelBtn));
//

//
// Adding nullptr widgets or nullptr child layouts to standard Qt layouts
// causes crash. But sometimes we want to create layouts which, based on some
// condition, may or may not contain certain widget ("Schödinger's widget").
// With layout wrappers, nullptr widgets or child layouts are simply ignored,
// which allows defining a uniform layout structures without unnecessary
// conditions testing children whether they are nullptr or not. Example:
//
// auto buttonA = new QPushButton("A");
// QPushButton *buttonB = nullptr;
// if (useBothButtons()) {
//     // buttonB may be dead or alive based on a condition
//     buttonB = new QPushButton("B");
// }
//
// HBox()
//     << buttonA
//     << buttonB;
//

/**
 * @brief Defines margins for layout wrappers. By default, all layout wrappers
 * have zero-sized margins. Non-zero sized margins are however needed for 
 * top-level layouts in a window or dialog. Use default constructor Margins() 
 * to create default margins, i.e. depending on application's QStyle. Default 
 * margins correspond to value -1.
 */
class Margins
{
public:
    /**
     * @brief Uses the same margin size for all left, top, right and bottom 
     * margins.
     */
    Margins(int value = -1)
        : m_margins(value, value, value, value)
    {}

    Margins(int left, int top, int right, int bottom)
        : m_margins(left, top, right, bottom)
    {}

    const QMargins &margins() const { return m_margins; }

private:
    QMargins m_margins;
};

/**
 * @brief Defines spacing for layout wrappers.
 */
class Spacing
{
public:
    Spacing(int spacing = -1)
        : m_spacing(spacing)
    {}

    int spacing() const { return m_spacing; }

private:
    int m_spacing;
};

/**
 * @brief Allows placing stretched blank space into parent VBox or HBox 
 * layouts.
 */
class Stretch
{
public:
    /**
     * @brief Stretched blank space.
     */
    explicit Stretch(int stretch = 1)
        : m_stretch(stretch)
    {}

    int stretch() const { return m_stretch; }

private:
    int m_stretch;
};

/**
 * @brief Allows placing stretched child widget or layout into parent VBox or 
 * HBox layouts.
 */
class Stretched
{
public:
    /**
     * @brief Wraps a stretched widget.
     */
    explicit Stretched(QWidget *widget, int stretch = 1)
        : m_widget(widget)
        , m_stretch(stretch)
    {}

    /**
     * @brief Wraps a stretched child layout.
     */
    explicit Stretched(QLayout *layout, int stretch = 1)
        : m_layout(layout)
        , m_stretch(stretch)
    {}

    QWidget *widget() const { return m_widget; }

    QLayout *layout() const { return m_layout; }

    int stretch() const { return m_stretch; }

private:
    QWidget *m_widget = nullptr;
    QLayout *m_layout = nullptr;
    int m_stretch;
};

/**
 * @brief Allows placing aligned and (optionally) stretched widget into parent 
 * VBox or HBox layouts.
 */
class Aligned
{
public:
    Aligned(QWidget *widget, Qt::Alignment alignment, int stretch = 0)
        : m_widget(widget)
        , m_alignment(alignment)
        , m_stretch(stretch)
    {}

    QWidget *widget() const { return m_widget; }

    Qt::Alignment alignment() const { return m_alignment; }

    int stretch() const { return m_stretch; }

private:
    QWidget *m_widget = nullptr;
    Qt::Alignment m_alignment;
    int m_stretch;
};

/**
 * @brief Base layout wrapper for layouts inheriting from QLayout. Do not
 * instantiate this class directly, use derived classes.
 */
template<typename Layout_T>
class LayoutWraper
{
public:
    explicit LayoutWraper(Layout_T *layout, const Margins &margins, int spacing)
        : p(layout)
    {
        p->setContentsMargins(margins.margins());
        p->setSpacing(spacing);
    }

    /**
     * @brief Implicit conversion to the wrapped layout class so that 
     * the wrapper can be used anywhere the underlying layout pointer 
     * is expected.
     */
    operator Layout_T *() const { return p; }

    /**
     * @brief Dereference operator can be used to access methods of the wrapped
     * layout class.
     */
    Layout_T *operator->() const { return p; }

protected:
    Layout_T *p = nullptr;
};

/**
 * @brief Base layout wrapper for layouts inheriting from QBoxLayout. Do not
 * instantiate this class directly, use HBox or VBox instead.
 */
class Box : public LayoutWraper<QBoxLayout>
{
public:
    Box(QBoxLayout *layout, const Margins &margins, int spacing)
        : LayoutWraper<QBoxLayout>(layout, margins, spacing)
    {}

    /**
     * @brief Adds a child widget. If the widget is null, it will be ignored.
     */
    Box &operator<<(QWidget *widget)
    {
        if (widget) {
            p->insertWidget(placement(), widget);
        }

        return *this;
    }

    /**
     * @brief Adds child layout. The child layout can also be a result of 
     * implicit conversion from layout wrapper, which allows nesting of layout
     * wrappers. If the layout is null, it will be ignored.
     */
    Box &operator<<(QLayout *layout)
    {
        if (layout) {
            p->insertLayout(placement(), layout);
        }

        return *this;
    }

    /**
     * @brief Adds stretched blank space.
     */
    Box &operator<<(const Stretch &stretch)
    {
        p->insertStretch(placement(), stretch.stretch());
        return *this;
    }

    /**
     * @brief Adds stretched widget or child layout.
     */
    Box &operator<<(const Stretched &stretched)
    {
        if (stretched.widget()) {
            p->insertWidget(placement(), stretched.widget(), stretched.stretch());
        } else if (stretched.layout()) {
            p->insertLayout(placement(), stretched.layout(), stretched.stretch());
        }
        return *this;
    }

    /**
     * @brief Adds aligned and (optionally) stretched widget.
     */
    Box &operator<<(const Aligned &aligned)
    {
        if (aligned.widget()) {
            p->insertWidget(placement(), aligned.widget(), aligned.stretch(), aligned.alignment());
        }
        return *this;
    }

    /**
     * @brief Adds fixed-size spacing.
     */
    Box &operator<<(int spacing)
    {
        if (spacing > 0) {
            p->insertSpacing(placement(), spacing);
        }
        return *this;
    }

    Box &setReversed(bool value)
    {
        m_reversed = value;
        return *this;
    }

    bool isReversed() const { return m_reversed; }

protected:
    int placement() const
    {
        // 0 .. insert at the first position
        // -1 .. insert at the last position
        return m_reversed ? 0 : -1;
    }

    bool m_reversed = false;
};

/**
 * @brief Layout wrapper for QVBoxLayout.
 */
class VBox : public Box
{
public:
    explicit VBox(QWidget *parent, const Margins &margins = 0, int spacing = -1)
        : Box(new QVBoxLayout(parent), margins, spacing)
    {}

    explicit VBox(const Margins &margins = 0, int spacing = -1)
        : VBox(nullptr, margins, spacing)
    {}
};

/**
 * @brief Layout wrapper for QHBoxLayout.
 */
class HBox : public Box
{
public:
    explicit HBox(QWidget *parent, const Margins &margins = 0, int spacing = -1)
        : Box(new QHBoxLayout(parent), margins, spacing)
    {}

    explicit HBox(const Margins &margins = 0, int spacing = -1)
        : HBox(nullptr, margins, spacing)
    {}
};

/**
 * @brief Wrapper of items to be added in a single row to a Form layout wrapper.
 */
class Row
{
public:
    Row(QWidget *label, QWidget *widget)
        : m_label(label)
        , m_widget(widget)
    {}

    Row(const QString &labelText, QWidget *widget)
        : m_labelText(labelText)
        , m_widget(widget)
    {}

    Row(QWidget *label, QLayout *layout)
        : m_label(label)
        , m_layout(layout)
    {}

    Row(const QString &labelText, QLayout *layout)
        : m_labelText(labelText)
        , m_layout(layout)
    {
        // If this constructor was called it means we want
        // to use QFormLayout::addRow(QString, ...) overload.
        // Form's << operator tests isNull() to decide whether
        // to use label text overload for QFormLayout::addRow().
        // Therefore we want to convert null string to empty one.
        if (m_labelText.isNull()) {
            m_labelText = QStringLiteral("");
        }
    }

    Row(QWidget *widget)
        : m_widget(widget)
    {}

    Row(QLayout *layout)
        : m_layout(layout)
    {}

    QWidget *label() const { return m_label; }

    const QString &labelText() const { return m_labelText; }

    QWidget *widget() const { return m_widget; }

    QLayout *layout() const { return m_layout; }

private:
    QWidget *m_label = nullptr;
    QString m_labelText;
    QWidget *m_widget = nullptr;
    QLayout *m_layout = nullptr;
};

/**
 * @brief Layout wrapper for QFormLayout. Rows can be added with Row objects.
 */
class Form : public LayoutWraper<QFormLayout>
{
public:
    explicit Form(QWidget *parent, const Margins &margins = 0, int spacing = -1)
        : LayoutWraper<QFormLayout>(new QFormLayout(parent), margins, spacing)
    {}

    explicit Form(const Margins &margins = 0, int spacing = -1)
        : Form(nullptr, margins, spacing)
    {}

    Form &operator<<(const Row &row)
    {
        if (row.widget()) {
            if (row.label()) {
                p->addRow(row.label(), row.widget());
            } else if (!row.labelText().isNull()) {
                p->addRow(row.labelText(), row.widget());
            } else {
                p->addRow(row.widget());
            }
        } else if (row.layout()) {
            if (row.label()) {
                p->addRow(row.label(), row.layout());
            } else if (!row.labelText().isNull()) {
                p->addRow(row.labelText(), row.layout());
            } else {
                p->addRow(row.layout());
            }
        }
        return *this;
    }
};

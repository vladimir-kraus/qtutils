//
// Copyright (c) Vladimir Kraus. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
// https://github.com/vladimir-kraus/qtutils
//

// Simple wrappers around QVBoxLayout and QHBoxLayout which allow concise
// declarative style of creating layouts in your code. It also supports
// simple setting of margins, spacing and stretch.

//
// An example of a layout with a title, text, stretched space and a button:
//
// auto title = new QLabel("Title");
// auto text = new QLabel("Text");
// auto ok = new QPushButton("Ok");
//
// dialog->setLayout(VBox() << title << text << Stretch() << ok);
//

//
// An example with declarative-style layout hierarchy:
//
// auto dialog = new QDialog();
// auto icon = new QLabel();
// auto text = new QLabel("A message ...");
// auto ok = new QPushButton("Ok");
// auto cancel = new QPushButton("Cancel");
//
// HBox(dialog)
//     << (VBox() 
//         << icon
//         << Stretch())
//     << Spacing(16)
//     << (VBox() 
//         << text 
//         << Stretch() 
//         << (HBox()
//             << Stretch()
//             << ok
//             << cancel));
//

//
// An example with Schödinger's widget:
// 
// auto buttonA = new QPushButton("A");
// QPushButton *buttonB = nullptr;
// if (useB) // buttonB may be dead or alive
// { 
//     buttonB = new QPushButton("B");
// }
// 
// // regardless of (non)existence of buttonB we can safely 
// // use one layout because possible nullptr will be ignored
// widget->setLayout(HBox() << buttonA << buttonB);
//

//
// An example with setting zero margins and reusing of explicit spacing:
//
// auto spacing = Spacing(10); // 10 pixels between button groups
// widget->setlayout(HBox(0, 0) // no margins around layout and no spacing
//     << button1 << button2
//     << spacing // space between button groups
//     << button3 << button4
//     << spacing // space between button groups
//     << button5 << button6);
// 

#pragma once

#include <QHBoxLayout>
#include <QVBoxLayout>

/*
 * A layout placeholder for a stretched space or a stretched widget.
 *
 * When used with a widget, copy of the instance of this class makes
 * only a shallow copy, i.e. it still points to the same widget.
 * In that case, you can insert it into a layout only once.
 *
 * When used without a widget, it can be copied or inserted
 * into a layout or layouts multiple times.
 */
struct Stretch final
{
    /*
     * Stretching a widget. If widget is nullptr, then stretch
     * value is ignored and stretch will be zero-sized.
     */
    explicit Stretch(QWidget *widget, int value = 1) :
        m_widget(widget),
        m_value(widget ? value : -1)
    {
    }

    /*
     * Stretching a layout. If layout is nullptr, then stretch
     * value is ignored and stretch will be zero-sized.
     */
    explicit Stretch(QLayout *layout, int value = 1) :
        m_layout(layout),
        m_value(layout ? value : -1)
    {
    }

    /*
     * Stretching empty space.
     */
    explicit Stretch(int value = 1) :
        m_value(value)
    { }

    QWidget *widget() const
    {
        return m_widget;
    }

    QLayout *layout() const
    {
        return m_layout;
    }

    int value() const
    {
        return m_value;
    }

private:
    QWidget *m_widget = nullptr;
    QLayout *m_layout = nullptr;
    int m_value = -1;
};

/*
 * A layout placeholder for a space of given size (in pixels).
 * It can be copied or inserted into a layout or layouts multiple times.
 */
struct Spacing final
{
    explicit Spacing(int value) :
        m_value(value)
    { }

    int value() const
    {
        return m_value;
    }

private:
    int m_value = 0;
};

/*
 * A templated layout wrapper. Use classes VBox or HBox based on this template.
 * 
 * For keeping default margins and spacing use value -1.
 * Copy of the instance of this class makes only
 * a shallow copy, i.e. it still points to the same layout.
 *
 * The wrapper creates the underlying layout in the constructor but does not own the layout.
 * You must insert it into another layout or set it as a layout to a widget. 
 * Then the lifetime of the layout will be taken over by the widget.
 */
template<typename LayoutType, int defaultMargins = -1, int defaultSpacing = -1>
class Box
{
public:
    /*
     * Creates a layout wrapper with default margins and spacing.
     * The parent widget can be left empty but that case
     * it is needed to call parent->setLayout(box) later.
     */
    explicit Box(QWidget *parent = nullptr) :
        Box(defaultMargins, defaultSpacing, parent)
    { }

    /*
     * Creates a layout wrapper with given margins and optional spacing.
     * The parent widget can be left empty but that case
     * it is needed to call parent->setLayout(box) later.
     */
    explicit Box(int margins, int spacing = defaultSpacing, QWidget *parent = nullptr) :
        p(new LayoutType(parent))
    {
        if (margins >= 0)
        {
            p->setContentsMargins(margins, margins, margins, margins);
        }

        if (spacing >= 0)
        {
            p->setSpacing(spacing);
        }
    }

    /*
     * Implicit conversion to the underlying layout type so that the wrapper
     * can be used anywhere the underlying layout pointer is expected.
     */
    operator LayoutType*() const
    {
        return p;
    }

    /*
     * Dereference operator, which can be used to access methods of the underlying layout.
     */
    LayoutType* operator-> () const
    {
        return p;
    }

    /*
     * Adds child widget.
     */
    Box& operator <<(QWidget *widget)
    {
        if (widget != nullptr)
        {
            p->addWidget(widget);
        }

        return *this;
    }

    /*
     * Adds child layout. The child layout can also be
     * a result of implicit conversion from layout wrapper.
     */
    Box& operator <<(QLayout *layout)
    {
        if (layout != nullptr)
        {
            p->addLayout(layout);
        }

        return *this;
    }

    /*
     * Adds a stretched space or widget.
     */
    Box& operator <<(const Stretch &stretch)
    {
        int s = stretch.value();
        if (s >= 0)
        {
            if (stretch.widget() != nullptr)
            {
                p->addWidget(stretch.widget(), s);
            }
            else if (stretch.layout() != nullptr)
            {
                p->addLayout(stretch.layout(), s);
            }
            else
            {
                p->addStretch(s);
            }
        }

        return *this;
    }

    /*
     * Adds empty space of given size.
     */
    Box& operator <<(const Spacing &spacing)
    {
        int s = spacing.value();
        if (s >= 0)
        {
            p->addSpacing(s);
        }

        return *this;
    }

private:
    LayoutType *p;
};

/*
 * Wrapper for QVBoxLayout with default margins and spacing.
 */
using VBox = Box<QVBoxLayout, -1, -1>;

/*
 * Wrapper for QHBoxLayout with default margins and spacing.
 */
using HBox = Box<QHBoxLayout, -1, -1>;

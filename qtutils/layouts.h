//
// Copyright (c) Vladimir Kraus. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

//
// Example of setting a child layout with a title and text labels to a parent widget:
//
// #include "layouts.h"
// ...
// auto title = new QLabel("Title");
// auto text = new QLabel("Text");
// auto button = new QPushButton("Ok");
// dialog->setLayout(VBox() << title << textBox << Stretch() << button);
// ...
//

#pragma once

#include <QHBoxLayout>
#include <QVBoxLayout>

/*
 * A layout placeholder for a stretched space or widget.
 * When used with a widget, copy of the instance of this class makes
 * only a shallow copy, i.e. it still points to the same widget.
 */
struct Stretch final
{
    explicit Stretch(QWidget *widget, int value = 1) :
        m_widget(widget),
        m_value(value)
    { }

    explicit Stretch(int value = 1) :
        m_value(value)
    { }

    QWidget *widget() const
    {
        return m_widget;
    }

    int value() const
    {
        return m_value;
    }

private:
    QWidget *m_widget = nullptr;

    int m_value = 0;
};

/*
 * A layout placeholder for a space of given size.
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
 * A templated layout wrapper.
 * For keeping default margins and spacing use value -1.
 * Copy of the instance of this class makes only
 * a shallow copy, i.e. it still points to the same layout.
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
        Box(defaultMargins, defaultMargins, parent)
    { }

    /*
     * Creates a layout wrapper with given margins and optional spacing.
     * The parent widget can be left empty but that case
     * it is needed to call parent->setLayout(box) later.
     */
    explicit Box(int margins, int spacing = defaultSpacing, QWidget *parent = nullptr) :
        p(new LayoutType(parent))
    {
        if (margins != -1)
        {
            p->setContentsMargins(margins, margins, margins, margins);
        }

        if (spacing != -1)
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
        if (stretch.widget() == nullptr)
        {
            p->addStretch(stretch.value());
        }
        else
        {
            p->addWidget(stretch.widget(), stretch.value());
        }

        return *this;
    }

    /*
     * Adds empty space of given size.
     */
    Box& operator <<(const Spacing &spacing)
    {
        p->addSpacing(spacing.value());
        return *this;
    }

private:
    LayoutType *p;
};

/*
 * Wrapper for QVBoxLayout.
 */
using VBox = Box<QVBoxLayout, -1, -1>;

/*
 * Wrapper for QHBoxLayout.
 */
using HBox = Box<QHBoxLayout, -1, -1>;

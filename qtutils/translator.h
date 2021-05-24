#pragma once

#include <QApplication>
#include <QObject>

/**
 * Provides simple dynamic translations for widgets.
 * Translator object must be instantiated after QApplication is instantiated.
 * Only one instance of this class can be created.
 */
class Translator : public QObject
{
    Q_OBJECT

public:
    Translator()
    {
        Q_ASSERT(s_instance == nullptr);
        s_instance = this;

        Q_ASSERT(qApp != nullptr);
        qApp->installEventFilter(this);
    }

    ~Translator() override
    {
        s_instance = nullptr;
    }

    static Translator *instance()
    {
        return s_instance;
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == qApp && event->type() == QEvent::LanguageChange)
        {
            emit languageChanged();
        }

        return false;
    }

signals:
    void languageChanged();

private:
    inline static Translator *s_instance = nullptr;
};

/**
 * Binds a dynamic translation to a widget.
 * Usage:
 * auto label = new QLabel();
 * TR_TEXT(label, tr("this is a text"));
 * TR_TOOLTIP(label, tr("this is a tooltip"));
 */
#define TR(widget, function, text) { auto p = widget; safeConnect(Translator::instance(), &Translator::languageChanged, p, [p]{ p->function(text); }); } widget->function(text)
#define TR_TEXT(widget, text) TR(widget, setText, text)
#define TR_TOOLTIP(widget, text) TR(widget, setToolTip, text)

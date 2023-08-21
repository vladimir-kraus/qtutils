#include <QApplication>
#include <QFrame>

#include "qtutils/layouts.h"

class Frame : public QFrame
{
public:
    explicit Frame(QWidget *parent = nullptr) : QFrame(parent)
    {
        setStyleSheet("background: gray;");
    }

    QSize minimumSizeHint() const
    {
        return { 100, 100 };
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget w;

    Frame *nullWidget =  nullptr;

    w.setLayout(VBox()
                    << nullWidget
                    << Spacing(5)
                    << (new Frame())
                    << Stretch(new Frame(), 1)
                    << Stretch(HBox()
                        << Stretch(new Frame(), 1)
                        << (new Frame())));

    w.show();
    return a.exec();
}

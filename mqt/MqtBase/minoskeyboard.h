#ifndef MINOSKEYBOARD_H
#define MINOSKEYBOARD_H

#include <QFrame>
#include <QToolButton>
#include <QVector>

namespace Ui {
class MinosKeyboard;
}

class MinosKeyboard : public QFrame
{
    Q_OBJECT

public:
    explicit MinosKeyboard(QWidget *parent = 0);
    ~MinosKeyboard();

private slots:

    void onKeyButton();
    void onEscButton();
    void onTabButton();
    void onReturnButton();

private:
    Ui::MinosKeyboard *ui;

    void sendChar(QChar charToSend);
};

#endif // MINOSKEYBOARD_H

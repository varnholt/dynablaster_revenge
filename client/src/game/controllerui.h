#ifndef CONTROLLERUI_H
#define CONTROLLERUI_H

// Qt
#include <QFrame>

// shared
#include "gameinformation.h"


// forward declarations
class QTimer;

namespace Ui {
    class controllerui;
}

class ControllerUi : public QFrame
{
    Q_OBJECT

public:
    explicit ControllerUi(QWidget *parent = 0);
    ~ControllerUi();

 public slots:

    void updateTable(const QList<GameInformation>&);


signals:

    void login(const QString& name, const QColor& color);
    void joinGame(int);
    void leaveGame(int);
    void createGame(const QString& name);
    void startGame(int);
    void stopGame(int);
    void updateTable();


private slots:

    void login();
    void joinGame();
    void leaveGame();
    void createGame();
    void startGame();
    void stopGame();

private:

    Ui::controllerui *mUi;

    QTimer* mUpdateTableTimer;
};

#endif // CONTROLLERUI_H

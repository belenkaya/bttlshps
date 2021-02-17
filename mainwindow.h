#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <qvector.h>
#include <QMainWindow>
#include "new_game.h"
#include <vector>
#include <future>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
enum class GameState {
    BEFORE_GAME = 0,
    GAME_VS_COMPUTER = 1,
    GAME_VS_PERSON = 2,

};
enum class Move {
    PLAYER1 = 1,
    PLAYER2
};

using Battleships = QVector<QVector<bool>>;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
public slots:
    void OpenNewGame();
    void GetState(int);
    void GetShip(Battleships&, int);
    void GetName(QString, int);
    Point Peek();
    Point MakeMove();
    void Congaratulate(int);
private slots:
    void StartGameComputer();
    void StartGamePerson();
    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_tableWidget_2_cellDoubleClicked(int row, int column);

private:
    bool HasAliveShip(const Battleships&);
    GameState current_state_ = GameState::BEFORE_GAME;
    Ui::MainWindow *ui;
    NewGame* new_game_;
    QString player1_name_;
    QString player2_name_;
    Battleships player_1_;
    Battleships mov1_;
    Battleships mov2_;
    Battleships player_2_;
    int num_players_;
    int move_count_ = 0;
    Move current_move_ = Move::PLAYER1;
    int alive1_ = 20;
    int alive2_ = 20;
    int cheater_ = 0;
};
#endif // MAINWINDOW_H

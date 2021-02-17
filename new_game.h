#ifndef NEW_GAME_H
#define NEW_GAME_H

#include <QDialog>
#include <vector>
#include <random>
using Battleships = QVector<QVector<bool>>;
namespace Ui {
class NewGame;
}
enum class NewGameState {
    ONE_PLAYER = 1,
    TWO_PLAYERS,
    PLAYER1_READY,
};
enum class RuleSet {
    EASY = 0,
    CLASSICAL
};
struct Point {
    int x;
    int y;
    bool operator<(const Point& rhs){
        return (x < rhs.x) || (y < rhs.y);
    }
};
bool cmp(const Point& a, const Point& b);
bool operator<(const Point& lhs, const Point& rhs);
Battleships GenVec();
class NewGame : public QDialog
{
    Q_OBJECT

public:
    explicit NewGame(QWidget *parent = nullptr);
    ~NewGame();
    QString GetPlayer1Name() const;
    QString GetPlayer2Name() const;
    Battleships GetShips(int) const;
    bool CheckShip(const Battleships&);
signals:
    void SendGameState(int);
    void SendShip(Battleships&, int);
    void SendName(QString, int);
private slots:

    void on_radioButton_2_toggled(bool checked);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_radioButton_4_toggled(bool checked);

    void on_tableWidget_cellDoubleClicked(int row, int column);

private:
    Ui::NewGame *ui;
    NewGameState state_ = NewGameState::ONE_PLAYER;
    RuleSet rules_ = RuleSet::EASY;
    QString player_1_name_ = "";
    QString player_2_name_ = "";
    Battleships player_1_;
    Battleships player_2_;
};

#endif // NEW_GAME_H

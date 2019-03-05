#include <iostream>
#include <conio.h>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

#define CURSOR '+'
#define FLAG 'P'
#define SPACE ' '
#define HIDDEN 'O'
#define NOT_SURE '?'

/*
map[i][j].second:
0 : space
1~8 : mine_info
9 : mine
*/
struct Difficulty {
    int height;
    int width;
    int mines;
    Difficulty(int h, int w, int m) : height(h), width(w), mines(m) {};
};

const Difficulty Small{13, 13, 20};
const Difficulty Middle{25, 25, 70};
const Difficulty Large{33, 33, 130};

typedef enum { Flag = 'k', Dig = 'j', Up = 'w', Left = 'a', Right = 'd', Down = 's', } Command;

struct Pos {
    int h;
    int w;
    inline Pos around(int h_change, int w_change) { return {h + h_change, w + w_change};}
};

struct Game {
    vector<vector<pair<char, int>>> map;

    int height;
    int width;

    int mines;
    bool isFail;
    bool isVictory;
    Pos here;

    Game(Difficulty diff, vector<pair<char, int>> temp1);

    inline pair<char, int> & get(Pos pos) { return map[pos.h][pos.w];}
    void set_mines(void);
    void move(Command command);
    void refresh(void);
    void show(Pos pos);
    void set_info(void);
    void is_victory(void);
};

Game::Game(Difficulty diff, vector<pair<char, int>> temp1) : map(diff.height+2, temp1) {
    mines = diff.mines;
    isFail = false;
    isVictory = false;
    height = diff.height;
    width = diff.width;
    here = {1,1};
    for (int i=0; i < height+2; i++) {
        for (int j=0; j < width+2; j++) {
            if (i == 0 || i == height+1) {
                map[i][j].second = 0;
                map[i][j].first = '-';
            }
            else if (j == 0 || j == width+1) {
                map[i][j].second = 0;
                map[i][j].first = '|';
            }
            else {
                map[i][j].second = 0;
                map[i][j].first = HIDDEN;
            }
        }
    }
    set_mines();
    set_info();
}

void Game::set_mines(void) {
    srand((unsigned)time(NULL));
    int s = width*height;
    bool temp[s];
    for (int i=0; i<s; i++) {
        if (i < mines) temp[i] = true;
        else temp[i] = false;
    }
    for (int i=1; i<s; i++) swap(temp[i], temp[rand()%(i+1)]);
    int ind = 0;
    for (int i=1; i<height+1; i++) 
        for (int j=1; j<width+1; j++)
            if (temp[ind++]) map[i][j].second = 9;
}

void Game::set_info(void) {
    int info;
    for (int i=1; i<height+1; i++) {
        for (int j=1; j<width+1; j++) {
            if (map[i][j].second == 0) {
                info = 0;
                if (map[i+1][j+1].second == 9) ++info;
                if (map[i+1][j].second == 9) ++info;
                if (map[i+1][j-1].second == 9) ++info;
                if (map[i][j+1].second == 9) ++info;
                if (map[i][j-1].second == 9) ++info;
                if (map[i-1][j+1].second == 9) ++info;
                if (map[i-1][j].second == 9) ++info;
                if (map[i-1][j-1].second == 9) ++info;
                map[i][j].second = info;
            }

        }
    }
}

void Game::refresh(void) {
    system("cls");
    if (!isFail){
        for (int i=0; i<height+2; i++) {
            for (int j=0; j<width+2; j++) {
                if (here.h == i && here.w == j) {
                    cout<<CURSOR;
                }
                else cout<<map[i][j].first;
                cout<<' ';
            }
            cout<<endl;
        }
        cout<<"Mines: "<< mines <<endl;
        if (isVictory) cout<<"You Won The Game"<<endl;
    }
    else {
        for (int i=0; i<height+2; i++) {
            for (int j=0; j<width+2; j++) {
                if (here.h == i && here.w == j) {
                    cout<<'*';
                }
                else cout<<map[i][j].second;
                cout<<' ';
            }
            cout<<endl;
        }
        cout<<"Mines: "<< mines <<endl;
        cout<<"You Lose The Game"<<endl;
    }

}

void Game::is_victory(void) {
    for (int i=1; i<height+1; i++) {
        for (int j=1; j<width+1; j++) {
            if (map[i][j].first == HIDDEN && map[i][j].second != 9 ||
                map[i][j].first == FLAG && map[i][j].second != 9) {
                isVictory = false;
                return ;
            }
        }
    }
    isVictory = true;
}

void Game::show(Pos pos) {
    if (get(pos).first == SPACE || pos.h < 1 || pos.h > height ||
        pos.w < 1 || pos.w > height)    return ;
    if (get(pos).second == 0) {
        get(pos).first = SPACE;
        show(pos.around(1,1));
        show(pos.around(1,0));
        show(pos.around(1,-1));
        show(pos.around(0,1));
        show(pos.around(0,-1));
        show(pos.around(-1,1));
        show(pos.around(-1,0));
        show(pos.around(-1,-1));
    }
    else {
        get(pos).first = get(pos).second + '0';
    }
}

void Game::move(Command command) {
    pair<char, int> &p = get(here);
    switch(command) {
        case Flag : 
            if (p.first == SPACE) break;
            if (p.first == FLAG) {
                p.first = NOT_SURE;
                ++mines;
            }
            else if (p.first == NOT_SURE) p.first = HIDDEN;
            else if (p.first == HIDDEN) {
                p.first = FLAG;
                --mines;
            }
            break;

        case Dig :
            if (p.first == SPACE) break; 
            if (p.second == 9) {
                isFail = true;
            } 
            else {
                show(here);
            }
            break;
        case Up : if (here.h == 1) break; --here.h; break;
        case Left : if (here.w == 1) break;--here.w; break;
        case Right : if (here.w == width) break; ++here.w; break;
        case Down : if (here.h == height) break; ++here.h; break;
        default : break;
    }
}

Difficulty change_difficulty(void) {
    cout<<"Please chioce a level: \n 1.small\n 2.middle\n 3.large\n 4.custom\n"<<endl;
    int n;
    cin >> n;
    if (n == 1) return Small;
    else if (n == 2 ) return Middle;
    else if (n == 3 ) return Large;
    else if (n == 4 ) {
        int w, h, m;
        cout<<"height: "; cin>>h;
        cout<<"\nwidth: "; cin>>w;
        cout<<"\nmines: "; cin>>m;
        return {h, w, m};
    }
}

void manual(void) {
    cout<<"ASDW : move\nJ : dig\nK : flag"<<endl;
}

void loop(void) {
    manual();
    Difficulty diff = change_difficulty();
            pair<char, int> temp{'a',1};
            vector<pair<char, int>> temp1(diff.width+2, temp);
    Game game(diff, temp1);
    Command command;
    do {
        game.refresh();
        if ( command = (Command)getch()) {
            if (game.isFail) continue;
            if (game.isVictory) continue;
            game.is_victory();
            game.move(command);
        }
    } while(true);
}

int main()
{
    loop();
}
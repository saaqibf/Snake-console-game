#include <iostream>
#include <windows.h>
#include <cstdlib> // For rand()
#include <vector>

using namespace std;

struct SNAKE_POSITION {
    int X, Y;
};

class family {
    static const int H;
    static const int W;
    char **F;

    // Prevent copying
    family(const family &);
    family operator=(const family &);

public:
    family() {
        F = new char*[H];
        for (int c = 0; c < H; ++c) {
            F[c] = new char[W];
        }
        clear();
    }

    ~family() {
        if (F) {
            for (int c = 0; c < H; ++c) {
                delete[] F[c];
            }
            delete[] F;
        }
    }

    void print() {
        for (int c = 0; c < H; ++c) {
            for (int r = 0; r < W; ++r) {
                cout << F[c][r];
            }
            cout << endl;
        }
    }

    void clear() {
        for (int c = 0; c < H; ++c) {
            for (int r = 0; r < W; ++r) {
                F[c][r] = ' ';
            }
        }
    }

    int get_W() const { return W; }
    int get_H() const { return H; }

    void draw(int y, int x, char what) {
        if (y >= 0 && y < H && x >= 0 && x < W) {
            F[y][x] = what;
        }
    }
};

const int family::H = 24;
const int family::W = 79;

class food_cls {
    SNAKE_POSITION pos;
    char symbol;

public:
    food_cls() : symbol('X'), pos() {
        pos.X = pos.Y = -1;
    }

    void reposition(const family &F) {
        pos.X = rand() % F.get_W();
        pos.Y = rand() % F.get_H();
    }

    int get_X() const { return pos.X; }
    int get_Y() const { return pos.Y; }
    char get_symbol() const { return symbol; }
};

class snake_cls {
    enum Direction { UP, DOWN, LEFT, RIGHT } dir;
    char symbol, head_symbol;
    vector<SNAKE_POSITION> pos;
    int speed;

public:
    snake_cls(int X, int Y)
        : dir(RIGHT), symbol('#'), head_symbol('@'), speed(1) {
        pos.push_back({X, Y});
    }

    bool check_food(const food_cls &food) {
        if (food.get_X() == pos[0].X && food.get_Y() == pos[0].Y) {
            pos.push_back({-1, -1}); // Add a new segment
            return true;
        }
        return false;
    }

    void get_input() {
        if ((GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) && dir != DOWN) {
            dir = UP;
        }
        if ((GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) && dir != UP) {
            dir = DOWN;
        }
        if ((GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) && dir != RIGHT) {
            dir = LEFT;
        }
        if ((GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) && dir != LEFT) {
            dir = RIGHT;
        }
    }

    void move(const family &F) {
        SNAKE_POSITION next = pos[0]; // Start with the head position
        switch (dir) {
        case UP:
            next.Y -= speed;
            break;
        case DOWN:
            next.Y += speed;
            break;
        case LEFT:
            next.X -= speed;
            break;
        case RIGHT:
            next.X += speed;
            break;
        }

        // Check for collisions with walls
        if (next.X < 0 || next.Y < 0 || next.X >= F.get_W() || next.Y >= F.get_H()) {
            throw "Game Over: Snake hit the wall!";
        }

        // Check for collisions with itself
        for (size_t i = 1; i < pos.size(); ++i) {
            if (pos[i].X == next.X && pos[i].Y == next.Y) {
                throw "Game Over: Snake hit itself!";
            }
        }

        // Move the snake
        for (size_t i = pos.size() - 1; i > 0; --i) {
            pos[i] = pos[i - 1];
        }
        pos[0] = next; // Update the head position
    }

    void draw(family &F) {
        for (size_t i = 0; i < pos.size(); ++i) {
            if (i == 0) {
                F.draw(pos[i].Y, pos[i].X, head_symbol);
            } else {
                F.draw(pos[i].Y, pos[i].X, symbol);
            }
        }
    }
};

int main() {
    family F;
    food_cls food;
    snake_cls snake(10, 10);

    cout << "Welcome to the Snake Game!" << endl;
    cout << "Use the arrow keys or WASD to control the snake." << endl;
    cout << "Eat the food (X) to grow your snake." << endl;
    cout << "Avoid hitting the walls or yourself!" << endl;
    cout << "Press any key to start..." << endl;
    system("pause");

    int score = 0;
    food.reposition(F);

    while (true) {
        F.clear();

        snake.get_input();
        try {
            snake.move(F);
        } catch (const char *er) {
            F.clear();
            cerr << er << endl;
            cout << "Your final score: " << score << endl;
            cout << "Press any key to restart or close the window to exit." << endl;
            system("pause");
            return 0;
        }

        if (snake.check_food(food)) {
            food.reposition(F);
            score += 10;
        }

        snake.draw(F);
        F.draw(food.get_Y(), food.get_X(), food.get_symbol());

        F.print();
        cout << "Score: " << score << endl;

        Sleep(100); // Control game speed
        system("cls");
    }

    return 0;
}
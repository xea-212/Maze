#include "DxLib.h"

#include <vector>

#include <queue>

#include <stack>

#include <algorithm>

#include <ctime>

#include <cstdlib>

#include <cmath>

#include <tchar.h>

using namespace std;

const int MAP_W = 51;

const int MAP_H = 51;

const int TILE = 16;

int maze[MAP_H][MAP_W];

int playerX = 1;

int playerY = 1;

int goalX = MAP_W - 2;

int goalY = MAP_H - 2;

bool visited[MAP_H][MAP_W];

bool path[MAP_H][MAP_W];

int parentX[MAP_H][MAP_W];

int parentY[MAP_H][MAP_W];

int dx[4] = { 1,-1,0,0 };

int dy[4] = { 0,0,1,-1 };

bool searching = false;

enum SearchType

{

    NONE,

    BFS,

    DIJKSTRA,

    ASTAR

};

SearchType searchType = NONE;

struct Node

{

    int x;

    int y;

    int cost;

};

queue<Node> bfsQueue;

vector<Node> openList;

int heuristic(int x, int y)

{

    return abs(goalX - x) + abs(goalY - y);

}

void generateMaze()

{

    for (int y = 0; y < MAP_H; y++)

        for (int x = 0; x < MAP_W; x++)

            maze[y][x] = 1;

    stack<pair<int, int>> st;

    st.push({ 1,1 });

    maze[1][1] = 0;

    while (!st.empty())

    {

        int x = st.top().first;

        int y = st.top().second;

        vector<int> dir = { 0,1,2,3 };

        random_shuffle(dir.begin(), dir.end());

        bool moved = false;

        for (int i : dir)

        {

            int nx = x + dx[i] * 2;

            int ny = y + dy[i] * 2;

            if (nx > 0 && ny > 0 && nx < MAP_W - 1 && ny < MAP_H - 1)

            {

                if (maze[ny][nx] == 1)

                {

                    maze[y + dy[i]][x + dx[i]] = 0;

                    maze[ny][nx] = 0;

                    st.push({ nx,ny });

                    moved = true;

                    break;

                }

            }

        }

        if (!moved)

            st.pop();

    }

}

void buildPath()

{

    int x = goalX;

    int y = goalY;

    while (!(x == playerX && y == playerY))

    {

        path[y][x] = true;

        int px = parentX[y][x];

        int py = parentY[y][x];

        x = px;

        y = py;

    }

}

void startSearch(SearchType type)

{

    memset(visited, false, sizeof(visited));

    memset(path, false, sizeof(path));

    while (!bfsQueue.empty()) bfsQueue.pop();

    openList.clear();

    searchType = type;

    visited[playerY][playerX] = true;

    if (type == BFS)

        bfsQueue.push({ playerX,playerY,0 });

    else

        openList.push_back({ playerX,playerY,0 });

    searching = true;

}

void updateBFS()

{

    if (bfsQueue.empty()) { searching = false; return; }

    Node n = bfsQueue.front();

    bfsQueue.pop();

    if (n.x == goalX && n.y == goalY)

    {

        searching = false;

        buildPath();

        return;

    }

    for (int i = 0; i < 4; i++)

    {

        int nx = n.x + dx[i];

        int ny = n.y + dy[i];

        if (nx >= 0 && ny >= 0 && nx < MAP_W && ny < MAP_H)

            if (maze[ny][nx] == 0 && !visited[ny][nx])

            {

                visited[ny][nx] = true;

                parentX[ny][nx] = n.x;

                parentY[ny][nx] = n.y;

                bfsQueue.push({ nx,ny,n.cost + 1 });

            }

    }

}

void updateDijkstra()

{
    if (openList.empty()) { searching = false; return; }
    int best = 0;
    for (int i = 1; i < openList.size(); i++)
        if (openList[i].cost < openList[best].cost)
            best = i;
    Node n = openList[best];
    openList.erase(openList.begin() + best);

    if (n.x == goalX && n.y == goalY)
    {
        searching = false;
        buildPath();
        return;
    }

    for (int i = 0; i < 4; i++)
    {
        int nx = n.x + dx[i];
        int ny = n.y + dy[i];
        if (nx >= 0 && ny >= 0 && nx < MAP_W && ny < MAP_H)
            if (maze[ny][nx] == 0 && !visited[ny][nx])
            {
                visited[ny][nx] = true;
                parentX[ny][nx] = n.x;
                parentY[ny][nx] = n.y;
                openList.push_back({ nx,ny,n.cost + 1 });
            }
    }
}

void updateAstar()
{
    if (openList.empty()) { searching = false; return; }
    int best = 0;
    for (int i = 1; i < openList.size(); i++)
    {
        int f1 = openList[i].cost + heuristic(openList[i].x, openList[i].y);
        int f2 = openList[best].cost + heuristic(openList[best].x, openList[best].y);
        if (f1 < f2) best = i;
    }

    Node n = openList[best];
    openList.erase(openList.begin() + best);

    if (n.x == goalX && n.y == goalY)
    {
        searching = false;
        buildPath();
        return;
    }

    for (int i = 0; i < 4; i++)
    {
        int nx = n.x + dx[i];
        int ny = n.y + dy[i];

        if (nx >= 0 && ny >= 0 && nx < MAP_W && ny < MAP_H)
            if (maze[ny][nx] == 0 && !visited[ny][nx])
            {
                visited[ny][nx] = true;
                parentX[ny][nx] = n.x;
                parentY[ny][nx] = n.y;

                openList.push_back({ nx,ny,n.cost + 1 });
            }
    }
}

void updateSearch()

{

    if (!searching) return;

    if (searchType == BFS) updateBFS();

    if (searchType == DIJKSTRA) updateDijkstra();

    if (searchType == ASTAR) updateAstar();

}

void drawMaze()

{

    for (int y = 0; y < MAP_H; y++)

        for (int x = 0; x < MAP_W; x++)

        {

            int px = x * TILE;

            int py = y * TILE;

            if (maze[y][x] == 1)

                DrawBox(px, py, px + TILE, py + TILE, GetColor(50, 50, 50), TRUE);

            else

                DrawBox(px, py, px + TILE, py + TILE, GetColor(220, 220, 220), TRUE);

            if (visited[y][x])

                DrawBox(px, py, px + TILE, py + TILE, GetColor(255, 0, 0), TRUE);

            if (path[y][x])

                DrawBox(px, py, px + TILE, py + TILE, GetColor(0, 0, 255), TRUE);

        }

    DrawBox(goalX * TILE, goalY * TILE,

        goalX * TILE + TILE,

        goalY * TILE + TILE,

        GetColor(0, 255, 0), TRUE);

    DrawBox(playerX * TILE, playerY * TILE,

        playerX * TILE + TILE,

        playerY * TILE + TILE,

        GetColor(255, 255, 0), TRUE);

}

int main()

{

    ChangeWindowMode(TRUE);

    SetGraphMode(MAP_W * TILE, MAP_H * TILE, 32);

    if (DxLib_Init() == -1) return -1;

    srand((unsigned)time(NULL));

    generateMaze();

    while (ProcessMessage() == 0)

    {

        ClearDrawScreen();

        if (CheckHitKey(KEY_INPUT_1)) startSearch(BFS);

        if (CheckHitKey(KEY_INPUT_2)) startSearch(DIJKSTRA);

        if (CheckHitKey(KEY_INPUT_3)) startSearch(ASTAR);

        updateSearch();

        drawMaze();

        DrawString(10, 10, _T("1:BFS  2:Dijkstra  3:A*"), GetColor(0, 255, 0));

        ScreenFlip();

    }

    DxLib_End();

    return 0;

}

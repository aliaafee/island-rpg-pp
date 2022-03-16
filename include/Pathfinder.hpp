#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <deque>
#include <cstring>

#include "Vector.hpp"
#include "Entity.hpp"

typedef std::pair<int, int> GridCell;

class Node
{
public:
    Node() : parent(nullptr){};
    Node(const int &I, const int &J,
         const float &G, const float &H, const float &F,
         Node *Parent) : i(I), j(J), g(G), h(H), f(F), parent(Parent){};
    int i;
    int j;
    float g;
    float h;
    float f;
    Node *parent;
};

struct MoreThanByF
{
    bool operator()(const Node *lhs, const Node *rhs) const
    {
        return lhs->f > rhs->f;
    }
};

class SpiralOut
{
public:
    SpiralOut(int start_i,
              int start_j,
              int max) : i_(start_i),
                         j_(start_j),
                         max_(max),
                         steps_(1),
                         t_(0), s_(0), count_(0),
                         d_i(1), d_j(0) {}

    bool next(int &out_i, int &out_j)
    {
        if (count_ > max_)
        {
            return false;
        }

        i_ += d_i;
        j_ += d_j;

        out_i = i_;
        out_j = j_;

        count_ += 1;

        s_++;
        if (s_ > steps_ - 1)
        {
            std::swap(d_i, d_j);
            d_j *= -1;
            t_++;
            if (t_ > 1)
            {
                steps_++;
                t_ = 0;
            }

            s_ = 0;
        }

        return true;
    }

private:
    int i_, j_, max_, steps_, t_, s_, count_, d_i, d_j;
};

class Pathfinder
{
public:
    Pathfinder(const Vector3f &position,
               const float &width, const float &height,
               const int &gridCols, const int &gridRows);
    ~Pathfinder();

    void clearGrid();
    void setGrid(const std::vector<int> &grid);

    void addObstacle(const Entity &entity);

    bool isAreaFree(const Vector3f &position, const Vector3f &size) const;
    bool findFreeCell(const Vector3f &position, int &out_i, int &out_j) const;
    bool findFreePosition(const Vector3f &position, Vector3f &out_position) const;

    bool findPath(const Vector3f &start, const Vector3f &end,
                  const bool &diagonal,
                  std::deque<Vector3f> &resultPath);

    bool searchAStar(const int &start_i, const int &start_j,
                     const int &end_i, const int &end_j,
                     const bool &diagonal,
                     std::vector<std::pair<int, int>> &resultPath);

    void printGrid(const int &start_i, const int &start_j,
                   const int &end_i, const int &end_j,
                   const std::vector<std::pair<int, int>> &resultPath) const;

    void printGrid(const int &start_i, const int &start_j,
                   const int &end_i, const int &end_j) const;

    const int &getRuns() const { return runs_; }
    const int &getNodesUsed() const { return nodesUsed_; }
    const int &getNodesReused() const { return reusedNodes_; }

    int gridIndex(const int &i, const int &j) const { return index_(i, j); }
    void toGridCoord(const Vector3f &point, int &out_i, int &out_j) const;
    Vector3f toPoint(const int &i, const int &j) const;

    const int &cellValue(const int &i, const int &j) const;
    const int &cellValue(const Vector3f &point) const;
    void setCellValue(const int &i, const int &j, const int &value);

    const float &getWidth() const { return width_; }
    const float &getHeight() const { return height_; }
    const int &getCols() const { return g_cols_; }
    const int &getRows() const { return g_rows_; }

private:
    Vector3f position_;
    float width_;
    float height_;
    int g_cols_;
    int g_rows_;

    float cellWidth_;
    float cellHeight_;

    int runs_;
    int nodesUsed_;
    int reusedNodes_;

    int zero_;

    std::vector<std::pair<int, int>> resultPathCells_;

    std::vector<int> grid_;

    std::vector<Node *> nodeList_;

    std::unordered_map<int, Node *> openList_;

    std::unordered_map<int, bool> closedList_;

    std::priority_queue<Node *, std::vector<Node *>, MoreThanByF> openQueue_;

    Node *newNode(const int &I, const int &J,
                  const float &G, const float &H, const float &F,
                  Node *Parent);
    void cleanUp();

    int index_(const int &i, const int &j) const { return i + g_cols_ * j; }
    bool validIndex_(const int &i, const int &j) const;
    bool validCell_(const int &i, const int &j) const;

    bool validAdjacent_(const int &i, const int &j, const int &center_i, const int &center_j, const bool &diagonal) const;
};

#endif // __PATHFINDER_H__
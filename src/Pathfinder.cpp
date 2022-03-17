#include "Pathfinder.hpp"

Pathfinder::Pathfinder(const Vector3f &position,
                       const float &width, const float &height,
                       const int &gridCols, const int &gridRows) : position_(position),
                                                                   width_(width),
                                                                   height_(height),
                                                                   g_cols_(gridCols),
                                                                   g_rows_(gridRows),
                                                                   zero_(0),
                                                                   active_cols_(g_cols_),
                                                                   active_rows_(g_rows_)
{
    cellWidth_ = width_ / ((float)g_cols_);
    cellHeight_ = height_ / ((float)g_rows_);

    grid_.resize(g_cols_ * g_rows_);
}

Pathfinder::~Pathfinder()
{
}

void Pathfinder::clearGrid()
{
    std::fill(grid_.begin(), grid_.end(), 1);
}

void Pathfinder::setGrid(const std::vector<int> &grid)
{
    grid_ = grid;
}

void Pathfinder::setActiveGridArea(const int &cols, const int &rows)
{
    active_cols_ = cols;
    active_rows_ = rows;
}

void Pathfinder::addObstacle(const Entity &entity)
{
    Vector3f topLeft = entity.getPosition() - position_ - (entity.getSize() / 2.f);

    int start_i = (int)floor(topLeft.x / cellWidth_);
    int start_j = (int)floor(topLeft.y / cellHeight_);

    int end_i = (int)ceil((topLeft.x + entity.getSize().x) / cellWidth_);
    int end_j = (int)ceil((topLeft.y + entity.getSize().y) / cellHeight_);

    for (int i = start_i; i < end_i; i++)
    {
        for (int j = start_j; j < end_j; j++)
        {
            setCellValue(i, j, 0);
        }
    }
}

bool Pathfinder::isAreaFree(const Vector3f &localPosition, const Vector3f &size) const
{
    Vector3f topLeft = localPosition - (size / 2.f);

    int start_i = (int)floor(topLeft.x / cellWidth_);
    int start_j = (int)floor(topLeft.y / cellHeight_);

    int end_i = (int)ceil((topLeft.x + size.x) / cellWidth_);
    int end_j = (int)ceil((topLeft.y + size.y) / cellHeight_);

    for (int i = start_i; i < end_i; i++)
    {
        for (int j = start_j; j < end_j; j++)
        {
            if (cellValue(i, j) == 0)
            {
                return false;
            }
        }
    }

    return true;
}

bool Pathfinder::findFreeCell(const Vector3f &position, int &out_i, int &out_j) const
{
    int i, j;

    toGridCoord(position, i, j);

    if (validCell_(i, j))
    {
        out_i = i;
        out_j = j;
        return true;
    }

    SpiralOut spiralOut(i, j, g_cols_ * g_rows_);

    while (spiralOut.next(i, j))
    {
        if (validCell_(i, j))
        {
            out_i = i;
            out_j = j;
            return true;
        }
    }

    return false;
}

bool Pathfinder::findFreePosition(const Vector3f &position, Vector3f &out_position) const
{
    int i, j;
    if (!findFreeCell(position, i, j))
    {
        return false;
    }
    out_position = toPoint(i, j);
    return true;
}

bool Pathfinder::findPath(const Vector3f &start, const Vector3f &end,
                          const bool &diagonal,
                          std::deque<Vector3f> &resultPath)
{
    int start_i, start_j, end_i, end_j;
    toGridCoord(start, start_i, start_j);
    toGridCoord(end, end_i, end_j);

    std::cout << start << start_i << ":" << start_j << "->";
    std::cout << end << end_i << ":" << end_j << " ";

    resultPathCells_.clear();
    bool found = searchAStar(start_i, start_j, end_i, end_j, diagonal, resultPathCells_);

    std::cout << runs_ << "runs ";

    if (!found)
        return false;

    for (auto [i, j] : resultPathCells_)
    {
        resultPath.push_back(toPoint(i, j));
    }

    resultPath.pop_front();

    return true;
}

bool Pathfinder::searchAStar(const int &start_i, const int &start_j,
                             const int &end_i, const int &end_j,
                             const bool &diagonal,
                             std::vector<std::pair<int, int>> &resultPath)
{
    runs_ = 0;

    if (!(validIndex_(start_i, start_j) && validIndex_(end_i, end_j)))
        return false;

    if ((start_i == end_i) && (start_j == end_j))
        return false;

    // Clear the data structures
    openQueue_ = std::priority_queue<Node *, std::vector<Node *>, MoreThanByF>();
    openList_.clear();
    closedList_.clear();

    // Add start node to open list
    Node *start = newNode(start_i, start_j, 0, 0, 0, nullptr);
    openList_[index_(start_i, start_j)] = start;
    openQueue_.push(start);

    Node *currentNode;
    float child_g, child_h, child_f;
    int currentNodeIndex, child_i, child_j, childIndex;
    int endIndex = index_(end_i, end_j);
    reusedNodes_ = 0;
    Node *newChild;
    std::unordered_map<int, bool>::iterator closedSearch;
    std::unordered_map<int, Node *>::iterator foundNode;
    while (!openQueue_.empty())
    {
        runs_ += 1;

        // Pop the openNode with lowest f
        currentNode = openQueue_.top();
        openQueue_.pop();
        currentNodeIndex = index_(currentNode->i, currentNode->j);

        // Make sure current node has not been closed yet
        closedSearch = closedList_.find(currentNodeIndex);
        if (closedSearch == closedList_.end())
        {
            closedList_[currentNodeIndex] = true;

            if (currentNodeIndex == endIndex)
            {
                // Reached the end backtrack and build the path from currentNode
                while (currentNode != nullptr)
                {
                    resultPath.insert(resultPath.begin(), std::pair<int, int>(currentNode->i, currentNode->j));
                    currentNode = currentNode->parent;
                }

                cleanUp();
                return true;
            }

            // Look at all adjacent nodes
            for (int ci = -1; ci < 2; ci++)
            {
                for (int cj = -1; cj < 2; cj++)
                {
                    if (validAdjacent_(ci, cj, currentNode->i, currentNode->j, diagonal))
                    {
                        child_i = currentNode->i + ci;
                        child_j = currentNode->j + cj;
                        childIndex = index_(child_i, child_j);
                        // Check to see if child node is closed
                        closedSearch = closedList_.find(childIndex);
                        if (closedSearch == closedList_.end())
                        {
                            child_g = currentNode->g + 1;
                            child_h = (end_i - child_i) * (end_i - child_i) + (end_j - child_j) * (end_j - child_j);
                            child_f = child_g + child_h;
                            foundNode = openList_.find(childIndex);
                            if (foundNode == openList_.end())
                            {
                                // Add child node to open list
                                newChild = newNode(
                                    child_i,
                                    child_j,
                                    child_g,
                                    child_h,
                                    child_f,
                                    currentNode);
                                openList_[index_(child_i, child_j)] = newChild;
                                openQueue_.push(newChild);
                            }
                            else
                            {
                                // Update the existing node
                                foundNode->second->g = child_g;
                                foundNode->second->h = child_h;
                                foundNode->second->f = child_f;
                                foundNode->second->parent = currentNode;
                                openQueue_.push(foundNode->second);
                                reusedNodes_ += 1;
                            }
                        }
                    }
                }
            }
        }
    }

    cleanUp();
    return false;
}

bool Pathfinder::validIndex_(const int &i, const int &j) const
{
    if (i < 0)
        return false;
    if (i > active_cols_ - 1)
        return false;
    if (j < 0)
        return false;
    if (j > active_rows_ - 1)
        return false;
    return true;
}

bool Pathfinder::validCell_(const int &i, const int &j) const
{
    if (!validIndex_(i, j))
        return false;

    if (grid_[index_(i, j)] == 0)
        return false;

    return true;
}

bool Pathfinder::validAdjacent_(const int &i, const int &j,
                                const int &center_i, const int &center_j,
                                const bool &diagonal) const
{
    if (!(abs(i) || abs(j)))
        return false;

    if (!validCell_(i + center_i, j + center_j))
        return false;

    if ((abs(i) != abs(j)))
    {
        return true;
    }

    if (!diagonal)
    {
        return false;
    }

    int adj_i = i + (0 - i) + center_i;
    int adj_j = j + center_j;
    if (validIndex_(adj_i, adj_j))
    {
        if (grid_[index_(adj_i, adj_j)] == 0)
        {
            return false;
        }
    }

    adj_i = i + center_i;
    adj_j = j + (0 - j) + center_j;
    if (validIndex_(adj_i, adj_j))
    {
        if (grid_[index_(adj_i, adj_j)] == 0)
        {
            return false;
        }
    }

    return true;
}

Node *Pathfinder::newNode(const int &I, const int &J,
                          const float &G, const float &H, const float &F,
                          Node *Parent)
{
    Node *n = new Node(I, J, G, H, F, Parent);
    nodeList_.push_back(n);
    return n;
}

void Pathfinder::cleanUp()
{
    nodesUsed_ = nodeList_.size();
    for (auto &node : nodeList_)
        delete node;
    nodeList_.clear();
}

void Pathfinder::printGrid(const int &start_i, const int &start_j,
                           const int &end_i, const int &end_j,
                           const std::vector<std::pair<int, int>> &resultPath) const
{
    std::vector<int> grid = grid_;

    for (auto &node : resultPath)
    {
        grid[index_(node.first, node.second)] = 80;
    }

    grid[index_(start_i, start_j)] = 90;
    grid[index_(end_i, end_j)] = 99;

    std::cout << "Grid (" << g_cols_ << "x" << g_rows_ << ")\n";

    for (int j = 0; j < g_rows_; j++)
    {
        std::cout << " ";
        for (int i = 0; i < g_cols_; i++)
        {
            switch (grid[index_(i, j)])
            {
            case 1:
                std::cout << ".";
                break;
            case 0:
                std::cout << "#";
                break;
            case 90:
                std::cout << "S";
                break;
            case 99:
                std::cout << "E";
                break;
            case 80:
                std::cout << "X";
                break;
            default:
                std::cout << "?";
                break;
            }
            std::cout << " ";
        }
        std::cout << "\n";
    }
}

void Pathfinder::printGrid(const int &start_i, const int &start_j,
                           const int &end_i, const int &end_j) const
{
    printGrid(start_i, start_j, end_i, end_j, resultPathCells_);
}

void Pathfinder::toGridCoord(const Vector3f &point, int &out_i, int &out_j) const
{
    out_i = (int)floor((point.x - position_.x) / cellWidth_);
    out_j = (int)floor((point.y - position_.y) / cellHeight_);
}

void Pathfinder::toLocalGridCoord(const Vector3f &localPoint, int &out_i, int &out_j) const
{
    out_i = (int)floor((localPoint.x) / cellWidth_);
    out_j = (int)floor((localPoint.y) / cellHeight_);
}

Vector3f Pathfinder::toPoint(const int &i, const int &j) const
{
    return position_ + toLocalPoint(i, j);
}

Vector3f Pathfinder::toLocalPoint(const int &i, const int &j) const
{
    return Vector3f(
        (float)i * cellWidth_ + cellWidth_ / 2.f,
        (float)j * cellHeight_ + cellHeight_ / 2.f,
        0);
}

const int &Pathfinder::cellValue(const int &i, const int &j) const
{
    if (!validIndex_(i, j))
        return zero_;

    return grid_[index_(i, j)];
}

const int &Pathfinder::cellValue(const Vector3f &point) const
{
    int i, j;
    toGridCoord(point, i, j);
    return cellValue(i, j);
}

void Pathfinder::setCellValue(const int &i, const int &j, const int &value)
{
    if (!validIndex_(i, j))
        return;

    grid_[index_(i, j)] = value;
}
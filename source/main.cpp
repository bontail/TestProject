#include <iostream>
#include <queue>
#include <ranges>
#include <vector>
#include <climits>

#define MAX_EDGE_WEIGHT 9
#define NULL_PARENT (-1)

using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::vector;
using std::pair;
using std::queue;


bool isForceAnswer(int64_t startLine, int64_t startColumn, int64_t finishLine, int64_t finishColumn) {
    return startLine == finishLine && startColumn == finishColumn;
}

void printAnswer(const vector<pair<int64_t, int64_t> > &answer) {
    for (auto &[line, column]: std::ranges::reverse_view{answer}) {
        cout << line << ' ' << column << endl;
    }
    cout << '.' << endl;
}

void buildGraph(int64_t lines, int64_t columns, vector<int8_t> &weight, vector<vector<int64_t> > &graph) {
    for (int64_t i = 0; i < lines; ++i) {
        for (int64_t j = 0; j < columns; ++j) {
            int64_t currentIndex = columns * i + j;
            if (weight[currentIndex] == 0) {
                continue;
            }

            if (i > 0 && weight[currentIndex - columns] != 0) {
                graph[currentIndex].push_back(currentIndex - columns);
            }

            if (i < lines - 1 && weight[currentIndex + columns] != 0) {
                graph[currentIndex].push_back(currentIndex + columns);
            }

            if (j > 0 && weight[currentIndex - 1] != 0) {
                graph[currentIndex].push_back(currentIndex - 1);
            }

            if (j < columns - 1 && weight[currentIndex + 1] != 0) {
                graph[currentIndex].push_back(currentIndex + 1);
            }
        }
    }
}


bool buildReversedAnswer(int64_t columns, int64_t startLine, int64_t startColumn, int64_t finishLine,
                         int64_t finishColumn, vector<int64_t> &parent, vector<pair<int64_t, int64_t> > &answer) {
    while (finishColumn != startColumn || finishLine != startLine) {
        answer.emplace_back(finishLine, finishColumn);
        int64_t index = parent[finishLine * columns + finishColumn];
        if (index == -1) {
            return false;
        }
        finishLine = index / columns;
        finishColumn = index % columns;
    }
    answer.emplace_back(finishLine, finishColumn);
    return true;
}


void bfs(vector<vector<int64_t> > &graph, vector<int64_t> &distance, vector<queue<int64_t> > &levels,
         vector<bool> &used, vector<int8_t> &weight, vector<int64_t> &parent) {
    int64_t level = 0, activeNodesCount = 1;
    while (activeNodesCount > 0) {
        while (levels[level % (MAX_EDGE_WEIGHT + 1)].empty()) {
            ++level;
        }

        int64_t currentNode = levels[level % (MAX_EDGE_WEIGHT + 1)].front();
        levels[level % (MAX_EDGE_WEIGHT + 1)].pop();

        --activeNodesCount;

        if (used[currentNode]) {
            continue;
        }

        used[currentNode] = true;
        for (int64_t child: graph[currentNode]) {
            int8_t childWeight = weight[child];
            if (distance[currentNode] + childWeight < distance[child]) {
                distance[child] = distance[currentNode] + childWeight;
                parent[child] = currentNode;
                levels[distance[child] % (MAX_EDGE_WEIGHT + 1)].push(child);
                ++activeNodesCount;
            }
        }
    }
}


int main() {
    vector<queue<int64_t> > levels(MAX_EDGE_WEIGHT);
    int64_t lines, columns;
    cin >> lines >> columns;

    vector<int8_t> weight(lines * columns);
    vector<int64_t> parent(lines * columns,
                           NULL_PARENT); // index of parent node from the shortest path (from start cell to finish cell)
    vector<int64_t> distance(lines * columns, LLONG_MAX); // from start to finish
    vector<bool> used(lines * columns, false);
    vector<vector<int64_t> > graph(lines * columns);

    for (int64_t i = 0; i < lines * columns; ++i) {
        cin >> weight[i];
    }

    int64_t startLine, startColumn, finishLine, finishColumn;
    cin >> startLine >> startColumn >> finishLine >> finishColumn;
    if (not cin || startLine < 0 || startLine >= lines || finishLine < 0 || finishColumn >= columns) {
        cerr << "Incorrect input";
        exit(1);
    }

    if (isForceAnswer(startLine, startColumn, finishLine, finishColumn)) {
        printAnswer({{startLine, startColumn}});
        exit(0);
    }

    buildGraph(lines, columns, weight, graph);

    int64_t startIndex = startLine * columns + startColumn;
    parent[startIndex] = startIndex;
    distance[startIndex] = 0;
    levels[0].push(startIndex);

    bfs(graph, distance, levels, used, weight, parent);

    vector<pair<int64_t, int64_t> > answer;
    if (!buildReversedAnswer(columns, startLine, startColumn, finishLine, finishColumn, parent, answer)) {
        cerr << "The finish cell is out of reach";
        exit(1);
    }
    printAnswer(answer);
}



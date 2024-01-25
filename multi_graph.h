#ifndef MULTI_GRAPH_H
#define MULTI_GRAPH_H

#include <vector>
#include <string>

struct GraphEdge
{
    std::string name;
    float weight[2];
    int endVertexIndex;
};

struct GraphVertex
{
    std::vector<GraphEdge> edges;
    std::string name;
};

class multi_graph
{
private:
    std::vector<GraphVertex> vertexList;

    static float Lerp(float w0, float w1, float alpha);

protected:
public:
    multi_graph();
    multi_graph(const std::string &filePath);

    void InsertVertex(const std::string &vertexName);
    void RemoveVertex(const std::string &vertexName);

    void AddEdge(const std::string &edgeName,
                 const std::string &vertexFromName,
                 const std::string &vertexToName,
                 float weight0, float weight1);
    void RemoveEdge(const std::string &edgeName,
                    const std::string &vertexFromName,
                    const std::string &vertexToName);

    bool HeuristicShortestPath(std::vector<int> &orderedVertexEdgeIndexList,
                               const std::string &vertexNameFrom,
                               const std::string &vertexNameTo,
                               float heuristicWeight) const;
    bool FilteredShortestPath(std::vector<int> &orderedVertexEdgeIndexList,
                              const std::string &vertexNameFrom,
                              const std::string &vertexNameTo,
                              float heuristicWeight,
                              const std::vector<std::string> &edgeNames) const;

    int BiDirectionalEdgeCount() const;
    int MaxDepthViaEdgeName(const std::string &vertexName,
                            const std::string &edgeName) const;

    void PrintPath(const std::vector<int> &orderedVertexEdgeIndexList,
                   float heuristicWeight,
                   bool sameLine = false) const;
    void PrintEntireGraph() const;

public:
    GraphEdge getEdge(const std::string &edgeName,
                      const std::string &vertexFromName,
                      const std::string &vertexToName);

    int getVertexIndex(const std::string &vertexName);
    void getVertexIndexModified(const std::string &vertexName, std::vector<int> &v) const;
};

#endif // MULTI_GRAPH_H
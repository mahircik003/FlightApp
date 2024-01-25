#include "multi_graph.h"
#include "Exceptions.h"
#include "IntPair.h"
#include <iostream>
#include <iomanip>
#include <fstream>

multi_graph::multi_graph()
{
}

multi_graph::multi_graph(const std::string &filePath)
{
    // Tokens
    std::string tokens[5];
    std::ifstream mapFile(filePath.c_str());

    if (!mapFile.is_open())
    {
        std::cout << "Unable to open " << filePath << std::endl;
        return;
    }

    // Read line by line
    std::string line;
    while (std::getline(mapFile, line))
    {
        // Empty Line Skip
        if (line.empty())
            continue;
        // Comment Skip
        if (line[0] == '#')
            continue;

        // Tokenize the line
        int i = 0;
        std::istringstream stream(line);
        while (stream >> tokens[i])
            i++;

        // Single token (Meaning it is a vertex)
        if (i == 1)
        {
            InsertVertex(tokens[0]);
        }
        // Exactly 5 tokens (Meaning it is an edge)
        else if (i == 5)
        {
            const std::string &vertexFromName = tokens[0];
            const std::string &vertexToName = tokens[1];
            const std::string &edgeName = tokens[2];
            float weight0 = static_cast<float>(std::atof(tokens[3].c_str()));
            float weight1 = static_cast<float>(std::atof(tokens[4].c_str()));
            AddEdge(edgeName, vertexFromName, vertexToName,
                    weight0, weight1);
        }
        else
            std::cerr << "Token Size Mismatch" << std::endl;
    }
}

void multi_graph::PrintPath(const std::vector<int> &orderedVertexEdgeIndexList,
                           float heuristicWeight,
                           bool sameLine) const
{

    // Name is too long
    const std::vector<int> &ove = orderedVertexEdgeIndexList;
    // Invalid list
    // At least three items should be available
    if (ove.size() < 3)
        return;

    // Check vertex and an edge
    for (size_t i = 0; i < orderedVertexEdgeIndexList.size(); i += 2)
    {
        int vertexId = ove[i];
        if (vertexId >= static_cast<int>(vertexList.size()))
        {
            // Return if there is a bad vertex id
            std::cout << "VertexId " << vertexId
                      << " not found!" << std::endl;
            return;
        }

        const GraphVertex &vertex = vertexList[vertexId];
        std::cout << vertex.name;
        if (!sameLine)
            std::cout << "\n";
        // Only find and print the weight if next is available
        if (i == ove.size() - 1)
            break;
        int nextVertexId = ove[i + 2];
        if (nextVertexId >= static_cast<int>(vertexList.size()))
        {
            // Return if there is a bad vertex id
            std::cout << "VertexId " << vertexId
                      << " not found!" << std::endl;
            return;
        }

        // Find the edge between these two vertices
        int localEdgeId = ove[i + 1];
        if (localEdgeId >= static_cast<int>(vertex.edges.size()))
        {
            // Return if there is a bad vertex id
            std::cout << "EdgeId " << localEdgeId
                      << " not found in " << vertexId << "!" << std::endl;
            return;
        }

        const GraphEdge &edge = vertex.edges[localEdgeId];

        // Combine with heuristic (linear interpolation)
        float weight = Lerp(edge.weight[0], edge.weight[1],
                            heuristicWeight);

        std::cout << "-" << std::setfill('-')
                  << std::setw(4)
                  << weight << "->";
    }
    // Print endline on the last vertex if same line is set
    if (sameLine)
        std::cout << "\n";
    // Reset fill value because it "sticks" to the std out
    std::cout << std::setfill(' ');
    std::cout.flush();
}

void multi_graph::PrintEntireGraph() const
{
    for (size_t i = 0; i < vertexList.size(); i++)
    {
        const GraphVertex &v = vertexList[i];
        std::cout << v.name << "\n";
        for (size_t j = 0; j < v.edges.size(); j++)
        {
            const GraphEdge &edge = v.edges[j];

            // List the all vertex names and weight
            std::cout << "    -"
                      << std::setfill('-')
                      << std::setw(4) << edge.weight[0]
                      << "-"
                      << std::setw(4) << edge.weight[1]
                      << "-> ";
            std::cout << vertexList[edge.endVertexIndex].name;
            std::cout << " (" << edge.name << ")"
                      << "\n";
        }
    }

    std::cout << std::setfill(' ');
    std::cout.flush();
}

float multi_graph::Lerp(float w0, float w1, float alpha)
{
    return w0 * (1 - alpha) + w1 * alpha;
}

void multi_graph::InsertVertex(const std::string &vertexName)
{
    for (int i = 0; i < vertexList.size(); i++)
    {

        if (vertexList[i].name == vertexName)
            throw DuplicateVertexException(vertexName);
    }

    GraphVertex new_vertex;
    new_vertex.name = vertexName;
    vertexList.push_back(new_vertex);
}

void multi_graph::RemoveVertex(const std::string &vertexName)
{
    for (int i = 0; i < vertexList.size(); i++)
    {

        if (vertexList[i].name == vertexName)
        {
            vertexList[i].edges.clear();

            for (int k = 0; k < vertexList.size(); k++)
            {
                for (int j = 0; j < vertexList[k].edges.size(); j++)
                {
                    if (vertexList[k].edges[j].endVertexIndex == i)
                    {
                        vertexList[k].edges.erase(vertexList[k].edges.begin() + j);
                        break;
                    }
                }
            }

            vertexList.erase(vertexList.begin() + i);
            return;
        }
    }

    throw VertexNotFoundException(vertexName);
}

void multi_graph::AddEdge(const std::string &edgeName,
                         const std::string &vertexFromName,
                         const std::string &vertexToName,
                         float weight0, float weight1)
{
    bool flag2 = false;
    int index;
    for (index = 0; index < vertexList.size(); index++)
    {
        if (vertexList[index].name == vertexToName)
        {
            flag2 = true;
            break;
        }
    } // index to vertexToName is preserved

    if (!flag2)
        throw VertexNotFoundException(vertexToName);

    for (int i = 0; i < vertexList.size(); i++)
    {

        if (vertexList[i].name == vertexFromName)
        {

            for (int q = 0; q < vertexList[i].edges.size(); q++)
            {
                if (vertexList[i].edges[q].name == edgeName && vertexList[i].edges[q].endVertexIndex == index)
                    throw SameNamedEdgeException(edgeName, vertexFromName, vertexToName);
            }

            GraphEdge new_edge;
            new_edge.name = edgeName;
            new_edge.weight[0] = weight0;
            new_edge.weight[1] = weight1;
            new_edge.endVertexIndex = index;
            vertexList[i].edges.push_back(new_edge);
            return;
        }
    }

    throw VertexNotFoundException(vertexFromName);
}

void multi_graph::RemoveEdge(const std::string &edgeName,
                            const std::string &vertexFromName,
                            const std::string &vertexToName)
{
    bool flag1 = false;
    for (int i = 0; i < vertexList.size(); i++)
    {
        if (vertexList[i].name == vertexFromName)
        {
            flag1 = true;
            break;
        }
    }

    if (!flag1)
        throw VertexNotFoundException(vertexFromName);

    flag1 = false;
    for (int i = 0; i < vertexList.size(); i++)
    {
        if (vertexList[i].name == vertexToName)
        {
            flag1 = true;
            break;
        }
    }

    if (!flag1)
        throw VertexNotFoundException(vertexToName);

    flag1 = false;

    for (int i = 0; i < vertexList.size(); i++)
    {
        int k = 0;
        for (k = 0; k < vertexList[i].edges.size(); k++)
        {

            if (vertexList[i].edges[k].name == edgeName)
            {
                flag1 = true;
                break;
            }
        }

        if (!flag1)
            throw EdgeNotFoundException(vertexFromName, edgeName);

        vertexList[i].edges.erase(vertexList[i].edges.begin() + k);
        return;
    }
}

bool multi_graph::HeuristicShortestPath(std::vector<int> &orderedVertexEdgeIndexList,
                                       const std::string &vertexNameFrom,
                                       const std::string &vertexNameTo,
                                       float heuristicWeight) const
{
    MaxPairHeap<int, GraphVertex> pq;

    std::vector<Pair<int, GraphVertex>> pairCounts;
    std::vector<int> counts;
    std::vector<int> prev;
    std::vector<int> Edges;

    Pair<int, GraphVertex> a;

    for (int i = 0; i < vertexList.size(); i++)
    {
        pairCounts.push_back(a);
        counts.push_back(50000);
        prev.push_back(-1);
        Edges.push_back(-1);
    }

    for (int i = 0; i < pairCounts.size(); i++)
    {

        Pair<int, GraphVertex> pair;
        pair.value = vertexList[i]; // put the vertex here
        pair.key = i;               // put its distance from start here
        pairCounts[i] = pair;
    }

    int index;
    for (index = 0; index < vertexList.size(); index++)
    {
        if (vertexList[index].name == vertexNameFrom)
            break;
    } // we have the index of the first vertex
    int index_first = index;

    int index_end;
    for (index_end = 0; index_end < vertexList.size(); index_end++)
    {
        if (vertexList[index_end].name == vertexNameTo)
            break;
    } // we have the index of the last vertex

    counts[index] = 0; // assigning A->A to 0
    prev[index] = -10;

    // implementing dijkstra's algo
    pq.push(pairCounts[index]);

    while (!pq.empty())
    {

        Pair<int, GraphVertex> a = pq.top();

        pq.pop();

        index = a.key;

        for (int i = 0; i < a.value.edges.size(); i++)
        {

            float weight = Lerp(a.value.edges[i].weight[0], a.value.edges[i].weight[1], heuristicWeight);

            int next_index = a.value.edges[i].endVertexIndex;

            if (counts[index] + weight < counts[next_index])
            {

                counts[next_index] = counts[index] + weight;
                prev[next_index] = a.key;
                Edges[next_index] = i;

                pq.push(pairCounts[next_index]);
            }
        }

        if (a.key == index_end)
            break;
    }

    int index2 = index_end;

    for (; index_first != index_end; index_end = prev[index_end])
    {

        if (index_end == -1)
            return false;
    }

    orderedVertexEdgeIndexList.clear();

    for (; index_first != index2; index2 = prev[index2])
    {

        orderedVertexEdgeIndexList.insert(orderedVertexEdgeIndexList.begin(), index2);

        orderedVertexEdgeIndexList.insert(orderedVertexEdgeIndexList.begin(), Edges[index2]);
    }

    orderedVertexEdgeIndexList.insert(orderedVertexEdgeIndexList.begin(), index2);

    orderedVertexEdgeIndexList.insert(orderedVertexEdgeIndexList.begin(), Edges[index2]);

    orderedVertexEdgeIndexList.erase(orderedVertexEdgeIndexList.begin());

    return true;
}

bool multi_graph::FilteredShortestPath(std::vector<int> &orderedVertexEdgeIndexList,
                                      const std::string &vertexNameFrom,
                                      const std::string &vertexNameTo,
                                      float heuristicWeight,
                                      const std::vector<std::string> &edgeNames) const
{
    MaxPairHeap<int, GraphVertex> pq;

    std::vector<Pair<int, GraphVertex>> pairCounts;
    std::vector<int> counts;
    std::vector<int> prev;
    std::vector<int> Edges;

    Pair<int, GraphVertex> a;

    for (int i = 0; i < vertexList.size(); i++)
    {
        pairCounts.push_back(a);
        counts.push_back(50000);
        prev.push_back(-1);
        Edges.push_back(-1);
    }

    for (int i = 0; i < pairCounts.size(); i++)
    {

        Pair<int, GraphVertex> pair;
        pair.value = vertexList[i]; // put the vertex here
        pair.key = i;               // put its distance from start here
        pairCounts[i] = pair;
    }

    int index;
    for (index = 0; index < vertexList.size(); index++)
    {
        if (vertexList[index].name == vertexNameFrom)
            break;
    } // we have the index of the first vertex
    int index_first = index;

    int index_end;
    for (index_end = 0; index_end < vertexList.size(); index_end++)
    {
        if (vertexList[index_end].name == vertexNameTo)
            break;
    } // we have the index of the last vertex

    counts[index] = 0; // assigning A->A to 0
    prev[index] = -10;

    // implementing dijkstra's algo
    pq.push(pairCounts[index]);

    while (!pq.empty())
    {

        Pair<int, GraphVertex> a = pq.top();

        pq.pop();

        index = a.key;

        for (int i = 0; i < a.value.edges.size(); i++)
        {

            bool flag = false;

            for (int k = 0; k < edgeNames.size(); k++)
            {
                if (a.value.edges[i].name == edgeNames[k])
                {
                    flag = true;
                    break;
                }
            }

            if (flag)
                continue;

            float weight = Lerp(a.value.edges[i].weight[0], a.value.edges[i].weight[1], heuristicWeight);

            int next_index = a.value.edges[i].endVertexIndex;

            if (counts[index] + weight < counts[next_index])
            {

                counts[next_index] = counts[index] + weight;
                prev[next_index] = a.key;
                Edges[next_index] = i;

                pq.push(pairCounts[next_index]);
            }
        }

        if (a.key == index_end)
            break;
    }

    int index2 = index_end;

    for (; index_first != index_end; index_end = prev[index_end])
    {

        if (index_end == -1)
            return false;
    }

    orderedVertexEdgeIndexList.insert(orderedVertexEdgeIndexList.begin(), index2);

    orderedVertexEdgeIndexList.insert(orderedVertexEdgeIndexList.begin(), Edges[index2]);

    for (; index_first != index2; index2 = prev[index2])
    {

        orderedVertexEdgeIndexList.insert(orderedVertexEdgeIndexList.begin(), prev[index2]);

        orderedVertexEdgeIndexList.insert(orderedVertexEdgeIndexList.begin(), Edges[prev[index2]]);
    }

    orderedVertexEdgeIndexList.erase(orderedVertexEdgeIndexList.begin());

    return true;
}

int multi_graph::BiDirectionalEdgeCount() const
{
    int counter = 0;

    for (int i = 0; i < vertexList.size(); i++)
    {
        for (int k = 0; k < vertexList[i].edges.size(); k++)
        {

            int toIndex = vertexList[i].edges[k].endVertexIndex;

            for (int l = 0; l < vertexList[toIndex].edges.size(); l++)
            {
                if (vertexList[toIndex].edges[l].endVertexIndex == i && vertexList[i].edges[k].name == vertexList[toIndex].edges[l].name)
                    counter++;
            }
        }
    }
    return counter / 2;
}

int multi_graph::MaxDepthViaEdgeName(const std::string &vertexName,
                                    const std::string &edgeName) const
{
    MaxPairHeap<int, GraphVertex> pq;

    std::vector<Pair<int, GraphVertex>> pairCounts;
    std::vector<int> counts;

    Pair<int, GraphVertex> a;

    for (int i = 0; i < vertexList.size(); i++)
    {
        pairCounts.push_back(a);
        counts.push_back(50000);
    }

    for (int i = 0; i < pairCounts.size(); i++)
    {

        Pair<int, GraphVertex> pair;
        pair.value = vertexList[i]; // put the vertex here
        pair.key = i;               // put its indexfrom start here
        pairCounts[i] = pair;
    }

    int index;
    for (index = 0; index < vertexList.size(); index++)
    {
        if (vertexList[index].name == vertexName)
            break;
    } // we have the index of the first vertex

    counts[index] = 0;

    // implementing dijkstra's algo
    pq.push(pairCounts[index]);

    while (!pq.empty())
    {

        Pair<int, GraphVertex> a = pq.top();

        pq.pop();

        index = a.key;

        for (int i = 0; i < a.value.edges.size(); i++)
        {

            if (a.value.edges[i].name == edgeName)
            {

                int next_index = a.value.edges[i].endVertexIndex;

                if (counts[index] + 1 < counts[next_index])
                {

                    counts[next_index] = counts[index] + 1;

                    pq.push(pairCounts[next_index]);
                }
            }
        }
    }

    int maximum = -20;

    for (int i = 0; i < counts.size(); i++)
    {
        if (counts[i] > maximum && counts[i] != 50000)
        {
            maximum = counts[i];
        }
    }

    return maximum;
}

GraphEdge multi_graph::getEdge(const std::string &edgeName,
                              const std::string &vertexFromName,
                              const std::string &vertexToName)
{

    for (int i = 0; i < vertexList.size(); i++)
    {
        if (vertexList[i].name == vertexFromName)
        {
            for (int k = 0; k < vertexList[i].edges.size(); k++)
            {
                for (int l = 0; l < vertexList.size(); l++)
                {

                    if (vertexList[i].edges[k].name == edgeName && vertexList[l].name == vertexToName && vertexList[i].edges[k].endVertexIndex == l)
                        return vertexList[i].edges[k];
                }
            }
        }
    }

    throw EdgeNotFoundException(vertexFromName, edgeName);
}

int multi_graph::getVertexIndex(const std::string &vertexName)
{

    for (int i = 0; i < vertexList.size(); i++)
    {
        if (vertexList[i].name == vertexName)
            return i;
    }

    throw VertexNotFoundException(vertexName);
}

void multi_graph::getVertexIndexModified(const std::string &vertexName, std::vector<int> &v) const
{

    for (int i = 0; i < vertexList.size(); i++)
    {
        if (vertexList[i].name == vertexName)
            v.push_back(i);
        return;
    }

    throw VertexNotFoundException(vertexName);
}
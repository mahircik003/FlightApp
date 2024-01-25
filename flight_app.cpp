#include "flight_app.h"
#include <iostream>

void flight_app::PrintCanNotHalt(const std::string &airportFrom,
                                 const std::string &airportTo,
                                 const std::string &airlineName)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" via "
              << airlineName
              << " airlines is not found and cannot be halted"
              << std::endl;
}

void flight_app::PrintCanNotResumeFlight(const std::string &airportFrom,
                                         const std::string &airportTo,
                                         const std::string &airlineName)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" via "
              << airlineName
              << " airlines cannot be resumed"
              << std::endl;
}

void flight_app::PrintFlightFoundInCache(const std::string &airportFrom,
                                         const std::string &airportTo,
                                         bool isCostWeighted)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" using "
              << ((isCostWeighted) ? "cost" : "price")
              << " is found in cache." << std::endl;
}

void flight_app::PrintFlightCalculated(const std::string &airportFrom,
                                       const std::string &airportTo,
                                       bool isCostWeighted)
{
    std::cout << "A flight path is calculated between \""
              << airportFrom << "\" and \""
              << airportTo << "\" using "
              << ((isCostWeighted) ? "cost" : "price")
              << "." << std::endl;
}

void flight_app::PrintPathDontExist(const std::string &airportFrom,
                                    const std::string &airportTo)
{
    std::cout << "A flight path does not exists between \""
              << airportFrom << "\" and \""
              << airportTo << "\"." << std::endl;
}

void flight_app::PrintMap()
{
    navigationMap.PrintEntireGraph();
}

void flight_app::PrintCache()
{
    lruTable.PrintTable();
}

flight_app::flight_app(const std::string &flightMapPath)
    : navigationMap(flightMapPath)
{
}


void flight_app::HaltFlight(const std::string &airportFrom,
                            const std::string &airportTo,
                            const std::string &airlineName)
{

    try
    {
        HaltedFlight removable;
        removable.airline = airlineName;
        removable.airportFrom = airportFrom;
        removable.airportTo = airportTo;
        removable.w0 = navigationMap.getEdge(airlineName, airportFrom, airportTo).weight[0];
        removable.w1 = navigationMap.getEdge(airlineName, airportFrom, airportTo).weight[1];

        navigationMap.RemoveEdge(airlineName, airportFrom, airportTo);

        haltedFlights.push_back(removable);
    }

    catch (struct VertexNotFoundException)
    {
        PrintCanNotHalt(airportFrom, airportTo, airlineName);
    }
    catch (struct EdgeNotFoundException)
    {
        PrintCanNotHalt(airportFrom, airportTo, airlineName);
    }
}

void flight_app::ContinueFlight(const std::string &airportFrom,
                                const std::string &airportTo,
                                const std::string &airlineName)
{

    try
    {
        bool flag = true;
        for (int i = 0; i < haltedFlights.size(); i++)
        {

            if (haltedFlights[i].airline == airlineName && haltedFlights[i].airportFrom == airportFrom && haltedFlights[i].airportTo == airportTo)
            {
                navigationMap.AddEdge(airlineName, airportFrom, airportTo, haltedFlights[i].w0, haltedFlights[i].w1);
                flag = false;
                break;
            }
        }

        if (flag)
            throw EdgeNotFoundException(airportFrom, airlineName);
    }

    catch (struct SameNamedEdgeException)
    { // if was not halted before
        PrintCanNotResumeFlight(airportFrom, airportTo, airlineName);
    }

    catch (struct EdgeNotFoundException)
    { // does not exist
        PrintCanNotResumeFlight(airportFrom, airportTo, airlineName);
    }
}

void flight_app::FindFlight(const std::string &startAirportName,
                            const std::string &endAirportName,
                            float alpha)
{
    std::vector<int> path;
    bool indicator = navigationMap.HeuristicShortestPath(path, startAirportName, endAirportName, alpha);

    if (alpha == 0 || alpha == 1)
    {
        if (lruTable.Find(path, navigationMap.getVertexIndex(startAirportName), navigationMap.getVertexIndex(endAirportName), 1 - alpha, true))
        {
            PrintFlightFoundInCache(startAirportName, endAirportName, 1 - alpha);
            navigationMap.PrintPath(path, alpha, true);
            return;
        }
        if (indicator)
        {
            lruTable.Insert(path, 1 - alpha);

            PrintFlightCalculated(startAirportName, endAirportName, 1 - alpha);
            navigationMap.PrintPath(path, alpha, true);
            return;
        }
    }

    if (indicator)
    {
        navigationMap.PrintPath(path, alpha, true);
    }

    else
    {
        PrintPathDontExist(startAirportName, endAirportName);
    }
}

void flight_app::FindSpecificFlight(const std::string &startAirportName,
                                    const std::string &endAirportName,
                                    float alpha,
                                    const std::vector<std::string> &unwantedAirlineNames) const
{
 

    std::vector<int> path;
    bool indicator = navigationMap.FilteredShortestPath(path, startAirportName, endAirportName, alpha, unwantedAirlineNames);

    if (indicator)
    {
        navigationMap.PrintPath(path, alpha, true);
    }

    else
    {
        PrintPathDontExist(startAirportName, endAirportName);
    }
}

int flight_app::FurthestTransferViaAirline(const std::string &airportName,
                                           const std::string &airlineName) const
{
    try
    {
        int a = navigationMap.MaxDepthViaEdgeName(airportName, airlineName);

        return a;
    }
    catch (...)
    {
        return -1;
    }
}
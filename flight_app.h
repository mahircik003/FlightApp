#ifndef CENG_FLIGHT_H
#define CENG_FLIGHT_H

#include "HashTable.h"
#include "multi_graph.h"

#define FLIGHT_TABLE_SIZE 29

struct HaltedFlight
{
    std::string airportFrom;
    std::string airportTo;
    std::string airline;
    float w0;
    float w1;
};

class flight_app
{
private:
    HashTable<FLIGHT_TABLE_SIZE> lruTable;
    multi_graph navigationMap;

    //Print functions
    static void PrintCanNotHalt(const std::string &airportFrom,
                                const std::string &airportTo,
                                const std::string &airlineName);
    static void PrintCanNotResumeFlight(const std::string &airportFrom,
                                        const std::string &airportTo,
                                        const std::string &airlineName);
    static void PrintFlightFoundInCache(const std::string &airportFrom,
                                        const std::string &airportTo,
                                        bool isCostWeighted);
    static void PrintFlightCalculated(const std::string &airportFrom,
                                      const std::string &airportTo,
                                      bool isCostWeighted);
    static void PrintPathDontExist(const std::string &airportFrom,
                                   const std::string &airportTo);

    static void PrintSisterAirlinesDontCover(const std::string &airportFrom);


    std::vector<HaltedFlight> haltedFlights;
    

protected:
public:
    flight_app
    (const std::string &flightMapPath);

    
    void HaltFlight(const std::string &airportFrom,
                    const std::string &airportTo,
                    const std::string &airlineName);

    
    void ContinueFlight(const std::string &airportFrom,
                        const std::string &airportTo,
                        const std::string &airlineName);

    void FindFlight(const std::string &startAirportName,
                    const std::string &endAirportName,
                    float alpha);

    void FindSpecificFlight(const std::string &startAirportName,
                            const std::string &endAirportName,
                            float alpha,
                            const std::vector<std::string> &unwantedAirlineNames) const;

    int FurthestTransferViaAirline(const std::string &airportName,
                                   const std::string &airlineName) const;

    void PrintMap();
    void PrintCache();
};

#endif // CENG_FLIGHT_H
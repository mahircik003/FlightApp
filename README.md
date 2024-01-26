# FlightApp

A flight app, which is based on Multigraphs and HashTables.

Airports are represented as Vertices of the multigraph, while flights between airports correspond to edges of the multigraph.

Users can specify flights according to convenient flight time and flight price.

HashTable is used for caching. Certain user-specified flights (specification according to only flight time or flight price) are stored in cache for fast fetching.

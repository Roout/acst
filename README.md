# Asynchronious client-server template

## Purpose

1. To be used as template for testing different configurations

2. Test interaction of multiply sockets (from single client) with one server.

## Prerequisites

- CMake 3.16+
- c++17 standard
- boost.asio, boost.format (used 1.72.0 but can work with earlier versions)

## Consist of

- client
- server
- mock (implementation of mock async read/write stream. Planning to use for tests)

## Result

The log files in folder with asio-server executable:

- server.txt
- session_0.txt
- session_1.txt
- session_2.txt
- session_3.txt

The log files in folder with asio-client executable:

- client_0.txt
- client_1.txt
- client_2.txt
- client_3.txt

Example of **server.txt** content:
> 93734683 --info:  Server accepted connection on endpoint: 127.0.0.1:60554  
> 93734684 --info:  Server accepted connection on endpoint: 127.0.0.1:60555  
> 93734685 --info:  Server accepted connection on endpoint: 127.0.0.1:60556  
> 93734685 --info:  Server accepted connection on endpoint: 127.0.0.1:60557  

Example of **session_0.txt** content:
> 93734689 --info:  Received: endpoint=127.0.0.1:60554 with id=0 joined.

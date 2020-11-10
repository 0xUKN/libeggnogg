# libeggnogg
*Binary instrumentation library for EGGNOGG+ by Madgarden*

## Todos
- Add multi-instance compatibility (play on several game at the same time)
- Add a way to insert inputs directly in memory

## Requirements
- rpcbind

## Build
`make libeggnogg.so` : build the instrumentation + hooking library\
`make pyeggnogg` : build Python bindings and install in local bin folder\
`make` : build both

## Installation
`make install` : install Python bindings\
`make uninstall` : uninstall Python bindings

## Clean
`make clean` : delete build and bin folders

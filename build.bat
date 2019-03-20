call waf clean
call waf distclean
call waf configure --disable-osc
call waf

call copy public\monome.h ..\serialosc\include

call copy build\src\libmonome.dll ..\serialosc\lib
call copy build\src\protocol_40h.dll ..\serialosc\lib
call copy build\src\protocol_mext.dll ..\serialosc\lib
call copy build\src\protocol_series.dll ..\serialosc\lib

call copy build\src\libmonome.dll.a ..\serialosc\lib
call copy build\src\libprotocol_40h.dll.a ..\serialosc\lib
call copy build\src\libprotocol_mext.dll.a ..\serialosc\lib
call copy build\src\libprotocol_series.dll.a ..\serialosc\lib
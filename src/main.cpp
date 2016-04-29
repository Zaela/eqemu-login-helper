
#include "define.hpp"
#include "connection.hpp"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

int main()
{
    Connection con;
    
    try
    {
#ifdef _WIN32
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa))
            throw 5;
#endif
        
        con.init();
        
        for (;;)
        {
            con.read();
        }
    }
    catch (int& e)
    {
        printf("Exception: %i\n", e);
    }
    
#ifdef _WIN32
    WSACleanup();
#endif
    
    return 0;
}

#include <genesis.h>

int main()
{

    while(1)
    {
        VDP_drawText("Hello Mega Drive World!", 8, 12);
        VDP_waitVSync();
    }
    return (0);
}

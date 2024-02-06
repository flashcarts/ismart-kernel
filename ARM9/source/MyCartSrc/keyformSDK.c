#include "keyformSDK.h"
#include "../../ipc6.h"

u16 PAD_Read(void)
{
    IPC6->RequestUpdateIPC=true;
    u16 key9 = *(vu16 *)0x04000130 ;
    //需要在release时为1， 按下为 0 ; arm7刚好相反，所以需要按位取反
    u16 key7 = (((~IPC6->buttons) & 0x3)<<10)|(((~IPC6->buttons) & IPC_PEN_DOWN)<<6) ;
    return (u16)(( key9 | key7 ) ^ PAD_ALL_MASK);
}

///////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------*
  Name:         KeyRead

  Description:  Edits key input data.
                Detects press trigger, release trigger and hold repeat trigger.

  Arguments:    pKey  - Structure that holds key input data to be edited.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void KeyRead(KeyInfo * pKey)
{
    static u16 repeat_count[12];
    int     i;
    u16     r;

    r = PAD_Read();
     pKey->trg = 0x0000;
    pKey->up = 0x0000;
    pKey->rep = 0x0000;

    for (i = 0; i < 14; i++)
    {
        if (r & (0x0001 << i))
        {
            if (!(pKey->cnt & (0x0001 << i)))
            {
                pKey->trg |= (0x0001 << i);     // push trigger
                repeat_count[i] = 1;
            }
            else
            {
                if (repeat_count[i] > KEY_REPEAT_START)
                {
                    pKey->rep |= (0x0001 << i); // push and hold repeat
                    repeat_count[i] = KEY_REPEAT_START - KEY_REPEAT_SPAN;
                }
                else
                {
                    repeat_count[i]++;
                }
            }
        }
        else
        {
            if (pKey->cnt & (0x0001 << i))
            {
                pKey->up |= (0x0001 << i);      // release trigger
            }
        }
    }
    pKey->cnt = r;                     // unprocessed key input
}
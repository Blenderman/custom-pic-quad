#include "BLI\BLILocal.h"

typedef struct
    {
    byte    Count;
    byte    DitDah[];
    }    BLIMorseLetter;
// <editor-fold defaultstate="collapsed" desc="Morse letters and Alphabet">
//----------------------------------------------------------
BLIMorseLetter        BLIMorseA    = { 2, { 1, 3 }};
BLIMorseLetter        BLIMorseB    = { 4, { 3, 1, 1, 1 }};
BLIMorseLetter        BLIMorseC    = { 4, { 3, 1, 3, 1 }};
BLIMorseLetter        BLIMorseD    = { 3, { 3, 1, 1 }};
BLIMorseLetter        BLIMorseE    = { 1, { 1 } };
BLIMorseLetter        BLIMorseF    = { 4, { 1, 1, 3, 1 }};
BLIMorseLetter        BLIMorseG    = { 3, { 3, 3, 1 }};
BLIMorseLetter        BLIMorseH    = { 4, { 1, 1, 1, 1 }};
BLIMorseLetter        BLIMorseI    = { 2, { 1, 1 }};
BLIMorseLetter        BLIMorseJ    = { 4, { 1, 3, 3, 3 }};
BLIMorseLetter        BLIMorseK    = { 3, { 3, 1, 3 }};
BLIMorseLetter        BLIMorseL    = { 4, { 1, 3, 1, 1 }};
BLIMorseLetter        BLIMorseM    = { 2, { 3, 3 }};
BLIMorseLetter        BLIMorseN    = { 2, { 3, 1 }};
BLIMorseLetter        BLIMorseO    = { 3, { 3, 3, 3 }};
BLIMorseLetter        BLIMorseP    = { 4, { 1, 3, 3, 1 }};
BLIMorseLetter        BLIMorseQ    = { 4, { 3, 3, 1, 3 }};
BLIMorseLetter        BLIMorseR    = { 3, { 1, 3, 1 }};
BLIMorseLetter        BLIMorseS    = { 3, { 1, 1, 1 }};
BLIMorseLetter        BLIMorseT    = { 1, { 3 }};
BLIMorseLetter        BLIMorseU    = { 3, { 1, 1, 3 }};
BLIMorseLetter        BLIMorseV    = { 4, { 1, 1, 1, 3 }};
BLIMorseLetter        BLIMorseW    = { 3, { 1, 3, 3 }};
BLIMorseLetter        BLIMorseX    = { 4, { 3, 1, 1, 3 }};
BLIMorseLetter        BLIMorseY    = { 4, { 3, 1, 3, 3 }};
BLIMorseLetter        BLIMorseZ    = { 4, { 3, 3, 1, 1 }};
//----------------------------------------------------------
BLIMorseLetter        BLIMorse0    = { 5, { 3, 3, 3, 3, 3 }};
BLIMorseLetter        BLIMorse1    = { 5, { 1, 3, 3, 3, 3 }};
BLIMorseLetter        BLIMorse2    = { 5, { 1, 1, 3, 3, 3 }};
BLIMorseLetter        BLIMorse3    = { 5, { 1, 1, 1, 3, 3 }};
BLIMorseLetter        BLIMorse4    = { 5, { 1, 1, 1, 1, 3 }};
BLIMorseLetter        BLIMorse5    = { 5, { 1, 1, 1, 1, 1 }};
BLIMorseLetter        BLIMorse6    = { 5, { 3, 1, 1, 1, 1 }};
BLIMorseLetter        BLIMorse7    = { 5, { 3, 3, 1, 1, 1 }};
BLIMorseLetter        BLIMorse8    = { 5, { 3, 3, 3, 1, 1 }};
BLIMorseLetter        BLIMorse9    = { 5, { 3, 3, 3, 3, 1 }};
//----------------------------------------------------------
BLIMorseLetter *    BLIMorseAlphabet[] =
        {
        &BLIMorseA,     //  0
        &BLIMorseB,     //  1
        &BLIMorseC,     //  2
        &BLIMorseD,     //  3
        &BLIMorseE,     //  4
        &BLIMorseF,     //  5
        &BLIMorseG,     //  6
        &BLIMorseH,     //  7
        &BLIMorseI,     //  8
        &BLIMorseJ,     //  9
        &BLIMorseK,     // 10
        &BLIMorseL,     // 11
        &BLIMorseM,     // 12
        &BLIMorseN,     // 13
        &BLIMorseO,     // 14
        &BLIMorseP,     // 15
        &BLIMorseQ,     // 16
        &BLIMorseR,     // 17
        &BLIMorseS,     // 18
        &BLIMorseT,     // 19
        &BLIMorseU,     // 20
        &BLIMorseV,     // 21
        &BLIMorseW,     // 22
        &BLIMorseX,     // 23
        &BLIMorseY,     // 24
        &BLIMorseZ,     // 25
        //-------------------
        &BLIMorse0,     // 26
        &BLIMorse1,     // 27
        &BLIMorse2,     // 28
        &BLIMorse3,     // 29
        &BLIMorse4,     // 30
        &BLIMorse5,     // 31
        &BLIMorse6,     // 32
        &BLIMorse7,     // 33
        &BLIMorse8,     // 34
        &BLIMorse9      // 35
        };
//----------------------------------------------------------
// </editor-fold>

BYTE    ASCIItoMorse[128] =
    {
    // 48 special characters (including blank that is processed in a special way)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    // Numbers plus some special characters: '0' to '9' converted into indexes 26 to 35
      26,   27,   28,   29,   30,   31,   32,   33,   34,   35, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    // Upper-case letters
    0xFF,    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
      15,   16,   17,   18,   19,   20,   21,   22,   23,   24,   25, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    // Lower-case letters
    0xFF,    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
      15,   16,   17,   18,   19,   20,   21,   22,   23,   24,   25, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

//************************************************************
void    BLIAsyncMorse( char * pStr, uint Length )
    {
    //---------------------------------------------------
    // First, stop current Async operation, if any
    //---------------------------------------------------
    if (_BLIAsync)
        BLIAsyncStop();
    //---------------------------------------------------
    // Second, if not initialized - Initialize!
    //---------------------------------------------------
    if (0 == _BLIInit)
        BLIInit();
    //---------------------------------------------------
    // Third, all ASYNC operations should start in the
    // OFF State for UI
    //---------------------------------------------------
    BLISignalOFF();
    //---------------------------------------------------
    // Fourth, validate parameters
    //---------------------------------------------------
    if (Length <= 0)
        return;            // Nothing to do...
    //---------------------------------------------------
    int     CharPos     = 0;
    int     NextPlayPos = 0;
    //---------------------------------------------------
    for (CharPos = 0; CharPos < Length; CharPos++)
        {
        int CharValue    = (int)(pStr[CharPos]);

        if (32 == CharValue)
            {
            //---------------------------------------
            // Special processing for SPACE...
            //---------------------------------------
            if (NextPlayPos > 0)
                {
                // Not the first character in a string...
                // Last interval should have been 7 "Dih" to indicate End-of-word
                _BLIAsyncTimes[NextPlayPos-1] = 896;
                }
            //---------------------------------------
            // Go to next character...
            //---------------------------------------
            continue;
            }


        //------------------------------------------------------------------------
        // Validate character...
        //------------------------------------------------------------------------
        if (CharValue > 127 || CharValue < 0)
            continue;               // Ignore characters outside of basic ASCII
        // Convert ASCII code to index into Morse alphabet
        int MorseIdx    = ASCIItoMorse[CharValue];
        if (MorseIdx > 35)
            continue;               // Ignore non-Morse characters
        //------------------------------------------------------------------------
        // Alphabetic character...
        //------------------------------------------------------------------------
        // Retrieve MORSE code structure representing this character
        BLIMorseLetter *    pMorseLetter    = BLIMorseAlphabet[MorseIdx];

        //------------------------------------------------------------------------
        // Evaluate space available in Play Array (limited to _BLIAsynchMax)
        // In the following calculation we need to account for NextPlayPos
        // being a ZERO-based counter (Index)
        //------------------------------------------------------------------------
        if (NextPlayPos + 2*pMorseLetter->Count + 1 > _BLIAsynchMax)
            break;            // Asynchronous play array exhausted...

        //-----------------------------------------------
        int    SignalIdx;
        for (SignalIdx = 0; SignalIdx < pMorseLetter->Count; SignalIdx++)
            {
            // NOTE: 1 "Dih" = 128 msec
            //-------------------------
            _BLIAsyncTimes[NextPlayPos] = ((uint)(pMorseLetter->DitDah[SignalIdx])) << 7;
            NextPlayPos++;
            // Now add 1 "Dih" interval 
            _BLIAsyncTimes[NextPlayPos] = 128;
            NextPlayPos++;    
            }
        // Last interval should have been 3 "Dih" to indicate End-of-character
        _BLIAsyncTimes[NextPlayPos - 1] = 384;
        }
    // Last interval should have been 7 "Dih" to indicate End-of-word
    _BLIAsyncTimes[NextPlayPos - 1] = 896;
    //---------------------------------------------------
    // Initialize asynchronous play of prepared timing array
    //---------------------------------------------------
    // Set size of timing array:
    _BLIAsync            = NextPlayPos;    // Flag Async operation;
    //---------------------------------------------------
    // Request immediate callback to initialize the sequence
    //---------------------------------------------------
    TMRCallBackAfter(0, &_BLICallBack);             
    //---------------------------------------------------
    return;
    }
//************************************************************




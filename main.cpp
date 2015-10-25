#include "mbed.h"
 
#include "Keypad.h"
 
Serial        PC(USBTX, USBRX);
DigitalOut    Led1(LED1);
DigitalOut    Led2(LED2);
 
char Keytable[] = { '1', '2', '3',   // r0
                    '4', '5', '6',   // r1
                    '7', '8', '9',   // r2
                    '*', '0', '#',   // r3
                  };
                 // c0   c1   c2  
 
int OutsideState = 0;
int lastState = 0;
int errorCommand = 0;
int SignalOut;
//0:Main Menu
//1:DAC
//2:Digipot
//3:ADC
//4:Select
 
 
 
int32_t       Index = -1;
int           State;
 
 
uint32_t cbAfterInput(uint32_t _index)
{
    Index = _index;
    return 0;
}
void sendSignal(){
    //send signal here
    PC.printf("%d\n",SignalOut);
}
void printMainMenu()
{
    //clear screen
    PC.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    if (errorCommand == 1){
        PC.printf("Invalid Command\n");
    }
    if (errorCommand == 2){
        sendSignal();
        PC.printf("Command Sent\n");
    }
    errorCommand = 0;
    switch(OutsideState)
    {
        case 0: //0:Main Menu
            PC.printf("Main Menu\n0:DAC\n1:Digipot\n2:ADC\n");
            break;
        case 1://1:DAC
            PC.printf("DAC Menu\n0:Start\n1:Stop\n2:Square\n3:Sin\n*:Back\n");
            break;
        case 2://2:Digipot
            PC.printf("Digipot Menu\n0:0v\n1:1v\n2:2v\n3:3v\n4:4v\n5:5v\n*:Back\n");
            break;
        case 3://3:ADC
            PC.printf("ADC Menu\n0:Start\n1:Stop\n*:Back\n");
            break;
        case 4://4:Select
            PC.printf("#:Confirm\t*:Back\n");
            break;
    }
}
 
void StateMachine(char &inputChar){
    switch(OutsideState)
    {
        case 0: //0:Main Menu
             switch(inputChar){
                case '0':
                    OutsideState = 1;
                    break;
                case '1':
                    OutsideState = 2;
                    break;
                case '2':
                    OutsideState = 3;
                    break;
                default:
                    errorCommand = 1;
            }
            break;
        case 1://1:DAC
            lastState = 1;
            switch(inputChar){
                case '0':
                    OutsideState = 4;
                    SignalOut = 0;//start DAC
                    break;
                case '1':
                    OutsideState = 4;
                    SignalOut = 1;//stop DAC
                    break;
                case '2':
                    OutsideState = 4;
                    SignalOut = 2;//Sqaure Wave
                    break;
                case '3':
                    OutsideState = 4;
                    SignalOut = 3;//Sin Wave
                    break;
                case '*':
                    OutsideState = 0;
                    break;
                default:
                    errorCommand = 1;
            }
            break;
        case 2://2:Digipot
            lastState = 2;
            switch(inputChar){
                case '0':
                    OutsideState = 4;
                    SignalOut = 10;//0v
                    break;
                case '1':
                    OutsideState = 4;
                    SignalOut = 11;//1v
                    break;
                case '2':
                    OutsideState = 4;
                    SignalOut = 12;//2v
                    break;
                case '3':
                    OutsideState = 4;
                    SignalOut = 13;//3v
                    break;
                case '4':
                    OutsideState = 4;
                    SignalOut = 14;//4v
                    break;
                case '5':
                    OutsideState = 4;
                    SignalOut = 15;//5v
                    break;
                case '*':
                    OutsideState = 0;
                    break;
                default:
                    errorCommand = 1;
            }
            break;
        case 3://3:ADC
            lastState = 3;
            switch(inputChar){
                case '0':
                    OutsideState = 4;
                    SignalOut = 20;//start ADC
                    break;
                case '1':
                    OutsideState = 4;
                    SignalOut = 21;//stop ADC
                    break;
                case '*':
                    OutsideState = 0;
                    break;
                default:
                    errorCommand = 1;
            }
            break;
        case 4://4:Select
            switch(inputChar){
                case '#':
                    errorCommand = 2;
                    //send command?
                    OutsideState = lastState;
                    break;
                case '*':
                    OutsideState = lastState;
                    break;
                default:
                    errorCommand = 1;
            }
            break;
    }
    printMainMenu();
}
 
int main()
{
    printMainMenu();
    
    //             r0   r1   r2   r3   c0   c1   c2   c3
    Keypad keypad(p21, p22,  p23,  p24, p25, p26,  p27,  NC);
    keypad.attach(&cbAfterInput);
    keypad.start();  // energize the columns c0-c3 of the keypad
 
    while (1) {
        __wfi();
        //PC.printf("%d/n",Index);
        if (Index > -1) {
            StateMachine(Keytable[Index]);
            Index = -1;
        }
    }
}
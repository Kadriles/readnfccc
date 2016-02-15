#include "CreditCardNFCController.hpp"

int main(int argc, char **argv)
{
    CCreditCardNFCController ccnfcctrl(true);
    int result = ccnfcctrl.init();
    
    if (result == 0) {
        ccnfcctrl.debugMsg("Error : unable to get a target. Exiting.");
        return 1;
    }

    return 0;
}

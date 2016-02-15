#pragma once

#ifndef __CREDIT_CARD_NFC_CONTROLLER_H__
#define __CREDIT_CARD_NFC_CONTROLLER_H__

#include <nfc/nfc.h>

#define NFC_TIMEOUT 500
#define NFC_MAX_ANSWER 255

class CCreditCardNFCController {
private:
    nfc_context *m_nfcContext;
    nfc_device *m_nfcDevice;
    nfc_target *m_nfcTarget;

    bool m_debug;
    
    uint8_t m_answerData[NFC_MAX_ANSWER];
    size_t m_answerLg;
    uint8_t m_answerCode[2];
    
private:
    void debugMsg(const char* message, bool debug = false);
    int send(uint8_t* message, size_t messageLg);
    int sendAPDU(uint8_t ins, uint8_t p1, uint8_t p2, uint8_t* data, size_t dataLg);
    
public:
    CCreditCardNFCController(bool debug = false);
    ~CCreditCardNFCController();
    
    int init();
    
    int sendSelect(uint8_t* data, uint8_t dataLg);
    int sendReadRecord(uint8_t recordNb, uint8_t refCtrlParam);
    
    bool isAnswerOk();
    int getAnswerCode();
};

#endif // __CREDIT_CARD_NFC_CONTROLLER_H__

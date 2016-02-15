#include "CreditCardNFCController.hpp"

CCreditCardNFCController::CCreditCardNFCController(bool debug) {
    m_nfcContext = NULL;
    m_nfcDevice = NULL;
    m_nfcTarget = NULL;
    m_answerCode[0] = 0;
    m_answerCode[1] = 0;
    m_answerLg = 0;
    
    m_debug = debug;
}

CCreditCardNFCController::~CCreditCardNFCController() {
    if(m_nfcDevice != NULL)
        nfc_close(m_nfcDevice);
        
    if(m_nfcContext != NULL)
        nfc_exit(m_nfcContext);
}

void CCreditCardNFCController::debugMsg(const char* message, bool debug) {
    if (m_debug == false && debug == true)
        return;
    
    if (debug == true) 
        printf("[DEBUG] %s\r\n", message);
    else
        printf("[INFO] %s\r\n", message);
}

/*
 *   This function waits for a target to connect.
 */
int CCreditCardNFCController::init() {
    nfc_init(&m_nfcContext);
    
    if (m_nfcContext == NULL) {
        debugMsg("Error : Unable to init libnfc (malloc).");
        return 0;
    }
    
    //const char *acLibnfcVersion = nfc_version();
    //debugMsg("Using libnfc %s.", acLibnfcVersion);
    
    m_nfcDevice = nfc_open(m_nfcContext, NULL);
    if (m_nfcDevice == NULL) {
        debugMsg("Error : Failed to open NFC reader.");
        return 0;
    }
    
    if (nfc_initiator_init(m_nfcDevice) < 0) {
        nfc_perror(m_nfcDevice, "nfc_initiator_init");
        return 0;
    }
    
    // TODO: change this to use debugMsg.
    printf("NFC reader: %s opened.\r\n", nfc_device_get_name(m_nfcDevice));

    nfc_modulation nm;
    nm.nmt = NMT_ISO14443A;
    nm.nbr = NBR_106;
    
    // It waits here...
    if (nfc_initiator_select_passive_target(m_nfcDevice, nm, NULL, 0, m_nfcTarget) <= 0) {
        nfc_perror(m_nfcDevice, "START_14443A");
        return 0;
    }
    
    debugMsg("A target has been selected !");
    
    return 1;
}

int CCreditCardNFCController::send(uint8_t* message, size_t messageLg) {
    m_answerLg = nfc_initiator_transceive_bytes(
        m_nfcDevice,
        message,
        messageLg,
        m_answerData,
        NFC_MAX_ANSWER,
        NFC_TIMEOUT
    );
    
    return m_answerLg;
}

int CCreditCardNFCController::sendSelect(uint8_t* data, uint8_t dataLg) {
    return sendAPDU(0xA4, 0x00, 0x00, data, dataLg);
}

int CCreditCardNFCController::sendReadRecord(uint8_t recordNb, uint8_t refCtrlParam) {
    return sendAPDU(0xB2, recordNb, refCtrlParam, 0x00, 0x00);
}

int CCreditCardNFCController::sendAPDU(uint8_t ins, uint8_t p1, uint8_t p2, uint8_t* data, size_t dataLg) {
    size_t messageLg = 6 + dataLg; 
    
    uint8_t message[255] = { 
        0x00, // CLA
        ins, // INS
        p1, // P1
        p2, // P2
        dataLg // Lc
    };
    
    for (int i = 0; i < dataLg; i++) {
        message[5+i] = data[i]; // Data
    }
    
    message[5+dataLg] = 0x00; // Le
    
    // now that message is formated, let send and store the answer...
    int lg = send(message, messageLg);
    
    // extract the return code.
    if (lg >= 2) {
        m_answerCode[0] = m_answerData[lg-1];
        m_answerCode[1] = m_answerData[lg-2];
    }
    
    return lg;
}

bool CCreditCardNFCController::isAnswerOk() {
    if (getAnswerCode() == 0x9000)
        return true;
        
    return false;
}

int CCreditCardNFCController::getAnswerCode() {
    return m_answerCode[0] << 8 + m_answerCode[1];
}

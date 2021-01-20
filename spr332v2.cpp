#include <stdexcept>

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>

#include "spr332v2.h"

#define IOCTL_CCID_ESCAPE SCARD_CTL_CODE(1)

namespace {
using namespace spr;

const BYTE ESC_READER_SETMODE = 0x01;
const BYTE ESC_READER_GETMODE = 0x02;
const BYTE ESC_CONTACT_GET_SET_POWERUPSEQUENCE = 0x04;
const BYTE ESC_CONTACT_EMV_LOOPBACK = 0x05;
const BYTE ESC_CONTACT_EMV_SINGLEMODE = 0x06;
const BYTE ESC_CONTACT_APDU_TRANSFER = 0x08;
const BYTE ESC_CONTACT_CONTROL_PPS = 0x0f;
const BYTE ESC_CONTACT_EXCHANGE_RAW = 0x10;
const BYTE ESC_READER_GETIFDTYPE = 0x12;

const BYTE ESC_READER_LED_CONTROL = 0x19;
const BYTE LED_RED = 0x00;
const BYTE LED_GREEN = 0x01;

const BYTE LED_OFF = 0x00;
const BYTE LED_ON = 0x01;

const BYTE ESC_BUZZER_CONTROL = 0x51;
const BYTE BUZZER_ENABLE = 0x01;
const BYTE BUZZER_DISABLE = 0x00;

const BYTE ESC_READER_LED_CONTROL_BY_FW = 0xB2;
const BYTE LED_FW_CONTROL_ENABLE = 0x00;
const BYTE LED_FW_CONTROL_DISABLE = 0x01;
const BYTE LED_FW_CONTROL_GET_STATE = 0xff;

const BYTE ESC_READER_GETINFO_EXTENDED = 0x1e;
const BYTE ESC_CONTACT_GET_SET_CLK_FREQUENCY = 0x1f;
const BYTE ESC_CONTACT_CONTROL_ATR_VALIDATION = 0x88;

const BYTE ESC_READER_RD_WR_USER_AREA = 0xf0;
const BYTE USER_AREA_READ = 0x01;
const BYTE USER_AREA_WRITE = 0x02;

bool
scardctl(SCARDHANDLE hCard, const Buffer &in, Buffer &out)
{
    DWORD dwRespLen = 0;
    out.resize(256);
    LONG ret = SCardControl(
      hCard, IOCTL_CCID_ESCAPE, in.data(), in.size(), out.data(), out.size(), &dwRespLen);
    out.resize(dwRespLen);
    return ret == SCARD_S_SUCCESS;
}

bool
scardctl(SCARDHANDLE hCard, const Buffer &in)
{
    LONG ret = SCardControl(hCard, IOCTL_CCID_ESCAPE, in.data(), in.size(), NULL, 0, NULL);
    return ret == SCARD_S_SUCCESS;
}

bool
nvread(SCARDHANDLE hCard, Buffer &buf)
{
    buf.resize(USER_AREA_SIZE);
    DWORD dwRespLen = 0;
    return scardctl(hCard, {ESC_READER_RD_WR_USER_AREA, USER_AREA_READ}, buf) &&
           dwRespLen == USER_AREA_SIZE;
}

bool
nvwrite(SCARDHANDLE hCard, Buffer &data)
{
    Buffer       buf;
    const Buffer beg = {ESC_READER_RD_WR_USER_AREA, USER_AREA_WRITE};
    buf = beg;
    std::copy(std::begin(data), std::end(data), buf.end());
    buf.resize(USER_AREA_SIZE);
    return scardctl(hCard, buf);
}

bool
buzzer_enable(SCARDHANDLE hCard)
{
    return scardctl(hCard, {ESC_BUZZER_CONTROL, BUZZER_ENABLE});
}

bool
buzzer_disable(SCARDHANDLE hCard)
{
    return scardctl(hCard, {ESC_BUZZER_CONTROL, BUZZER_DISABLE});
}

bool
led_fw_ctrl_enable(SCARDHANDLE hCard)
{
    return scardctl(hCard, {ESC_READER_LED_CONTROL_BY_FW, LED_FW_CONTROL_ENABLE});
}

bool
led_fw_ctrl_disable(SCARDHANDLE hCard)
{
    return scardctl(hCard, {ESC_READER_LED_CONTROL_BY_FW, LED_FW_CONTROL_DISABLE});
}

bool
led_ctrl(SCARDHANDLE hCard, LED led, bool state)
{
    return scardctl(hCard, {ESC_READER_LED_CONTROL, led, state == true ? LED_ON : LED_OFF});
}

StringList
list_readers()
{
    SCARDCONTEXT hContext;
    LONG         ret;
    DWORD        pcchReaders = SCARD_AUTOALLOCATE;
    LPSTR        readers = NULL;
    StringList   result;

    ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
    if (ret != SCARD_S_SUCCESS) {
        throw std::runtime_error("SCardEstablishContext");
    }
    ret = SCardListReaders(hContext, NULL, (LPSTR) &readers, &pcchReaders);
    if (ret != SCARD_S_SUCCESS) {
        throw std::runtime_error("SCardListReaders");
    }
    LPSTR reader = readers;
    while (*reader) {
        result.push_back(reader);
        reader += strlen(reader) + 1;
    }
    return result;
}

} // namespace

namespace spr {

StringList
Pinpad::readers()
{
    return ::list_readers();
}

std::string
Pinpad::pick_reader()
{
    const char *reader = getenv("SPR_READER");
    if (reader) {
	    return reader;
    }
    StringList readers(Pinpad::readers());
    if (readers.empty()) {
        throw std::runtime_error("No readers");
    }
    return readers[0];
}

Pinpad::Pinpad(const std::string &reader)
{
    SCARDCONTEXT hContext;
    DWORD        dwProtocol;
    LONG         ret;

    ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
    if (ret != SCARD_S_SUCCESS) {
        throw std::runtime_error("SCardEstablishContext");
    }
    ret = SCardConnect(hContext, reader.c_str(), SCARD_SHARE_DIRECT, 0, &m_card, &dwProtocol);
    if (ret != SCARD_S_SUCCESS) {
        throw std::runtime_error("SCardConnect");
    }
}

Pinpad::~Pinpad()
{
    SCardDisconnect(m_card, SCARD_LEAVE_CARD);
    SCardReleaseContext(m_ctx);
}

bool
Pinpad::buzzer_enable()
{
    return ::buzzer_enable(m_card);
}

bool
Pinpad::buzzer_disable()
{
    return ::buzzer_disable(m_card);
}

bool
Pinpad::led_fw_ctrl_enable()
{
    return ::led_fw_ctrl_enable(m_card);
}

bool
Pinpad::led_fw_ctrl_disable()
{
    return ::led_fw_ctrl_disable(m_card);
}

bool
Pinpad::led_ctrl(LED led, bool state)
{
    return ::led_ctrl(m_card, led, state);
}

bool
Pinpad::nvread(Buffer &buf)
{
    return ::nvread(m_card, buf);
}

bool
Pinpad::nvwrite(Buffer &buf)
{
    return ::nvwrite(m_card, buf);
}

} // namespace spr

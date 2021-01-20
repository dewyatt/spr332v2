/*
https://files.identiv.com/products/smart-card-readers/contact/spr332/SPR332v2_Product_Manual.pdf
https://tech.springcard.com/2013/scardcontrol-linux-and-java/
https://support.identiv.com/spr332/
*/

#include <string>

extern "C" {
#ifdef __APPLE__
#include <pcsc/winscard.h>
#include <pcsc/wintypes.h>
#else
#include <reader.h>
#include <winscard.h>
#endif
}

namespace spr {

const size_t USER_AREA_SIZE = 249;

typedef std::vector<uint8_t>     Buffer;
typedef std::vector<std::string> StringList;

enum LED : BYTE {
    RED = 0x00,
    GREEN = 0x01,
};

class Pinpad {
  public:
    static StringList  readers();
    static std::string pick_reader();

    Pinpad(const std::string &reader);
    ~Pinpad();

    bool buzzer_enable();
    bool buzzer_disable();

    bool led_fw_ctrl_enable();
    bool led_fw_ctrl_disable();

    bool led_ctrl(LED led, bool state);

    bool nvread(Buffer &buf);
    bool nvwrite(Buffer &buf);

  private:
    SCARDHANDLE  m_card;
    SCARDCONTEXT m_ctx;
};

} // namespace spr

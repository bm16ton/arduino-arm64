#ifndef APOLLO3_CORDIO_HCI_TRANSPORT_DRIVER_H_
#define APOLLO3_CORDIO_HCI_TRANSPORT_DRIVER_H_

#include "CordioHCITransportDriver.h"

#define AP3_STUPID_DEF_OF_BLECIRQ_BIT 0x00000080 // AM_BLEIF_INT_BLECIRQ

namespace ble
{
    namespace vendor
    {
        namespace ambiq_micro
        {

            class AP3CordioHCITransportDriver : public cordio::CordioHCITransportDriver
            {
            public:
                //AP3CordioHCITransportDriver(/* specific constructor arguments*/);

                virtual ~AP3CordioHCITransportDriver();

                virtual void initialize();

                virtual void terminate();

                virtual uint16_t write(uint8_t packet_type, uint16_t len, uint8_t *data);

                void *handle;

            private:
                // private driver declarations
            };

        } // namespace ambiq_micro
    }     // namespace vendor
} // namespace ble

extern "C" void CordioHCITransportDriver_on_data_received(uint8_t *data, uint16_t len);

#endif /* APOLLO3_CORDIO_HCI_TRANSPORT_DRIVER_H_ */

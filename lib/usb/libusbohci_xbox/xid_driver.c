//Copyright 2020, Ryan Wendland
//SPDX-License-Identifier: MIT

/*
 * XID input driver
 * This driver is applied with the following Original Xbox Input Devices:
 * - Original Xbox gamecontrollers (S, Duke, Wheels, Arcade Sticks etc)
 * - Xbox DVD Movie Playback IR Dongle
 * - Steel Battalion Controller
 * This code should be read in conjuntion with https://xboxdevwiki.net/index.php?title=Xbox_Input_Devices
 */

#include <stdio.h>
#include <string.h>
#include "usbh_lib.h"
#include "xid_driver.h"

//#define ENABLE_USBH_XID_DEBUG
#ifdef ENABLE_USBH_XID_DEBUG
#include <hal/debug.h>
#define USBH_XID_DEBUG debugPrint
#else
#define USBH_XID_DEBUG(...)
#endif

static xid_dev_t xid_devices[CONFIG_XID_MAX_DEV];
static xid_dev_t *pxid_list = NULL;
static XID_CONN_FUNC *xid_conn_func = NULL, *xid_disconn_func = NULL;

static xid_dev_t *alloc_xid_device(void) {
    xid_dev_t *new_xid = NULL;
    for (int i = 0; i < CONFIG_XID_MAX_DEV; i++)
    {
        if (xid_devices[i].iface == NULL)
        {
            new_xid = &xid_devices[i];
            break;
        }
    }

    if (new_xid == NULL)
    {
        return NULL;
    }

    memset(new_xid, 0, sizeof(xid_dev_t));
    new_xid->uid = get_ticks();

    //Chain the new XID to the end of the list.
    if (pxid_list == NULL)
    {
        pxid_list = new_xid;
    }
    else
    {
        xid_dev_t *x;
        for (x = pxid_list; x->next != NULL; x = x->next)
            ;
        x->next = new_xid;
    }

    return new_xid;
}

static void free_xid_device(xid_dev_t *xid_dev) {
    xid_dev_t *head = pxid_list;

    //If the xid is at the head of the list, remove now.
    if (xid_dev == head)
    {
        pxid_list = head->next;
        head = NULL;
    }

    //Find the device head in the linked list
    while (head != NULL && head->next != xid_dev)
    {
        head = head->next;
    }

    //Remove it from the linked list
    if (head != NULL)
    {
        xid_dev_t *new_tail = xid_dev->next;
        head->next = new_tail;
    }

    //Mark it as free
    memset(xid_dev, 0, sizeof(xid_dev_t));
}

static int xid_probe(IFACE_T *iface) {
    UDEV_T *udev = iface->udev;
    xid_dev_t *xid;
    DESC_IF_T *ifd = iface->aif->ifd;

    if (ifd->bInterfaceClass != XID_INTERFACE_CLASS || ifd->bInterfaceSubClass != XID_INTERFACE_SUBCLASS)
    {
        return USBH_ERR_NOT_MATCHED;
    }

    xid = alloc_xid_device();

    if (xid == NULL)
    {
        return USBH_ERR_MEMORY_OUT;
    }

    //Device is an OG Xbox peripheral.
    //Get the XID descriptor to find out what type:
    xid_descriptor *xid_desc = (xid_descriptor *)usbh_alloc_mem(sizeof(xid_descriptor));
    uint32_t xfer_len;
    int32_t ret = usbh_ctrl_xfer(iface->udev,
                                 0xC1,                   //bmRequestType
                                 0x06,                   //bRequest
                                 0x4200,                 //wValue
                                 iface->if_num,          //wIndex
                                 sizeof(xid_descriptor), //wLength
                                 (uint8_t *)xid_desc, &xfer_len, 100);

    //Populate the xid device struct.
    if (ret != USBH_OK)
    {
        //Controller didn't respond to control transfer. Likely some unusual 3rd party device.
        //Exit for now.
        USBH_XID_DEBUG("Error: XID did not have xid descriptor.\n");
        usbh_free_mem(xid_desc, sizeof(xid_descriptor));
        free_xid_device(xid);
        return USBH_ERR_NOT_MATCHED;
    }

    memcpy(&xid->xid_desc, xid_desc, sizeof(xid_descriptor));

    xid->iface = iface;
    xid->idVendor = udev->descriptor.idVendor;
    xid->idProduct = udev->descriptor.idProduct;
    xid->next = NULL;
    xid->user_data = NULL;
    iface->context = (void *)xid;

    usbh_free_mem(xid_desc, sizeof(xid_descriptor));

#ifdef ENABLE_USBH_XID_DEBUG
    for (int i = 0; i < sizeof(xid_descriptor); i++)
    {
        USBH_XID_DEBUG("%02x ", ((uint8_t*)&xid->xid_desc)[i]);
    }
    USBH_XID_DEBUG("\n");
#endif

    USBH_XID_DEBUG("OG Xbox peripheral type %02x, sub type: %02x connected\n", xid->xid_desc.bType,
               xid->xid_desc.bSubType);

    if (xid_conn_func)
    {
        xid_conn_func(xid, 0);
    }

    return USBH_OK;
}

static void xid_disconnect(IFACE_T *iface) {
    xid_dev_t *xid_dev;
    UTR_T *utr;
    xid_dev = (xid_dev_t *)(iface->context);

    //Stop any running pipe transfers
    for (int i = 0; i < iface->aif->ifd->bNumEndpoints; i++)
    {
        iface->udev->hc_driver->quit_xfer(NULL, &(iface->aif->ep[i]));
    }

    //Free any running UTRs
    for (int i = 0; i < XID_MAX_TRANSFER_QUEUE; i++)
    {
        utr = xid_dev->utr_list[i];
        if (utr != NULL)
        {
            usbh_quit_utr(utr);
            usbh_free_mem(utr->buff, utr->ep->wMaxPacketSize);
            free_utr(utr);
        }
    }

    USBH_XID_DEBUG("xid disconnected - device (vid=0x%x, pid=0x%x), interface %d.\n",
               xid_dev->idVendor, xid_dev->idProduct, iface->if_num);

    if (xid_disconn_func)
    {
        xid_disconn_func(xid_dev, 0);
    }

    free_xid_device(xid_dev);
}

UDEV_DRV_T xid_driver =
{
    xid_probe,
    xid_disconnect,
    NULL, //suspend
    NULL, //resume
};

/**
 * @brief Adds a callback function when an XID interface is connected or removed. Pass NULL to remove the callback. 
 * 
 * @param conn_func  The user's connection callback function.
 * @param disconn_func The user's disconnection callback function.
 */
void usbh_install_xid_conn_callback(XID_CONN_FUNC *conn_func, XID_CONN_FUNC *disconn_func) {
    xid_conn_func = conn_func;
    xid_disconn_func = disconn_func;
}

/**
 * @brief Initialises the XID driver with the USB backend.
 * 
 */
void usbh_xid_init(void) {
    usbh_register_driver(&xid_driver);
}

/**
 * Returns a pointer to the first connected XID device. This is a linked list
 * to all connected XID devices.
 * @return A pointer to a xid_dev_t device.
 */
xid_dev_t *usbh_xid_get_device_list(void) {
    return pxid_list;
}

static int32_t queue_int_xfer(xid_dev_t *xid_dev, uint8_t dir, uint8_t ep_addr, uint8_t *buff, uint32_t len, void *callback) {
    IFACE_T *iface = (IFACE_T *)xid_dev->iface;
    UTR_T *utr = NULL;
    int ret, i, free_slot;

    if (iface == NULL || iface->udev == NULL)
    {
        return USBH_ERR_DISCONNECTED;
    }

    EP_INFO_T *ep = usbh_iface_find_ep(iface, ep_addr, dir | EP_ATTR_TT_INT);
    if (ep == NULL)
    {
        return USBH_ERR_EP_NOT_FOUND;
    }

    //Clean up finished UTRs in the queue
    for (i = 0; i < XID_MAX_TRANSFER_QUEUE; i++)
    {
        utr = xid_dev->utr_list[i];
        if (utr != NULL && utr->ep != NULL && utr->ep->bEndpointAddress == ep->bEndpointAddress)
        {
            //Don't queue multiple reads. User is calling faster than controller can update so has no benefit.
            if (dir == EP_ADDR_DIR_IN && utr->bIsTransferDone == 0)
            {
                return HID_RET_XFER_IS_RUNNING;
            }

            if (utr->bIsTransferDone)
            {
                usbh_free_mem(utr->buff, utr->ep->wMaxPacketSize);
                free_utr(utr);
                xid_dev->utr_list[i] = NULL;
            }
        }
    }

    //Find a free slot in the queue
    free_slot = USBH_ERR_MEMORY_OUT;
    for (i = 0; i < XID_MAX_TRANSFER_QUEUE; i++)
    {
        if (xid_dev->utr_list[i] == NULL)
        {
            free_slot = i;
            break;
        }
    }

    if (free_slot < 0)
    {
        return free_slot;
    }

    utr = alloc_utr(iface->udev);
    if (utr == NULL)
    {
        return USBH_ERR_MEMORY_OUT;
    }

    //Populate the new utr
    utr->context = xid_dev;
    utr->ep = ep;
    utr->data_len = (dir == EP_ADDR_DIR_OUT && len < ep->wMaxPacketSize) ? len : ep->wMaxPacketSize;
    utr->xfer_len = 0;
    utr->func = callback;
    utr->buff = usbh_alloc_mem(ep->wMaxPacketSize);
    if (utr->buff == NULL)
    {
        free_utr(utr);
        return USBH_ERR_MEMORY_OUT;
    }

    if (dir == EP_ADDR_DIR_OUT)
    {
        memcpy(utr->buff, buff, utr->data_len);
    }

    ret = usbh_int_xfer(utr);
    if (ret != USBH_OK)
    {
        usbh_free_mem(utr->buff, ep->wMaxPacketSize);
        free_utr(utr);
        return ret;
    }

    //Register a queued UTR for this device.
    xid_dev->utr_list[free_slot] = utr;

    return USBH_OK;
}

/**
 * @brief Reads an interrupt in endpoint. This is a nonblocking function. The received data can be read from the callback function.
 * 
 * @param xid_dev Pointer to the XID device.
 * @param ep_addr The endpoint address to read. If ep_addr equal zero, the first interrupt in endpoint found will be used.
 * @param rx_complete_callback The user's receive complete callback function with the form `void my_callback(UTR_T *utr)`.
 * The received data is read from the user implemented callback function.
 * @return USBH_OK or the error.
 */
int32_t usbh_xid_read(xid_dev_t *xid_dev, uint8_t ep_addr, void *rx_complete_callback) {
    return queue_int_xfer(xid_dev, EP_ADDR_DIR_IN, ep_addr, NULL, 0, rx_complete_callback);
}

/**
 * @brief Queues a write to an interrupt out endpoint. This is a nonblocking function. The total amount of transfers that can be queued is
 * set by XID_MAX_TRANSFER_QUEUE.
 * 
 * @param xid_dev Pointer to the XID device.
 * @param ep_addr The endpoint address to write. If ep_addr equal zero, the first interrupt out endpoint found will be used.
 * @param txbuff The user transmit buffer
 * @param len The number of bytes to transfer
 * @param tx_complete_callback Optional transfer complete callback function.
 * @return USBH_OK or the error.
 */
int32_t usbh_xid_write(xid_dev_t *xid_dev, uint8_t ep_addr, uint8_t *txbuff, uint32_t len, void *tx_complete_callback) {
    return queue_int_xfer(xid_dev, EP_ADDR_DIR_OUT, ep_addr, txbuff, len, tx_complete_callback);
}

/**
 * @brief Send a rumble command to an xid game controller.
 * 
 * @param xid_dev Pointer to the XID device.
 * @param l_value Value of the low frequency rumble. 0 to 0xFFFF.
 * @param h_value Value of the high frequency rumble. 0 to 0xFFFF.
 * @return USBH_OK or the error. 
 */
int32_t usbh_xid_rumble(xid_dev_t *xid_dev, uint16_t l_value, uint16_t h_value){
    if (xid_dev->xid_desc.bType != XID_TYPE_GAMECONTROLLER)
    {
        return USBH_ERR_NOT_SUPPORTED;
    }

    xid_gamepad_out command =
    {
        .startByte = 0,
        .bLength = sizeof(xid_gamepad_out),
        .lValue = l_value,
        .hValue = h_value,
    };
    return usbh_xid_write(xid_dev, 0, (uint8_t *)&command, sizeof(xid_gamepad_out), NULL);
}

/**
 * @brief Returns the xid_type.
 * 
 * @param xid_dev Pointer to the XID device.
 * @return xid_type. Refer to xid_type enum.
 */
xid_type usbh_xid_get_type(xid_dev_t *xid_dev)
{
    if (xid_dev->xid_desc.bLength == 0)
    {
        return XID_UNKNOWN;
    }

    if (xid_dev->xid_desc.bType == 0x01)
    {
        switch (xid_dev->xid_desc.bSubType)
        {
            case 0x01: return GAMECONTROLLER_DUKE; break;
            case 0x02: return GAMECONTROLLER_S; break;
            case 0x10: return GAMECONTROLLER_WHEEL; break;
            case 0x20: return GAMECONTROLLER_ARCADESTICK; break;
            default:
                USBH_XID_DEBUG("Unknown XID SubType\n");
                return XID_UNKNOWN;
                break;
        }
    }

    if (xid_dev->xid_desc.bType == 0x03)
    {
        switch (xid_dev->xid_desc.bSubType)
        {
            case 0x00: return XREMOTE; break;
            default:
                USBH_XID_DEBUG("Unknown XID SubType\n");
                return XID_UNKNOWN;
        }
    }

    if (xid_dev->xid_desc.bType == 0x80)
    {
        switch (xid_dev->xid_desc.bSubType)
        {
            case 0x01: return STEEL_BATTALION; break;
            default:
                USBH_XID_DEBUG("Unknown XID SubType\n");
                return XID_UNKNOWN;
        }
    }

    USBH_XID_DEBUG("Unknown XID Type\n");
    return XID_UNKNOWN;
}

/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/* Assemble the msg here: http://rednaxela.net/pdu.php */

#include <zephyr.h>
#include <stdio.h>
#include <string.h>
#include <modem/at_cmd.h>
#include <dk_buttons_and_leds.h>

#define USER_BUTTON             DK_BTN1_MSK

K_SEM_DEFINE(send_sms,0,1);

static int encode_msg_in_pdu(uint8_t * buf, uint32_t bufsize)
{
    int len = 0;

    len += snprintf(buf + len, bufsize - len,"at+cmgs=");
    len += snprintf(buf + len, bufsize - len,"44");
    buf[len++] = 0x0D; /**< \r */
    len += snprintf(buf + len, bufsize - len,"insert-msg-in-pdu-format");
    buf[len++] = 0x1A;  /**< ctrl+z */

    return len;
}

static void button_handler(uint32_t button_state, uint32_t has_changed)
{
    printk("button pressed!\n");

	if (has_changed & USER_BUTTON && button_state & USER_BUTTON) {
        k_sem_give(&send_sms);
    }
}

static char resp_buf[1024] = {0};

void main(void)
{
    int err = -1;

    printk("SMS sample with the NRF9160!\n");

    dk_buttons_init(button_handler);

    err = at_cmd_init();    
    printk("at_cmd_init(): %d\n",err);
    if(err)
        goto app_ending;

    enum at_cmd_state modem_st;

    const char * cmd_cnmi = "at+cnmi=3,2,0,1";
    err = at_cmd_write(cmd_cnmi,resp_buf,sizeof(resp_buf),&modem_st);
    printk("Response to %s: Result: %d\n",cmd_cnmi, err);
    if(err < 0)
        goto app_ending;

    const char * cmd_subscribe_nwreg_changes = "at+cereg=5";
    err = at_cmd_write(cmd_subscribe_nwreg_changes,resp_buf,sizeof(resp_buf),&modem_st);
    printk("Response to %s: Result: %d\n",cmd_subscribe_nwreg_changes, err);
    if(err < 0)
        goto app_ending;

    const char * cmd_enable_modem = "at+cfun=1";
    err = at_cmd_write(cmd_enable_modem,resp_buf,sizeof(resp_buf),&modem_st);
    printk("Response to %s: Result: %d\n",cmd_enable_modem, err);
    if(err < 0)
        goto app_ending;

    printk("Press button 1 to send an sms to the pre-defined number\n");
    for(;;){
        err = k_sem_take(&send_sms,K_FOREVER);
        if(err < 0)
            continue;
        printk("Sending sms!\n");

        char msgbuf[2048] = {0};
        err = encode_msg_in_pdu(msgbuf, sizeof(msgbuf));
        if(err <= 0){
            printk("Failed encoding msg - skipping send\n");
            continue;
        }

        err = at_cmd_write(msgbuf,resp_buf,sizeof(resp_buf),&modem_st);
        printk("SMS Send result: %d\n",modem_st);

    }

app_ending:
    printk("Ending of sample\n");

}

/*
 *  HID driver for Zuoya MMR6 mouse.
 *
 *  Copyright (c) 1999 Andreas Gal
 *  Copyright (c) 2000-2005 Vojtech Pavlik <vojtech@suse.cz>
 *  Copyright (c) 2005 Michael Haboustak <mike-@cinci.rr.com> for Concept2, Inc
 *  Copyright (c) 2006-2007 Jiri Kosina
 *  Copyright (c) 2008 Jiri Slaby
 *  Copyright (c) 2018 Vladislav Dmitrievich Turbanov <vladislav@turbanov.ru>
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/device.h>
#include <linux/input.h>
#include <linux/hid.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "hid-ids.h"

struct mmr6_sc
{
	u8 button;
};

static int mmr6_raw_event(struct hid_device *hdev, struct hid_report *report,
						  u8 *data, int size)
{
	struct mmr6_sc *mmr6;

	if (size < 2)
	{
		hid_err(hdev, "invalid raw data size\n");
		return -EINVAL;
	}

	mmr6 = hid_get_drvdata(hdev);
	mmr6->button = data[1];

	return 0;
}

static int mmr6_event(struct hid_device *hdev, struct hid_field *field,
					  struct hid_usage *usage, __s32 value)
{
	struct mmr6_sc *mmr6;

	//printk("mmr6_event: %X", value);

	mmr6 = hid_get_drvdata(hdev);

	if (!(hdev->claimed & HID_CLAIMED_INPUT) || !field->hidinput ||
		!usage->type)
		return 0;

	//printk("mdmr6->button: %X", mmr6->button);

	if (usage->type == EV_KEY)
	{
		if (usage->code == BTN_LEFT)
		{
			//printk("mdmr6 left button");
			if (mmr6->button == 3)
			{
				//printk("mdmr6 cancel event");
				return 1;
			}
			return 0;
		}
		else if (usage->code == BTN_LEFT)
			return 0;
		else if (usage->code == BTN_RIGHT)
			return 0;
		else if (usage->code == BTN_MIDDLE)
			return 0;
		else if (usage->code == BTN_SIDE)
			return 0;
		else if (usage->code == BTN_EXTRA)
			return 0;
		return 1;
	}
	else if (usage->type == EV_REL)
		return 0;

	return 1;
}

static int mmr6_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	struct mmr6_sc *mmr6;
	int ret;

	mmr6 = devm_kzalloc(&hdev->dev, sizeof(*mmr6), GFP_KERNEL);
	if (mmr6 == NULL)
	{
		hid_err(hdev, "can't alloc device descriptor\n");
		return -ENOMEM;
	}

	hid_set_drvdata(hdev, mmr6);

	ret = hid_parse(hdev);
	if (ret)
	{
		hid_err(hdev, "parse failed\n");
		return ret;
	}

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (ret)
	{
		hid_err(hdev, "hw start failed\n");
		return ret;
	}

	return 0;
}

static const struct hid_device_id mmr6_devices[] = {
	{HID_USB_DEVICE(USB_VENDOR_ID_ZUOYA, USB_DEVICE_ID_ZUOYA_MMR6)},
	{}};

MODULE_DEVICE_TABLE(hid, mmr6_devices);

static struct hid_driver mmr6_driver = {
	.name = "hid-zuoya-mmr6",
	.id_table = mmr6_devices,
	.raw_event = mmr6_raw_event,
	.event = mmr6_event,
	.probe = mmr6_probe,
};

module_hid_driver(mmr6_driver);

MODULE_LICENSE("GPL");
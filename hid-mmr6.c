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

#define MMR6_2WHEEL_MOUSE_HACK_7 0x01
#define MMR6_2WHEEL_MOUSE_HACK_B8 0x02

struct mmr6_sc
{
	unsigned long quirks;
	unsigned int hw_wheel;
	__s32 delayed_value;
	u8 raw_data[7];
};

static int mmr6_input_mapped(struct hid_device *hdev, struct hid_input *hi,
							 struct hid_field *field, struct hid_usage *usage,
							 unsigned long **bit, int *max)
{
	struct mmr6_sc *mmr6 = hid_get_drvdata(hdev);

	if (usage->type == EV_REL && usage->code == REL_WHEEL)
		set_bit(REL_HWHEEL, *bit);

	if ((mmr6->quirks & MMR6_2WHEEL_MOUSE_HACK_7) && usage->hid == 0x00090007)
		return -1;

	return 0;
}

static int mmr6_raw_event(struct hid_device *hdev, struct hid_report *report,
						  u8 *data, int size)
{
	printk("MMR6 RAW EVENT");
	int i;

	for (i = 0; i < size; i++)
	{
		printk("%02X", data[i]);
	}

	struct mmr6_sc *mmr6 = hid_get_drvdata(hdev);

	memcpy(mmr6->raw_data, data, size);

	return 0;
}

static int mmr6_event(struct hid_device *hdev, struct hid_field *field,
					  struct hid_usage *usage, __s32 value)
{

	struct mmr6_sc *mmr6 = hid_get_drvdata(hdev);
	struct input_dev *input;

	//printk("MMR6 EVENT");

	if (!(hdev->claimed & HID_CLAIMED_INPUT) || !field->hidinput ||
		!usage->type)
		return 0;

	input = field->hidinput->input;

	printk("MMR6 type: %d", usage->type);
	printk("MMR6 code: %04X", usage->code);
	printk("MMR6 value: %04X", value);
	printk("MMR6 raw data:");

	int i = 0;
	for (i = 0; i < 7; i++)
	{
		printk("%04X", mmr6->raw_data[i]);
	}

	if (usage->type == EV_REL)
	{
		return 0;
	}

	if (usage->type == EV_KEY)
	{

		printk("MMR6 Button: %04X", usage->code);
		if (usage->code == BTN_LEFT)
		{
			if (mmr6->raw_data[1] == 0x03)
				return 1;
			printk("MMR LMB value: %04X", value);
			input_event(input, EV_KEY, BTN_LEFT, value);
		}
		else if (usage->code == BTN_MIDDLE)
		{
			printk("MMR MMB value: %04X", value);
			input_event(input, EV_KEY, BTN_MIDDLE, value);
		}
		else if (usage->code == BTN_RIGHT)
		{
			printk("MMR RMB value: %04X", value);
			input_event(input, EV_KEY, BTN_RIGHT, value);
		}
	}

	return 1;

	// if (mmr6->quirks & MMR6_2WHEEL_MOUSE_HACK_B8)
	// {
	// 	if (usage->type == EV_REL && usage->code == REL_WHEEL)
	// 	{
	// 		mmr6->delayed_value = value;
	// 		return 1;
	// 	}

	// 	if (usage->hid == 0x000100b8)
	// 	{
	// 		input_event(input, EV_REL, value ? REL_HWHEEL : REL_WHEEL, mmr6->delayed_value);
	// 		return 1;
	// 	}
	// }

	// if ((mmr6->quirks & MMR6_2WHEEL_MOUSE_HACK_7) && usage->hid == 0x00090007)
	// {
	// 	mmr6->hw_wheel = !!value;
	// 	return 1;
	// }

	// if (usage->code == REL_WHEEL && mmr6->hw_wheel)
	// {
	// 	input_event(input, usage->type, REL_HWHEEL, value);
	// 	return 1;
	// }

	return 0;
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

	mmr6->quirks = id->driver_data;

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
	{HID_USB_DEVICE(USB_VENDOR_ID_ZUOYA, USB_DEVICE_ID_ZUOYA_MMR6),
	 .driver_data = MMR6_2WHEEL_MOUSE_HACK_7},
	{}};
MODULE_DEVICE_TABLE(hid, mmr6_devices);

static struct hid_driver mmr6_driver = {
	.name = "hid-zuoya-mmr6",
	.id_table = mmr6_devices,
	.input_mapped = mmr6_input_mapped,
	.raw_event = mmr6_raw_event,
	.event = mmr6_event,
	.probe = mmr6_probe,
};
module_hid_driver(mmr6_driver);

MODULE_LICENSE("GPL");
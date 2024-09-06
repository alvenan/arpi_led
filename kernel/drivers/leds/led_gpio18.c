#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>

struct drvled_data_st {
	struct gpio_desc *desc;
	struct led_classdev led_cdev;
};

static void drvled_change_state(struct led_classdev *cdev,
				enum led_brightness brightness) {
	static struct drvled_data_st *drvled_data;

	drvled_data = container_of(cdev, struct drvled_data_st, led_cdev);

	if (brightness)
		gpiod_set_value(drvled_data->desc, 1);
	else
		gpiod_set_value(drvled_data->desc, 0);
}

static int drvled_probe(struct platform_device *pdev) {
	struct device_node *np = pdev->dev.of_node;
	struct device_node *child = NULL;
	static struct drvled_data_st *drvled_data;
	int result, num_leds;

	num_leds = of_get_available_child_count(np);
	if (!num_leds) {
		dev_err(&pdev->dev, "No leds available!\n");
		return -ENODEV;
	}

	drvled_data = devm_kzalloc(&pdev->dev, sizeof(*drvled_data) * num_leds,
				   GFP_KERNEL);
	if (!drvled_data)
		return -ENOMEM;

	platform_set_drvdata(pdev, drvled_data);

	for_each_available_child_of_node(np, child) {

		drvled_data->desc = devm_gpiod_get_from_of_node(&pdev->dev, child, "gpios", 0, 0, pdev->name);
		if (IS_ERR(drvled_data->desc)) {
			dev_err(&pdev->dev, "Error requesting GPIO\n");
			return PTR_ERR(drvled_data->desc);
		}

		drvled_data->led_cdev.name = of_get_property(child, "label", NULL);
		drvled_data->led_cdev.default_trigger = of_get_property(child, "default-trigger", NULL);
		drvled_data->led_cdev.brightness_set = drvled_change_state;

		result = devm_led_classdev_register(&pdev->dev, &drvled_data->led_cdev);
		if (result) {
			dev_err(&pdev->dev, "Error registering led\n");
			return result;
		}

		gpiod_direction_output(drvled_data->desc, 0);

		drvled_data++;
	}

	dev_info(&pdev->dev, "initialized.\n");

	return 0;
}

static int drvled_remove(struct platform_device *pdev) {
	dev_info(&pdev->dev, "exiting.\n");

	return 0;
}

static const struct of_device_id of_drvled_match[] = {
	{ .compatible = "arpi4,leds" },
	{},
};

static struct platform_driver drvled_driver = {
	.driver = {
		.name	= "gpio18_led",
		.owner	= THIS_MODULE,
		.of_match_table = of_drvled_match,
	},
	.probe		= drvled_probe,
	.remove		= drvled_remove,
};

module_platform_driver(drvled_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alison Venancio <alison.venancio@gmail.com>");

// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2015-2018, The Linux Foundation. All rights reserved.
 * Copyright (c) 2023, Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/qcom_scm.h>
#include <soc/qcom/msm_tz_smmu.h>

static const char * const device_id_mappings[] = {
	[TZ_DEVICE_VIDEO] = "VIDEO",
	[TZ_DEVICE_MDSS] = "MDSS",
	[TZ_DEVICE_LPASS] = "LPASS",
	[TZ_DEVICE_MDSS_BOOT] = "MDSS_BOOT",
	[TZ_DEVICE_USB1_HS] = "USB1_HS",
	[TZ_DEVICE_OCMEM] = "OCMEM",
	[TZ_DEVICE_LPASS_CORE] = "LPASS_CORE",
	[TZ_DEVICE_VPU] = "VPU",
	[TZ_DEVICE_COPSS_SMMU] = "COPSS_SMMU",
	[TZ_DEVICE_USB3_0] = "USB3_0",
	[TZ_DEVICE_USB3_1] = "USB3_1",
	[TZ_DEVICE_PCIE_0] = "PCIE_0",
	[TZ_DEVICE_PCIE_1] = "PCIE_1",
	[TZ_DEVICE_BCSS] = "BCSS",
	[TZ_DEVICE_VCAP] = "VCAP",
	[TZ_DEVICE_PCIE20] = "PCIE20",
	[TZ_DEVICE_IPA] = "IPA",
	[TZ_DEVICE_APPS] = "APPS",
	[TZ_DEVICE_GPU] = "GPU",
	[TZ_DEVICE_UFS] = "UFS",
	[TZ_DEVICE_ICE] = "ICE",
	[TZ_DEVICE_ROT] = "ROT",
	[TZ_DEVICE_VFE] = "VFE",
	[TZ_DEVICE_ANOC0] = "ANOC0",
	[TZ_DEVICE_ANOC1] = "ANOC1",
	[TZ_DEVICE_ANOC2] = "ANOC2",
	[TZ_DEVICE_CPP] = "CPP",
	[TZ_DEVICE_JPEG] = "JPEG",
};

#define MAX_DEVICE_ID_NAME_LEN 20

#define TZ_SMMU_PREPARE_ATOS_ID 0x21
#define TZ_SMMU_ATOS_START 1
#define TZ_SMMU_ATOS_END 0

#define SMMU_CHANGE_PAGETABLE_FORMAT    0X01

enum tz_smmu_device_id msm_dev_to_device_id(struct device *dev)
{
	const char *device_id;
	enum tz_smmu_device_id iter;

	if (of_property_read_string(dev->of_node, "qcom,tz-device-id",
					    &device_id)) {
		dev_err(dev, "no qcom,device-id property\n");
		return TZ_DEVICE_MAX;
	}

	for (iter = TZ_DEVICE_START; iter < TZ_DEVICE_MAX; iter++)
		if (!strcmp(device_id_mappings[iter], device_id))
			return iter;

	return TZ_DEVICE_MAX;
}

int msm_tz_smmu_atos_start(struct device *dev, int cb_num)
{
	enum tz_smmu_device_id devid = msm_dev_to_device_id(dev);

	if (devid == TZ_DEVICE_MAX)
		return -ENODEV;

	return qcom_scm_smmu_prepare_atos_id(devid, cb_num, TZ_SMMU_ATOS_START);
}

int msm_tz_smmu_atos_end(struct device *dev, int cb_num)
{
	enum tz_smmu_device_id devid = msm_dev_to_device_id(dev);

	if (devid == TZ_DEVICE_MAX)
		return -ENODEV;

	return qcom_scm_smmu_prepare_atos_id(devid, cb_num, TZ_SMMU_ATOS_END);
}

int msm_tz_set_cb_format(enum tz_smmu_device_id sec_id, int cbndx)
{
	int ret;

	ret = qcom_scm_smmu_change_pgtbl_format(sec_id, cbndx);

	if (ret) {
		WARN(1, "Format change failed for CB %d with ret %d\n",
		     cbndx, ret);
		return ret;
	}

	return 0;
}

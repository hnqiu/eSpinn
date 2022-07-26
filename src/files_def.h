/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


/* @brief: file definitions
 * include this in implementor files
 */
#pragma once

#include <string>

namespace eSpinn {
    const std::string DIR_ASSET("./asset/");
    const std::string DIR_DATA("./asset/data/");
    const std::string DIR_ARCHIVE("./asset/archive/");

    const std::string FILE_IN   (DIR_DATA + "in");
    const std::string FILE_OUT  (DIR_DATA + "out");
    const std::string FILE_IN0  (DIR_DATA + "in0");
    const std::string FILE_IN1  (DIR_DATA + "in1");
    const std::string FILE_IN2  (DIR_DATA + "in2");
    const std::string FILE_OUT0 (DIR_DATA + "out0");
    const std::string FILE_OUT1 (DIR_DATA + "out1");
    const std::string FILE_DATA_RANGE(DIR_DATA + "data_range");

    const std::string FILE_REF_DATA         (DIR_DATA + "ref_data");
    const std::string FILE_CTRL_OUT         (DIR_DATA + "ctrl_out");
    const std::string FILE_ACT_OUT          (DIR_DATA + "out.act");
    const std::string FILE_VERIFY_DATA      (DIR_DATA + "verify_data");
    const std::string FILE_VERIFY_CTRL_OUT  (DIR_DATA + "verify_ctrl_out");
    const std::string FILE_VERIFY_OUT       (DIR_DATA + "verify_out.act");
    const std::string FILE_FIT              (DIR_DATA + "fit");
    const std::string FILE_WEIGHT           (DIR_DATA + "conn_weight");
    const std::string FILE_GEN_REC          (DIR_DATA + "gen_record");

    const std::string FILE_POP      (DIR_ARCHIVE + "pop");
    const std::string FILE_CHAMP    (DIR_ARCHIVE + "champ_net");
    const std::string FILE_EXT      (".arch");
}

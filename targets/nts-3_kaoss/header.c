#include "dev_id.h"
#include "unit_genericfx.h"

const __unit_header genericfx_unit_header_t unit_header = {
    .common = {
        .header_size = sizeof(genericfx_unit_header_t),
        .target = UNIT_TARGET_PLATFORM | k_unit_module_genericfx,
        .api = UNIT_API_VERSION,
        .dev_id = UNIT_DEV_ID,
        .unit_id = 0x00000001U,
        .version = UNIT_VERSION,
        .name = "Sine",
        .num_params = 3,
        .params = {
            {0, 3, 0, 0, k_unit_param_type_strings, 0, 0, 0, {"WAVE"}},
            {0, 1023, 0, 0, k_unit_param_type_none, 0, 0, 0, {"SHPE"}},
            {0, 1000, 0, 1000, k_unit_param_type_percent, 1, 1, 0, {"MIX"}},
            {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
            {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
            {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
            {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
            {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}}},
    },
    .default_mappings = {
        {k_genericfx_param_assign_none, k_genericfx_curve_linear, k_genericfx_curve_unipolar, 0, 3, 0},
        {k_genericfx_param_assign_none, k_genericfx_curve_linear, k_genericfx_curve_unipolar, 0, 1023, 0},
        {k_genericfx_param_assign_depth, k_genericfx_curve_linear, k_genericfx_curve_unipolar, 0, 1000, 1000},
        {k_genericfx_param_assign_none, k_genericfx_curve_linear, k_genericfx_curve_unipolar, 0, 0, 0},
        {k_genericfx_param_assign_none, k_genericfx_curve_linear, k_genericfx_curve_unipolar, 0, 0, 0},
        {k_genericfx_param_assign_none, k_genericfx_curve_linear, k_genericfx_curve_unipolar, 0, 0, 0},
        {k_genericfx_param_assign_none, k_genericfx_curve_linear, k_genericfx_curve_unipolar, 0, 0, 0},
        {k_genericfx_param_assign_none, k_genericfx_curve_linear, k_genericfx_curve_unipolar, 0, 0, 0},
    },
};

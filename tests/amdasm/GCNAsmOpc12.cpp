/*
 *  CLRadeonExtender - Unofficial OpenCL Radeon Extensions Library
 *  Copyright (C) 2014-2015 Mateusz Szpakowski
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <CLRX/Config.h>
#include "GCNAsmOpc.h"

const GCNAsmOpcodeCase encGCN12OpcodeCases[] =
{
    /* SOPK */
    { "    s_add_u32  flat_scratch_lo, s4, s61", 0x80663d04U, 0, false, true, "" },
    { "    s_add_u32  flat_scratch_hi, s4, s61", 0x80673d04U, 0, false, true, "" },
    { "    s_add_u32  xnack_mask_lo, s4, s61", 0x80683d04U, 0, false, true, "" },
    { "    s_add_u32  xnack_mask_hi, s4, s61", 0x80693d04U, 0, false, true, "" },
    { "    s_add_u32  xnack_mask_hi, 0.15915494, s61", 0x80693df8U, 0, false, true, "" },
    { "    s_add_u32  xnack_mask_hi, 15.915494e-2, s61", 0x80693df8U, 0, false, true, "" },
    // SOP2 instructions
    { "    s_addc_u32  s21, s4, s61", 0x82153d04U, 0, false, true, "" },
    { "    s_and_b32  s21, s4, s61", 0x86153d04U, 0, false, true, "" },
    { "    s_and_b64  s[20:21], s[4:5], s[62:63]", 0x86943e04U, 0, false, true, "" },
    { "    s_or_b32  s21, s4, s61", 0x87153d04U, 0, false, true, "" },
    { "    s_or_b64  s[20:21], s[4:5], s[62:63]", 0x87943e04U, 0, false, true, "" },
    { "    s_xor_b32  s21, s4, s61", 0x88153d04U, 0, false, true, "" },
    { "    s_xor_b64  s[20:21], s[4:5], s[62:63]", 0x88943e04U, 0, false, true, "" },
    { "    s_andn2_b32  s21, s4, s61", 0x89153d04U, 0, false, true, "" },
    { "    s_andn2_b64  s[20:21], s[4:5], s[62:63]", 0x89943e04U, 0, false, true, "" },
    { "    s_orn2_b32  s21, s4, s61", 0x8a153d04U, 0, false, true, "" },
    { "    s_orn2_b64  s[20:21], s[4:5], s[62:63]", 0x8a943e04U, 0, false, true, "" },
    { "    s_nand_b32  s21, s4, s61", 0x8b153d04U, 0, false, true, "" },
    { "    s_nand_b64  s[20:21], s[4:5], s[62:63]", 0x8b943e04U, 0, false, true, "" },
    { "    s_nor_b32  s21, s4, s61", 0x8c153d04U, 0, false, true, "" },
    { "    s_nor_b64  s[20:21], s[4:5], s[62:63]", 0x8c943e04U, 0, false, true, "" },
    { "    s_xnor_b32  s21, s4, s61", 0x8d153d04U, 0, false, true, "" },
    { "    s_xnor_b64  s[20:21], s[4:5], s[62:63]", 0x8d943e04U, 0, false, true, "" },
    { "    s_lshl_b32  s21, s4, s61", 0x8e153d04U, 0, false, true, "" },
    { "    s_lshl_b64  s[20:21], s[4:5], s61", 0x8e943d04U, 0, false, true, "" },
    { "    s_lshr_b32  s21, s4, s61", 0x8f153d04U, 0, false, true, "" },
    { "    s_lshr_b64  s[20:21], s[4:5], s61", 0x8f943d04U, 0, false, true, "" },
    { "    s_ashr_i32  s21, s4, s61", 0x90153d04U, 0, false, true, "" },
    { "    s_ashr_i64  s[20:21], s[4:5], s61", 0x90943d04U, 0, false, true, "" },
    { "    s_bfm_b32  s21, s4, s61", 0x91153d04U, 0, false, true, "" },
    { "    s_bfm_b64  s[20:21], s4, s62", 0x91943e04U, 0, false, true, "" },
    { "    s_mul_i32  s21, s4, s61", 0x92153d04U, 0, false, true, "" },
    { "    s_bfe_u32  s21, s4, s61", 0x92953d04U, 0, false, true, "" },
    { "    s_bfe_i32  s21, s4, s61", 0x93153d04U, 0, false, true, "" },
    { "    s_bfe_u64  s[20:21], s[4:5], s61", 0x93943d04U, 0, false, true, "" },
    { "    s_bfe_i64  s[20:21], s[4:5], s61", 0x94143d04U, 0, false, true, "" },
    { "    s_cbranch_g_fork  s[4:5], s[62:63]", 0x94803e04U, 0, false, true, "" },
    { "    s_absdiff_i32  s21, s4, s61", 0x95153d04U, 0, false, true, "" },
    { "    s_rfe_restore_b64 s[4:5], s61", 0x95803d04U, 0, false, true, "" },
    /* SOP1 encoding */
    { "    s_mov_b32  s86, s20", 0xbed60014U, 0, false, true, "" },
    { "    s_mov_b64  s[86:87], s[20:21]", 0xbed60114U, 0, false, true, "" },
    { "    s_cmov_b32  s86, s20", 0xbed60214U, 0, false, true, "" },
    { "    s_cmov_b64  s[86:87], s[20:21]", 0xbed60314U, 0, false, true, "" },
    { "    s_not_b32  s86, s20", 0xbed60414U, 0, false, true, "" },
    { "    s_not_b64  s[86:87], s[20:21]", 0xbed60514U, 0, false, true, "" },
    { "    s_wqm_b32  s86, s20", 0xbed60614U, 0, false, true, "" },
    { "    s_wqm_b64  s[86:87], s[20:21]", 0xbed60714U, 0, false, true, "" },
    { "    s_brev_b32  s86, s20", 0xbed60814U, 0, false, true, "" },
    { "    s_brev_b64  s[86:87], s[20:21]", 0xbed60914U, 0, false, true, "" },
    { "    s_bcnt0_i32_b32  s86, s20", 0xbed60a14U, 0, false, true, "" },
    { "    s_bcnt0_i32_b64  s86, s[20:21]", 0xbed60b14U, 0, false, true, "" },
    { "    s_bcnt1_i32_b32  s86, s20", 0xbed60c14U, 0, false, true, "" },
    { "    s_bcnt1_i32_b64  s86, s[20:21]", 0xbed60d14U, 0, false, true, "" },
    { "    s_ff0_i32_b32  s86, s20", 0xbed60e14U, 0, false, true, "" },
    { "    s_ff0_i32_b64  s86, s[20:21]", 0xbed60f14U, 0, false, true, "" },
    { "    s_ff1_i32_b32  s86, s20", 0xbed61014U, 0, false, true, "" },
    { "    s_ff1_i32_b64  s86, s[20:21]", 0xbed61114U, 0, false, true, "" },
    { "    s_flbit_i32_b32  s86, s20", 0xbed61214U, 0, false, true, "" },
    { "    s_flbit_i32_b64  s86, s[20:21]", 0xbed61314U, 0, false, true, "" },
    { "    s_flbit_i32  s86, s20", 0xbed61414U, 0, false, true, "" },
    { "    s_flbit_i32_i64  s86, s[20:21]", 0xbed61514U, 0, false, true, "" },
    { "    s_sext_i32_i8  s86, s20", 0xbed61614U, 0, false, true, "" },
    { "    s_sext_i32_i16  s86, s20", 0xbed61714U, 0, false, true, "" },
    { "    s_bitset0_b32  s86, s20", 0xbed61814U, 0, false, true, "" },
    { "    s_bitset0_b64  s[86:87], s20", 0xbed61914U, 0, false, true, "" },
    { "    s_bitset1_b32  s86, s20", 0xbed61a14U, 0, false, true, "" },
    { "    s_bitset1_b64  s[86:87], s20", 0xbed61b14U, 0, false, true, "" },
    { "    s_getpc_b64  s[86:87]", 0xbed61c00U, 0, false, true, "" },
    { "    s_setpc_b64  s[20:21]", 0xbe801d14U, 0, false, true, "" },
    { "    s_swappc_b64  s[86:87], s[20:21]", 0xbed61e14U, 0, false, true, "" },
    { "    s_rfe_b64  s[20:21]", 0xbe801f14U, 0, false, true, "" },
    { "    s_and_saveexec_b64 s[86:87], s[20:21]", 0xbed62014U, 0, false, true, "" },
    { "    s_or_saveexec_b64 s[86:87], s[20:21]", 0xbed62114U, 0, false, true, "" },
    { "    s_xor_saveexec_b64 s[86:87], s[20:21]", 0xbed62214U, 0, false, true, "" },
    { "    s_andn2_saveexec_b64 s[86:87], s[20:21]", 0xbed62314U, 0, false, true, "" },
    { "    s_orn2_saveexec_b64 s[86:87], s[20:21]", 0xbed62414U, 0, false, true, "" },
    { "    s_nand_saveexec_b64 s[86:87], s[20:21]", 0xbed62514U, 0, false, true, "" },
    { "    s_nor_saveexec_b64 s[86:87], s[20:21]", 0xbed62614U, 0, false, true, "" },
    { "    s_xnor_saveexec_b64 s[86:87], s[20:21]", 0xbed62714U, 0, false, true, "" },
    { "    s_quadmask_b32  s86, s20",  0xbed62814U, 0, false, true, "" },
    { "    s_quadmask_b64  s[86:87], s[20:21]",  0xbed62914U, 0, false, true, "" },
    { "    s_movrels_b32  s86, s20",  0xbed62a14U, 0, false, true, "" },
    { "    s_movrels_b64  s[86:87], s[20:21]",  0xbed62b14U, 0, false, true, "" },
    { "    s_movreld_b32  s86, s20",  0xbed62c14U, 0, false, true, "" },
    { "    s_movreld_b64  s[86:87], s[20:21]",  0xbed62d14U, 0, false, true, "" },
    { "    s_cbranch_join  s20", 0xbe802e14U, 0, false, true, "" },
    { "    s_mov_regrd_b32 s86, s20", 0xbed62f14U, 0, false, true, "" },
    { "    s_abs_i32  s86, s20", 0xbed63014U, 0, false, true, "" },
    { "    s_mov_fed_b32  s86, s20", 0xbed63114U, 0, false, true, "" },
    { "    s_set_gpr_idx_idx s20", 0xbe803214U, 0, false, true, "" },
    /* SOPC encoding */
    { "    s_cmp_lt_i32  s29, s69", 0xbf04451dU, 0, false, true, "" },
    { "    s_bitcmp1_b32  s29, s69", 0xbf0d451dU, 0, false, true, "" },
    { "    s_bitcmp0_b64  s[28:29], s69", 0xbf0e451cU, 0, false, true, "" },
    { "    s_setvskip  s29, s69", 0xbf10451dU, 0, false, true, "" },
    /* SOPC new instructions */
    { "    s_set_gpr_idx_on s29, 0x45", 0xbf11451dU, 0, false, true, "" },
    { "    s_set_gpr_idx_on s29, 0xff", 0xbf11ff1dU, 0, false, true, "" }, // special case
    { "xd=43; s_set_gpr_idx_on s29, 4+xd", 0xbf112f1dU, 0, false, true, "" },
    { "s_set_gpr_idx_on s29, 4+xd;xd=43", 0xbf112f1dU, 0, false, true, "" },
    { "    s_cmp_eq_u64  s[28:29], s[68:69]", 0xbf12441cU, 0, false, true, "" },
    { "    s_cmp_lg_u64  s[28:29], s[68:69]", 0xbf13441cU, 0, false, true, "" },
    { "    s_cmp_ne_u64  s[28:29], s[68:69]", 0xbf13441cU, 0, false, true, "" },
    /* SOPK encoding */
    { "    s_movk_i32      s43, 0xd3b9", 0xb02bd3b9U, 0, false, true, "" },
    { "    s_cmovk_i32  s43, 0xd3b9", 0xb0abd3b9U, 0, false, true, "" },
    { "    s_cmpk_eq_i32  s43, 0xd3b9", 0xb12bd3b9U, 0, false, true, "" },
    { "    s_cmpk_lg_i32  s43, 0xd3b9", 0xb1abd3b9U, 0, false, true, "" },
    { "    s_cmpk_gt_i32  s43, 0xd3b9", 0xb22bd3b9U, 0, false, true, "" },
    { "    s_cmpk_ge_i32  s43, 0xd3b9", 0xb2abd3b9U, 0, false, true, "" },
    { "    s_cmpk_lt_i32  s43, 0xd3b9", 0xb32bd3b9U, 0, false, true, "" },
    { "    s_cmpk_le_i32  s43, 0xd3b9", 0xb3abd3b9U, 0, false, true, "" },
    { "    s_cmpk_eq_u32  s43, 0xd3b9", 0xb42bd3b9U, 0, false, true, "" },
    { "    s_cmpk_lg_u32  s43, 0xd3b9", 0xb4abd3b9U, 0, false, true, "" },
    { "    s_cmpk_gt_u32  s43, 0xd3b9", 0xb52bd3b9U, 0, false, true, "" },
    { "    s_cmpk_ge_u32  s43, 0xd3b9", 0xb5abd3b9U, 0, false, true, "" },
    { "    s_cmpk_lt_u32  s43, 0xd3b9", 0xb62bd3b9U, 0, false, true, "" },
    { "    s_cmpk_le_u32  s43, 0xd3b9", 0xb6abd3b9U, 0, false, true, "" },
    { "    s_addk_i32  s43, 0xd3b9", 0xb72bd3b9U, 0, false, true, "" },
    { "    s_mulk_i32  s43, 0xd3b9", 0xb7abd3b9U, 0, false, true, "" },
    { "    s_cbranch_i_fork s[44:45], xxxx-8\nxxxx:\n", 0xb82cfffeU, 0, false, true, "" },
    { "    s_getreg_b32    s43, hwreg(mode, 0, 1)", 0xb8ab0001U, 0, false, true, "" },
    { "    s_setreg_b32  hwreg(trapsts, 3, 10), s43", 0xb92b48c3u, 0, false, true, "" },
    { "    s_setreg_imm32_b32 hwreg(trapsts, 3, 10), 0x24da4f",
                    0xba0048c3u, 0x24da4fU, true, true, "" },
    /* SOPP encoding */
    { "    s_nop  7", 0xbf800007U, 0, false, true, "" },
    { "    s_cbranch_execnz  xxxx-8\nxxxx:\n", 0xbf89fffeU, 0, false, true, "" },
    { "    s_wakeup\n", 0xbf830000U, 0, false, true, "" },
    { "    s_setkill  0x32b\n", 0xbf8b032bU, 0, false, true, "" },
    { "    s_cbranch_execnz  xxxx-8\nxxxx:\n", 0xbf89fffeU, 0, false, true, "" },
    { "    s_endpgm_saved", 0xbf9b0000U, 0, false, true, "" },
    { "    s_set_gpr_idx_off", 0xbf9c0000U, 0, false, true, "" },
    { "    s_set_gpr_idx_mode 332", 0xbf9d014cU, 0, false, true, "" },
    /* SMEM encoding */
    { "    s_load_dword  s50, s[58:59], 0x1345b", 0xc0020c9dU, 0x1345b, true, true, "" },
    { "    s_load_dword  s50, s[58:59], 0x1345b glc   ",
        0xc0030c9dU, 0x1345b, true, true, "" },
    { "    s_load_dword  s50, s[58:59], 0x1345b glc  glc ",
        0xc0030c9dU, 0x1345b, true, true, "" },
    { "xd=17;xy=35; s_load_dword  s50, s[58:59], xd*xy",
        0xc0020c9dU, 17*35, true, true, "" },
    { "s_load_dword  s50, s[58:59], xd*xy;xd=17;xy=35",
        0xc0020c9dU, 17*35, true, true, "" },
    { "s6=0x4dca7; s_load_dword  s50, s[58:59], @s6",
        0xc0020c9dU, 0x4dca7U, true, true, "" },
    { "    s_load_dword  s50, s[58:59], s6 ", 0xc0000c9dU, 6, true, true, "" },
    /* SMEM warnings */
    { "xx=0x14dca7; s_load_dword  s50, s[58:59], xx", 0xc0020c9dU, 0x4dca7U, true, true,
        "test.s:1:43: Warning: Value 0x14dca7 truncated to 0x4dca7\n" },
    { "s_load_dword  s50, s[58:59], xx;xx=0x14dca7", 0xc0020c9dU, 0x4dca7U, true, true,
        "test.s:1:30: Warning: Value 0x14dca7 truncated to 0x4dca7\n" },
    /* SMEM errors */
    { "    s_load_dword  s[50:53], s[58:59], 0x1345b", 0, 0, false, false,
        "test.s:1:19: Error: Required 1 scalar register\n" },
    { "    s_load_dword  s50, s[58:58], 0x1345b", 0, 0, false, false,
        "test.s:1:24: Error: Required 2 scalar registers\n" },
    /* SMEM instructons */
    { "    s_load_dwordx2  s[50:51], s[58:59], 0x1b", 0xc0060c9dU, 0x1b, true, true, "" },
    { "    s_load_dwordx4  s[52:55], s[58:59], 0x1b", 0xc00a0d1dU, 0x1b, true, true, "" },
    { "    s_load_dwordx8  s[52:59], s[58:59], 0x1b", 0xc00e0d1dU, 0x1b, true, true, "" },
    { "    s_load_dwordx16  s[52:67], s[58:59], 0x1b", 0xc0120d1dU, 0x1b, true, true, "" },
    { "    s_buffer_load_dword s50, s[56:59], 0x5b", 0xc0220c9cU, 0x5b, true, true, "" },
    { "    s_buffer_load_dwordx2 s[50:51], s[56:59], 0x5b",
        0xc0260c9cU, 0x5b, true, true, "" },
    { "    s_buffer_load_dwordx4 s[52:55], s[56:59], 0x5b",
        0xc02a0d1cU, 0x5b, true, true, "" },
    { "    s_buffer_load_dwordx8 s[52:59], s[56:59], 0x5b",
        0xc02e0d1cU, 0x5b, true, true, "" },
    { "    s_buffer_load_dwordx16 s[52:67], s[56:59], 0x5b",
        0xc0320d1cU, 0x5b, true, true, "" },
    { "    s_store_dword  s50, s[58:59], 0x1b", 0xc0420c9dU, 0x1b, true, true, "" },
    { "    s_store_dwordx2  s[50:51], s[58:59], 0x1b", 0xc0460c9dU, 0x1b, true, true, "" },
    { "    s_store_dwordx4  s[52:55], s[58:59], 0x1b", 0xc04a0d1dU, 0x1b, true, true, "" },
    { "    s_buffer_store_dword  s50, s[60:63], 0x1b", 0xc0620c9eU, 0x1b, true, true, "" },
    { "    s_buffer_store_dwordx2  s[50:51], s[60:63], 0x1b",
        0xc0660c9eU, 0x1b, true, true, "" },
    { "    s_buffer_store_dwordx4  s[52:55], s[60:63], 0x1b",
        0xc06a0d1eU, 0x1b, true, true, "" },
    { "    s_dcache_inv", 0xc0800000U, 0, true, true, "" },
    { "    s_dcache_inv   glc ", 0xc0810000U, 0, true, true, "" },
    { "    s_dcache_wb", 0xc0840000U, 0, true, true, "" },
    { "    s_dcache_inv_vol", 0xc0880000U, 0, true, true, "" },
    { "    s_dcache_wb_vol", 0xc08c0000U, 0, true, true, "" },
    { "    s_memtime  s[52:53]", 0xc0900d00U, 0, true, true, "" },
    { "    s_memrealtime  s[52:53]", 0xc0940d00U, 0, true, true, "" },
    { "    s_atc_probe  0x32, s[58:59], 0xfff5b", 0xc09a0c9dU, 0xfff5b, true, true, "" },
    { "da=0x35; s_atc_probe  0x35, s[58:59], 0xfff5b",
        0xc09a0d5dU, 0xfff5b, true, true, "" },
    { "    s_atc_probe_buffer  0x32, s[56:59], 0xfff5b",
        0xc09e0c9cU, 0xfff5b, true, true, "" },
    { "s_atc_probe_buffer  x, s[56:59], 0xfff5b; x=0x37",
        0xc09e0ddcU, 0xfff5b, true, true, "" },
    { "s_atc_probe_buffer  x, s[56:59], 0xfff5b; x=0xb7", 0xc09e0ddcU, 0xfff5b, true, true,
        "test.s:1:21: Warning: Value 0xb7 truncated to 0x37\n" },
    { "    s_atc_probe  0xb2, s[58:59], 0xfff5b", 0xc09a0c9dU, 0xfff5b, true, true,
        "test.s:1:18: Warning: Value 0xb2 truncated to 0x32\n" },
    /* VOP2 encoding - VOP_SDWA */
    { "   v_cndmask_b32   v154, v0, v107, vcc dst_sel:byte0 src0_sel:byte0 src1_sel:byte0",
        0x0134d6f9U, 0, true, true, "" },
    { "   v_cndmask_b32   v154, v0, v107, vcc dst_sel:byte_0 src0_sel:BYTE_0 src1_sel:b0",
        0x0134d6f9U, 0, true, true, "" },
    { "   v_cndmask_b32   v154, v0, v107, vcc dst_sel :byte_0 src0_sel : b0 src1_sel : b0 ",
        0x0134d6f9U, 0, true, true, "" },
    { "   v_cndmask_b32   v154, v61, v107, vcc dst_sel:byte0 src0_sel:byte0 src1_sel:byte0",
        0x0134d6f9U, 0x3dU, true, true, "" },
    { "   v_cndmask_b32   v154, v61, v107, vcc dst_sel:byte3 src0_sel:byte2 src1_sel:byte1",
        0x0134d6f9U, 0x0102033dU, true, true, "" },
    { "   v_cndmask_b32   v154, v65, v107, vcc dst_sel:b1 src0_sel:byte1 src1_sel:byte_1",
        0x0134d6f9U, 0x01010141U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, v107, vcc dst_sel:b2 src0_sel:byte2 src1_sel:byte_2",
        0x0134d6f9U, 0x02020241U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, v107, vcc dst_sel:b3 src0_sel:byte3 src1_sel:byte_3",
        0x0134d6f9U, 0x03030341U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, v107, vcc dst_sel:w0 src0_sel:word0 src1_sel:word_0",
        0x0134d6f9U, 0x04040441U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, v107, vcc dst_sel:w1 src0_sel:word1 src1_sel:word_1",
        0x0134d6f9U, 0x05050541U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, v107, vcc dst_sel:w1 src0_sel:word0 src1_sel:dword",
        0x0134d6f9U, 0x06040541U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, v107, vcc dst_un:preserve",
        0x0134d6f9U, 0x06061641U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, v107, vcc dst_unused:UNUSED_PRESERVE",
        0x0134d6f9U, 0x06061641U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, v107, vcc dst_unused : SEXT",
        0x0134d6f9U, 0x06060e41U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, v107, vcc dst_unused:pad",
        0x0134d6f9U, 0x06060641U, true, true, "" },
    /* VOP_SDWA - operand modifiers */
    { "   v_cndmask_b32   v154, abs(v65), v107, vcc dst_sel:word1",
        0x0134d6f9U, 0x06260541U, true, true, "" },
    { "   v_cndmask_b32   v154, -abs(v65), v107, vcc dst_sel:word1",
        0x0134d6f9U, 0x06360541U, true, true, "" },
    { "   v_cndmask_b32   v154, sext(-abs(v65)), v107, vcc dst_sel:word1",
        0x0134d6f9U, 0x063e0541U, true, true, "" },
    { "   v_cndmask_b32   v154, sext(-v65), v107, vcc dst_sel:word1",
        0x0134d6f9U, 0x061e0541U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, abs(v107), vcc dst_sel:word1",
        0x0134d6f9U, 0x26060541U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, -abs(v107), vcc dst_sel:word1",
        0x0134d6f9U, 0x36060541U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, sext(-abs(v107)), vcc dst_sel:word1",
        0x0134d6f9U, 0x3e060541U, true, true, "" },
    { "   v_cndmask_b32   v154, v65, sext(-v107), vcc dst_sel:word1",
        0x0134d6f9U, 0x1e060541U, true, true, "" },
    { "   v_cndmask_b32   v154, abs(v65), v107, vcc clamp dst_sel:word1",
        0x0134d6f9U, 0x06262541U, true, true, "" },
    /* VOP_SDWA - warnings */
    { "   v_cndmask_b32   v154, abs(v65), v107, vcc dst_sel:word1 dst_sel:word0",
        0x0134d6f9U, 0x06260441U, true, true,
        "test.s:1:60: Warning: Dst_sel is already defined\n" },
    { "   v_cndmask_b32   v154, abs(v65), v107, vcc src0_sel:word1 src0_sel:word0",
        0x0134d6f9U, 0x06240641U, true, true,
        "test.s:1:61: Warning: Src0_sel is already defined\n" },
    { "   v_cndmask_b32   v154, abs(v65), v107, vcc src1_sel:word1 src1_sel:word0",
        0x0134d6f9U, 0x04260641U, true, true,
        "test.s:1:61: Warning: Src1_sel is already defined\n" },
    { "   v_cndmask_b32   v154, v65, v107, vcc dst_unused:pad dst_un:PRESERVE",
        0x0134d6f9U, 0x06061641U, true, true,
        "test.s:1:56: Warning: Dst_unused is already defined\n" },
    /* VOP_SDWA - errors */
    { "   v_cndmask_b32   v154, v67, v107, vcc dst_sel:byte",
        0, 0, false, false, "test.s:1:49: Error: Unknown dst_sel\n" },
    { "   v_cndmask_b32   v154, v67, v107, vcc dst_sel:",
        0, 0, false, false, "test.s:1:49: Error: Expected dst_sel\n" },
    { nullptr, 0, 0, false, false, 0 }
};

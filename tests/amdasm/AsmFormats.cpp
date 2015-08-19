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
#include <iostream>
#include <cstdio>
#include <sstream>
#include <map>
#include <memory>
#include <CLRX/utils/Containers.h>
#include <CLRX/amdasm/Assembler.h>
#include "../TestUtils.h"

using namespace CLRX;

static const char* galliumArgTypeTbl[] =
{
    "scalar", "constant", "global", "local", "image2dro", "image2dwr",
    "image3dro", "image3dwr", "sampler"
};

static const char* galliumArgSemanticTbl[] =
{   "general", "griddim", "gridoffset" };

static const char* argTypeNameTbl[] =
{
    "void", "uchar", "char", "ushort", "short", "uint", "int",
    "ulong", "long", "float", "double", "pointer",
    "image", "image1d", "image1d_array", "image1d_buffer",
    "image2d", "image2d_array", "image3d",
    "uchar2", "uchar3", "uchar4", "uchar8", "uchar16",
    "char2", "char3", "char4", "char8", "char16",
    "ushort2", "ushort3", "ushort4", "ushort8", "ushort16",
    "short2", "short3", "short4", "short8", "short16",
    "uint2", "uint3", "uint4", "uint8", "uint16",
    "int2", "int3", "int4", "int8", "int16",
    "ulong2", "ulong3", "ulong4", "ulong8", "ulong16",
    "long2", "long3", "long4", "long8", "long16",
    "float2", "float3", "float4", "float8", "float16",
    "double2", "double3", "double4", "double8", "double16",
    "sampler", "structure", "counter32", "counter64"
};

static const char* ptrSpaceNameTbl[] =
{ "none", "local", "constant", "global" };

static const char* calNoteNamesTbl[] =
{
    "none", "proginfo", "inputs", "outputs", "condout", "float32consts", "int32consts",
    "bool32consts", "earlyexit", "globalbuffers", "constantbuffers", "inputsamplers",
    "persistentbuffers", "scratchbuffers", "subconstantbuffers", "uavmailboxsize",
    "uav", "uavopmask"
};

static void printHexData(std::ostream& os, cxuint indentLevel, size_t size,
             const cxbyte* data)
{
    if (data==nullptr)
    {
        for (cxuint j = 0; j < indentLevel; j++)
            os << "  ";
        os << "nullptr\n";
        return;
    }
    for (size_t i = 0; i < size; i++)
    {
        if ((i&31)==0)
            for (cxuint j = 0; j < indentLevel; j++)
                os << "  ";
        char buf[10];
        snprintf(buf, 10, "%02x", cxuint(data[i]));
        os << buf;
        if ((i&31)==31 || i+1 == size)
            os << '\n';
    }
}

static std::string confValueToString(uint32_t val)
{
    if (val == AMDBIN_DEFAULT)
        return "default";
    if (val == AMDBIN_NOTSUPPLIED)
        return "notsup";
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

static void printAmdOutput(std::ostream& os, const AmdInput* output)
{
    os << "AmdBinDump:" << std::endl;
    os << "  Bitness=" << ((output->is64Bit) ? "64-bit" : "32-bit") << ", "
            "devType=" << getGPUDeviceTypeName(output->deviceType) << ", "
            "drvVersion=" << output->driverVersion << ", "
            "drvInfo=\"" << output->driverInfo << "\", "
            "compileOptions=\"" << output->compileOptions << "\"\n";
    
    for (const AmdKernelInput& kernel: output->kernels)
    {
        os << "  Kernel: " << kernel.kernelName << "\n";
        os << "    Data:\n";
        printHexData(os, 2, kernel.dataSize, kernel.data);
        os << "    Code:\n";
        printHexData(os, 2, kernel.codeSize, kernel.code);
        if (!kernel.useConfig)
        {
            os << "    Header:\n";
            printHexData(os, 2, kernel.headerSize, kernel.header);
            os << "    Metadata:\n";
            printHexData(os, 2, kernel.metadataSize, (const cxbyte*)kernel.metadata);
            // CALNotes
            if (!kernel.calNotes.empty())
            {
                for(const CALNoteInput calNote: kernel.calNotes)
                {
                    os << "    CALNote: type=";
                    if (calNote.header.type<=CALNOTE_ATI_MAXTYPE)
                        os << calNoteNamesTbl[calNote.header.type];
                    else // unknown
                        os << calNote.header.type;
                    os << ", nameSize=" << calNote.header.nameSize << "\n";
                    printHexData(os, 2, calNote.header.descSize, calNote.data);
                }
            }
        }
        else
        {   // when config
            const AmdKernelConfig& config = kernel.config;
            os << "    Config:\n";
            for (AmdKernelArgInput arg: config.args)
                os << "      Arg: \"" << arg.argName << "\", \"" <<
                        arg.typeName << "\", " <<
                        argTypeNameTbl[cxuint(arg.argType)] << ", " <<
                        argTypeNameTbl[cxuint(arg.pointerType)] << ", " <<
                        ptrSpaceNameTbl[cxuint(arg.ptrSpace)] << ", " <<
                        cxuint(arg.ptrAccess) << ", " << arg.structSize << ", " <<
                        arg.constSpaceSize << ", " << confValueToString(arg.resId) << ", " <<
                        (arg.used ? "true":"false")<< "\n";
            
            if (!config.samplers.empty())
            {
                os << "      Sampler:";
                for (cxuint sampler: config.samplers)
                    os << " " << sampler;
                os << '\n';
            }
            os << "      cws=" << config.reqdWorkGroupSize[0] << " " <<
                    config.reqdWorkGroupSize[1] << " " << config.reqdWorkGroupSize[2] << ", "
                    "SGPRS=" << confValueToString(config.usedSGPRsNum) << ", "
                    "VGPRS=" << confValueToString(config.usedVGPRsNum) << ", "
                    "pgmRSRC2=" << std::hex << "0x" << config.pgmRSRC2 << ", "
                    "ieeeMode=0x" << config.ieeeMode << "\n      "
                    "floatMode=0x" << config.floatMode<< std::dec << ", "
                    "hwLocalSize=" << config.hwLocalSize << ", "
                    "hwRegion=" << confValueToString(config.hwRegion) << ", "
                    "scratchBuffer=" << config.scratchBufferSize << "\n      "
                    "uavPrivate=" << confValueToString(config.uavPrivate) << ", "
                    "uavId=" << confValueToString(config.uavId) << ", "
                    "constBufferId=" << confValueToString(config.constBufferId) << ", "
                    "printfId=" << confValueToString(config.printfId) << "\n      "
                    "privateId=" << confValueToString(config.privateId) << ", "
                    "earlyExit=" << config.earlyExit << ","
                    "condOut=" << config.condOut << ", " <<
                    (config.usePrintf?"usePrintf ":"") <<
                    (config.useConstantData?"useConstantData ":"") << "\n";
             for (cxuint u = 0; u < config.userDataElemsNum; u++)
                 os << "      UserData: " << config.userDatas[u].dataClass << ", " <<
                        config.userDatas[u].apiSlot << ", " <<
                        config.userDatas[u].regStart << ", " <<
                        config.userDatas[u].regSize << "\n";
        }
        for (BinSection section: kernel.extraSections)
        {
            os << "  Section " << section.name << ":\n";
            printHexData(os, 3, section.size, section.data);
        }
        for (BinSymbol symbol: kernel.extraSymbols)
                os << "    Symbol: name=" << symbol.name << ", value=" << symbol.value <<
                ", size=" << symbol.size << ", section=" << symbol.sectionId << "\n";
        os.flush();
    }
    os << "  GlobalData:\n";
    printHexData(os,  1, output->globalDataSize, output->globalData);
    for (BinSection section: output->extraSections)
    {
        os << "  Section " << section.name << ":\n";
        printHexData(os, 1, section.size, section.data);
    }
    for (BinSymbol symbol: output->extraSymbols)
        os << "  Symbol: name=" << symbol.name << ", value=" << symbol.value <<
                ", size=" << symbol.size << ", section=" << symbol.sectionId << "\n";
    os.flush();
}

static void printGalliumOutput(std::ostream& os, const GalliumInput* output)
{
    os << "GalliumBinDump:" << std::endl;
    for (const GalliumKernelInput& kernel: output->kernels)
    {
        os << "  Kernel: name=" << kernel.kernelName << ", " <<
                "offset=" << kernel.offset << "\n";
        os << "    ProgInfo: ";
        for (cxuint i = 0; i < 3; i++)
            os << "0x" << std::hex << kernel.progInfo[i].address <<
                    "=0x" << kernel.progInfo[i].value << ((i==2)?"\n":", ");
        os << std::dec;
        for (const GalliumArgInfo& arg: kernel.argInfos)
        {
            os << "    Arg: " << galliumArgTypeTbl[cxuint(arg.type)] << ", " <<
                    ((arg.signExtended) ? "true" : "false") << ", " <<
                    galliumArgSemanticTbl[cxuint(arg.semantic)] << ", " <<
                    "size=" << arg.size << ", tgtSize=" << arg.targetSize << ", " <<
                    "tgtAlign=" << arg.targetAlign << "\n";
        }
        os.flush();
    }
    os << "  Comment:\n";
    printHexData(os, 1, output->commentSize, (const cxbyte*)output->comment);
    os << "  GlobalData:\n";
    printHexData(os, 1, output->globalDataSize, output->globalData);
    os << "  Code:\n";
    printHexData(os, 1, output->codeSize, output->code);
    
    for (BinSection section: output->extraSections)
    {
        os << "  Section " << section.name << ":\n";
        printHexData(os, 1, section.size, section.data);
    }
    for (BinSymbol symbol: output->extraSymbols)
        os << "  Symbol: name=" << symbol.name << ", value=" << symbol.value <<
                ", size=" << symbol.size << ", section=" << symbol.sectionId << "\n";
    os.flush();
}


struct AsmTestCase
{
    const char* input;
    const char* dump;
    const char* errors;
    bool good;
};

static const AsmTestCase asmTestCases1Tbl[] =
{
    /* 0 - gallium  */
    {
        R"ffDXD(            .gallium
            .kernel firstKernel
            .args
            .arg scalar,8
            .arg local,4
            .arg constant,4
            .arg global,4
            .arg image3d_rd,4
            .arg image2d_rd,4
            .arg image3d_wr,4
            .arg image2d_wr,4
            .arg sampler,8
            .arg scalar,8, , ,sext
            .arg scalar,8, , ,sext,general
            .arg scalar,8, 18,2
            .arg scalar,20, 18,2
            .arg scalar,8, , , ,griddim
            .arg griddim,4
            .arg gridoffset,4
            .arg scalar, 11111111111, 22222222222222, 4
            .section .comment
            .ascii "nocomments"
            .globaldata
            .byte 0xf0,0xfd,0x3d,0x44
            .kernel secondKernel
            .proginfo
            .entry 12,22
            .entry 14,288
            .entry 16,160
            .args
            .arg scalar,4
            .arg scalar,4
            .arg griddim,4
            .arg gridoffset,4
            .kernel thirdKernel
            .proginfo
            .entry 0xfffffaaaaa, 0x12233
            .entry 0xff, 0x111223030
            .entry 1,2
            .text
firstKernel: .byte 1,22,3,4
            .p2align 4
secondKernel:.byte 77,76,75,90,11
thirdKernel:
            .section .info1
            .ascii "noinfo"
            .section .infox
            .ascii "refer to some link")ffDXD",
        /* dump */
        R"ffDXD(GalliumBinDump:
  Kernel: name=firstKernel, offset=0
    ProgInfo: 0xb848=0xc0000, 0xb84c=0x1788, 0xb860=0x0
    Arg: scalar, false, general, size=8, tgtSize=8, tgtAlign=8
    Arg: local, false, general, size=4, tgtSize=4, tgtAlign=4
    Arg: constant, false, general, size=4, tgtSize=4, tgtAlign=4
    Arg: global, false, general, size=4, tgtSize=4, tgtAlign=4
    Arg: image3dro, false, general, size=4, tgtSize=4, tgtAlign=4
    Arg: image2dro, false, general, size=4, tgtSize=4, tgtAlign=4
    Arg: image3dwr, false, general, size=4, tgtSize=4, tgtAlign=4
    Arg: image2dwr, false, general, size=4, tgtSize=4, tgtAlign=4
    Arg: sampler, false, general, size=8, tgtSize=8, tgtAlign=8
    Arg: scalar, true, general, size=8, tgtSize=8, tgtAlign=8
    Arg: scalar, true, general, size=8, tgtSize=8, tgtAlign=8
    Arg: scalar, false, general, size=8, tgtSize=18, tgtAlign=2
    Arg: scalar, false, general, size=20, tgtSize=18, tgtAlign=2
    Arg: scalar, false, griddim, size=8, tgtSize=8, tgtAlign=8
    Arg: scalar, false, griddim, size=4, tgtSize=4, tgtAlign=4
    Arg: scalar, false, gridoffset, size=4, tgtSize=4, tgtAlign=4
    Arg: scalar, false, general, size=2521176519, tgtSize=61432718, tgtAlign=4
  Kernel: name=secondKernel, offset=16
    ProgInfo: 0xc=0x16, 0xe=0x120, 0x10=0xa0
    Arg: scalar, false, general, size=4, tgtSize=4, tgtAlign=4
    Arg: scalar, false, general, size=4, tgtSize=4, tgtAlign=4
    Arg: scalar, false, griddim, size=4, tgtSize=4, tgtAlign=4
    Arg: scalar, false, gridoffset, size=4, tgtSize=4, tgtAlign=4
  Kernel: name=thirdKernel, offset=21
    ProgInfo: 0xfffaaaaa=0x12233, 0xff=0x11223030, 0x1=0x2
  Comment:
  6e6f636f6d6d656e7473
  GlobalData:
  f0fd3d44
  Code:
  011603040000000000000000000000004d4c4b5a0b
  Section .info1:
  6e6f696e666f
  Section .infox:
  726566657220746f20736f6d65206c696e6b
)ffDXD",
        "test.s:20:26: Warning: Size of argument out of range\n"
        "test.s:20:39: Warning: Target size of argument out of range\n"
        "test.s:37:20: Warning: Value 0xfffffaaaaa truncated to 0xfffaaaaa\n"
        "test.s:38:26: Warning: Value 0x111223030 truncated to 0x11223030\n", true
    },
    /* 1 - gallium (errors) */
    {
        R"ffDXD(            .gallium
            .kernel firstKernel
            .entry 111,1
            .arg scalar,8
            .arg local,4
            .arg constant,4
            .args
            .arg sclar,5
            .arg scalar
            .arg image3d_rd,0,0,1
            .arg sampler,4,4,4     , zxx,    ssds
            .arg scalar,8, 18,2,zext,general ,
            .proginfo
            .entry ,
            .entry  ,66
            .entry 66,
            .kernel secondKernel
            .proginfo
            .entry 1,2
            .entry 2,3
            .entry 3,4
            .entry 5,6
            .args
            .proginfo
            .entry 7,8)ffDXD",
        /* dump */
        "",
        /* errors */
        R"ffDXD(test.s:3:13: Error: ProgInfo entry definition outside ProgInfo
test.s:4:13: Error: Argument definition outside arguments list
test.s:5:13: Error: Argument definition outside arguments list
test.s:6:13: Error: Argument definition outside arguments list
test.s:8:18: Error: Unknown argument type
test.s:9:24: Error: Expected ',' before argument
test.s:10:29: Warning: Size of argument out of range
test.s:10:31: Warning: Target size of argument out of range
test.s:11:38: Error: Unknown numeric extension
test.s:11:46: Error: Unknown argument semantic type
test.s:12:46: Error: Garbages at end of line
test.s:14:20: Error: Expected expression
test.s:14:21: Error: Expected expression
test.s:15:21: Error: Expected expression
test.s:16:23: Error: Expected expression
test.s:22:13: Error: Maximum 3 entries can be in ProgInfo
test.s:25:13: Error: Maximum 3 entries can be in ProgInfo
)ffDXD", false
    },
    {
        R"ffDXD(            .amd
            
            .kernel non_config
            .header
            .byte 0,2,3,4,5,65,6,6,8
            .metadata
            .ascii "alamakota. this is only test"
            .data
            .byte 77,55,33
            .text
            .int 0x12334dcd,0x23349aac,0x348d8189
            .boolconsts
            .segment 31,42
            .proginfo
            .entry 123,544
            .entry 112,1
            .entry 1,21
            .intconsts
            .segment 2334,6553
            .segment 66544,112
            .earlyexit 23
            .boolconsts
            .segment 12,344
            .segment 15,112
            .outputs
            .short 0xfda,0x223d
            .inputs
            .short 0xfdb,0x223e
            .scratchbuffers
            .short 55,77
            .persistentbuffers
            .short 155,177
            .globalbuffers
            .short 5,6,7,168,88
            .condout 664
            .byte 122
            .inputsamplers
            .sampler 0,1
            .sampler 5,7
            .sampler 77, 5
            .floatconsts
            .segment 71,77
            .segment 11,0
            .constantbuffers
            .cbid 0xfeda,0xdb44
            .calnote 24
            .hword 4321,64321
            .calnote 112
            .hword 999,888
            .subconstantbuffers
            .byte 11,10,9,8,7
            .uavmailboxsize 145
            .uavopmask 344
            .uav
            .entry 11,20,23,21
            .entry 15,25,27,29
            .calnote 0
            .byte 55,66,77
            
            .rodata
            .ascii "this is rodata"
            .section .ubu
            .ascii "this is ubu"
            
            .kernel defconfigured
            .ascii "this is code"
            .config
            
            .kernel configured
            .ascii "this is code2"
            .config
            .cws 554,44,11
            .sampler 55,44,332,121
            .sgprsnum 24
            .vgprsnum 47
            .pgmrsrc2 0xaabbccdd
            .ieeemode 0x03
            .floatmode 0xe0
            .hwlocal 0x33
            .hwregion 394
            .scratchbuffer 9
            .printfid 10
            .privateid 8
            .cbid 11
            .uavid 12
            .uavprivate 8
            .earlyexit 1
            .condout 2
            .userdata imm_kernel_arg,0,0,4
            .userdata ptr_indirect_uav,0,4,1
            .userdata ptr_indirect_uav,0,5,1
            .userdata imm_sampler,0,6,2
            .arg v0,double
            .arg v1,"double_t",double
            .arg v2,double2
            .arg v3,double3
            .arg v4,double4
            .arg v5,double8
            .arg v6,double16
            .arg v7,"float_t",float
            .arg v8,float2
            .arg v9,float3
            .arg v10,float4
            .arg v11,float8
            .arg v12,float16
            .arg v13,float16,unused
            .arg v14,short8
            .arg v15,uint4
            .arg v16,long3
            .arg v17,sampler
            .arg v18,structure,24
            .arg v19,structure,44,unused
            .arg v20,counter32,3
            .arg v21,counter32,3,unused
            .arg v22,image
            .arg v23,image2d
            .arg v24,image3d
            .arg v25,image2d_array
            .arg v26,image1d_buffer
            .arg v27,image1d_array
            .arg v28,image1d
            .arg v29,image2d,write_only
            .arg v30,image2d,,5
            .arg v31,image2d,,5,unused
            .arg v32,image2d,,,unused
            .arg v33,"myimage",image3d
            .arg v34,void*,global
            .arg v35,void*,global,const
            .arg v36,void*,local
            .arg v37,void*,constant
            .arg v38,uchar8*,global
            .arg v39,double3*,global
            .arg v40,long4*,global
            .arg v41,ulong16  *,global
            .arg v42,ulong16  *,global, restrict
            .arg v43,ulong16  *,global, restrict volatile
            .arg v44,ulong16  *,global, restrict restrict volatile const
            .arg v45,ulong16  *,global,
            .arg v46,structure*,0,global,
            .arg v47,structure*,14,global,const
            .arg v48,structure*,14,global,const, 17
            .arg v49,ulong16  *,global,,18
            .arg v50,structure*,14,global,const, 17, unused
            .arg v51,float*,global,,,unused
            .arg v52,float*,local,restrict,unused
            .arg v53,float*,local,restrict
            .arg v54,float*,constant,,40
            .arg v55,float*,constant,,40,20
            .arg v56,float*,constant,,40,20,unused
            .arg v57,structure*,82,global,,,unused
            .arg v58,structure*,78,local,restrict,unused
            .arg v59,structure*,110,local,restrict
            .arg v60,structure*,17,constant,,40
            .arg v61,structure*,19,constant,,40,20
            .arg v62,structure*,22,constant,,40,20,unused
            .section .notknown
            .ascii "notknownsection")ffDXD",
        /* dump */
        R"ffDXD(AmdBinDump:
  Bitness=32-bit, devType=CapeVerde, drvVersion=0, drvInfo="", compileOptions=""
  Kernel: non_config
    Data:
    4d3721
    Code:
    cd4d3312ac9a342389818d34
    Header:
    000203040541060608
    Metadata:
    616c616d616b6f74612e2074686973206973206f6e6c792074657374
    CALNote: type=bool32consts, nameSize=8
    1f0000002a000000
    CALNote: type=proginfo, nameSize=8
    7b0000002002000070000000010000000100000015000000
    CALNote: type=int32consts, nameSize=8
    1e09000099190000f003010070000000
    CALNote: type=earlyexit, nameSize=8
    17000000
    CALNote: type=bool32consts, nameSize=8
    0c000000580100000f00000070000000
    CALNote: type=outputs, nameSize=8
    da0f3d22
    CALNote: type=inputs, nameSize=8
    db0f3e22
    CALNote: type=scratchbuffers, nameSize=8
    37004d00
    CALNote: type=persistentbuffers, nameSize=8
    9b00b100
    CALNote: type=globalbuffers, nameSize=8
    050006000700a8005800
    CALNote: type=condout, nameSize=8
    980200007a
    CALNote: type=inputsamplers, nameSize=8
    000000000100000005000000070000004d00000005000000
    CALNote: type=float32consts, nameSize=8
    470000004d0000000b00000000000000
    CALNote: type=constantbuffers, nameSize=8
    dafe000044db0000
    CALNote: type=24, nameSize=8
    e11041fb
    CALNote: type=112, nameSize=8
    e7037803
    CALNote: type=subconstantbuffers, nameSize=8
    0b0a090807
    CALNote: type=uavmailboxsize, nameSize=8
    91000000
    CALNote: type=uavopmask, nameSize=8
    58010000
    CALNote: type=uav, nameSize=8
    0b0000001400000017000000150000000f000000190000001b0000001d000000
    CALNote: type=none, nameSize=8
    37424d
  Section .rodata:
      7468697320697320726f64617461
  Section .ubu:
      7468697320697320756275
  Kernel: defconfigured
    Data:
    nullptr
    Code:
    7468697320697320636f6465
    Config:
      cws=0 0 0, SGPRS=default, VGPRS=default, pgmRSRC2=0x0, ieeeMode=0x0
      floatMode=0xc0, hwLocalSize=0, hwRegion=default, scratchBuffer=0
      uavPrivate=default, uavId=default, constBufferId=default, printfId=default
      privateId=default, earlyExit=0,condOut=0, 
  Kernel: configured
    Data:
    nullptr
    Code:
    7468697320697320636f646532
    Config:
      Arg: "v0", "double", double, void, none, 0, 0, 0, default, true
      Arg: "v1", "double_t", double, void, none, 0, 0, 0, default, true
      Arg: "v2", "double2", double2, void, none, 0, 0, 0, default, true
      Arg: "v3", "double3", double3, void, none, 0, 0, 0, default, true
      Arg: "v4", "double4", double4, void, none, 0, 0, 0, default, true
      Arg: "v5", "double8", double8, void, none, 0, 0, 0, default, true
      Arg: "v6", "double16", double16, void, none, 0, 0, 0, default, true
      Arg: "v7", "float_t", float, void, none, 0, 0, 0, default, true
      Arg: "v8", "float2", float2, void, none, 0, 0, 0, default, true
      Arg: "v9", "float3", float3, void, none, 0, 0, 0, default, true
      Arg: "v10", "float4", float4, void, none, 0, 0, 0, default, true
      Arg: "v11", "float8", float8, void, none, 0, 0, 0, default, true
      Arg: "v12", "float16", float16, void, none, 0, 0, 0, default, true
      Arg: "v13", "float16", float16, void, none, 0, 0, 0, default, false
      Arg: "v14", "short8", short8, void, none, 0, 0, 0, default, true
      Arg: "v15", "uint4", uint4, void, none, 0, 0, 0, default, true
      Arg: "v16", "long3", long3, void, none, 0, 0, 0, default, true
      Arg: "v17", "sampler_t", sampler, void, none, 0, 0, 0, default, true
      Arg: "v18", "structure", structure, void, none, 0, 24, 0, default, true
      Arg: "v19", "structure", structure, void, none, 0, 44, 0, default, false
      Arg: "v20", "counter32_t", counter32, void, none, 0, 0, 0, 3, true
      Arg: "v21", "counter32_t", counter32, void, none, 0, 0, 0, 3, false
      Arg: "v22", "image2d_t", image, void, none, 1, 0, 0, default, true
      Arg: "v23", "image2d_t", image2d, void, none, 1, 0, 0, default, true
      Arg: "v24", "image3d_t", image3d, void, none, 1, 0, 0, default, true
      Arg: "v25", "image2d_array_t", image2d_array, void, none, 1, 0, 0, default, true
      Arg: "v26", "image1d_buffer_t", image1d_buffer, void, none, 1, 0, 0, default, true
      Arg: "v27", "image1d_array_t", image1d_array, void, none, 1, 0, 0, default, true
      Arg: "v28", "image1d_t", image1d, void, none, 1, 0, 0, default, true
      Arg: "v29", "image2d_t", image2d, void, none, 2, 0, 0, default, true
      Arg: "v30", "image2d_t", image2d, void, none, 1, 0, 0, 5, true
      Arg: "v31", "image2d_t", image2d, void, none, 1, 0, 0, 5, false
      Arg: "v32", "image2d_t", image2d, void, none, 1, 0, 0, default, false
      Arg: "v33", "myimage", image3d, void, none, 1, 0, 0, default, true
      Arg: "v34", "void*", pointer, void, global, 0, 0, 0, default, true
      Arg: "v35", "void*", pointer, void, global, 4, 0, 0, default, true
      Arg: "v36", "void*", pointer, void, local, 0, 0, 0, default, true
      Arg: "v37", "void*", pointer, void, constant, 0, 0, 0, default, true
      Arg: "v38", "uchar8*", pointer, uchar8, global, 0, 0, 0, default, true
      Arg: "v39", "double3*", pointer, double3, global, 0, 0, 0, default, true
      Arg: "v40", "long4*", pointer, long4, global, 0, 0, 0, default, true
      Arg: "v41", "ulong16*", pointer, ulong16, global, 0, 0, 0, default, true
      Arg: "v42", "ulong16*", pointer, ulong16, global, 8, 0, 0, default, true
      Arg: "v43", "ulong16*", pointer, ulong16, global, 24, 0, 0, default, true
      Arg: "v44", "ulong16*", pointer, ulong16, global, 28, 0, 0, default, true
      Arg: "v45", "ulong16*", pointer, ulong16, global, 0, 0, 0, default, true
      Arg: "v46", "structure*", pointer, structure, global, 0, 0, 0, default, true
      Arg: "v47", "structure*", pointer, structure, global, 4, 14, 0, default, true
      Arg: "v48", "structure*", pointer, structure, global, 4, 14, 0, 17, true
      Arg: "v49", "ulong16*", pointer, ulong16, global, 0, 0, 0, 18, true
      Arg: "v50", "structure*", pointer, structure, global, 4, 14, 0, 17, false
      Arg: "v51", "float*", pointer, float, global, 0, 0, 0, default, false
      Arg: "v52", "float*", pointer, float, local, 8, 0, 0, default, false
      Arg: "v53", "float*", pointer, float, local, 8, 0, 0, default, true
      Arg: "v54", "float*", pointer, float, constant, 0, 0, 40, default, true
      Arg: "v55", "float*", pointer, float, constant, 0, 0, 40, 20, true
      Arg: "v56", "float*", pointer, float, constant, 0, 0, 40, 20, false
      Arg: "v57", "structure*", pointer, structure, global, 0, 82, 0, default, false
      Arg: "v58", "structure*", pointer, structure, local, 8, 78, 0, default, false
      Arg: "v59", "structure*", pointer, structure, local, 8, 110, 0, default, true
      Arg: "v60", "structure*", pointer, structure, constant, 0, 17, 40, default, true
      Arg: "v61", "structure*", pointer, structure, constant, 0, 19, 40, 20, true
      Arg: "v62", "structure*", pointer, structure, constant, 0, 22, 40, 20, false
      Sampler: 55 44 332 121
      cws=554 44 11, SGPRS=24, VGPRS=47, pgmRSRC2=0xaabbccdd, ieeeMode=0x3
      floatMode=0xe0, hwLocalSize=51, hwRegion=394, scratchBuffer=9
      uavPrivate=8, uavId=12, constBufferId=11, printfId=10
      privateId=8, earlyExit=1,condOut=2, 
      UserData: 15, 0, 0, 4
      UserData: 28, 0, 4, 1
      UserData: 28, 0, 5, 1
      UserData: 1, 0, 6, 2
  Section .notknown:
      6e6f746b6e6f776e73656374696f6e
  GlobalData:
)ffDXD", "", true
    },
    /* 3 - amd format - errors */
    {
        R"ffDXD(            .amd
            
            .kernel non_config
            .header
            .byte 0,2,3,4,5,65,6,6,8
            
            .config
            
            .boolconsts 4
            .intconsts 55
            .floatconsts 51
            .earlyexit
            .segment 3,4
            .cbid 4,32
            .entry 4,2
            .sampler 21,1
            .cbmask 431,33
            
            .calnote 
            .inputsamplers
            .sampler 3,
            .sampler ,4
            .sampler ,
            .sampler 45
            .uav
            .entry 1,2,3
            .entry 1,2,3,
            .entry 1,2,
            .entry 1,2
            .entry 1,
            .entry 1
            .entry
            .calnote 45,5
            
            .sgprsnum 2
            
            .kernel configuredKernel
            .config
            .metadata
            .header
            .proginfo
            .calnote 34
            .sgprsnum -4
            .sgprsnum 103
            .vgprsnum -4
            .vgprsnum 102
            .vgprsnum 257
            .vgprsnum 256
            .uavid 1024
            .cbid 1024
            .printfid 1024
            .privateid 1024
            .uavprivate 333
            .hwlocal 120303
            .hwlocal
            .sgprsnum
            .vgprsnum
            .earlyexit
            .condout
            .printfid
            .uavid
            .cbid
            .privateid
            .cws
            .cws 1
            .cws 1,
            .cws 1,2
            .cws 1,2,
            
            .userdata 0,0,2,112
            .userdata imm_sampler,0,14,2
            .userdata imm_sampler,0,14,3
            .userdata imm_sampler,0,16,1
            .userdata imm_sampler
            .userdata imm_sampler,0
            .userdata imm_sampler,0,1
            .userdata imm_sampler,0,3,
            
            .arg 
            .arg v0, "test1", t
            .arg v0, ucharx
            .arg v0, uchar*, glocal
            .arg v0, uchar*,, restrict
            .arg v0, uchar*,global, ::
            .arg v0, uchar*,global,,XXX
            .arg v0, uchar*,global,,5,unx
            .arg v0, structure
            .arg v0, structure,
            .arg v0, image, const
            .arg v0, image, local
            .arg v0, counter32, 111
            .arg v0, uchar*,global,,,
            .arg vx, int
            .arg vx, uint
            .arg v0, structure*,global
            .arg v0, structure*,global,const
            .arg v0, ulong*, global,,1024
            .arg v0, image2d,read_only,128
            .arg v0, image2d,write_only,8
            .arg v0, short*, constant,,,160
            .arg v0, short*, local,,,,
            .arg v0, short*, local, const,4
            .arg v0, sampler*, global
            .arg v0, image3d*, global
            .arg v0, sampler*, global
            .arg v0, counter32*, global
            .text
            .sgprsnum 103)ffDXD",
        "",
        /* errors */
        R"ffDXD(test.s:7:13: Error: Config can't be defined if metadata,header and/or CALnotes section exists
test.s:9:25: Error: Garbages at end of line
test.s:10:24: Error: Garbages at end of line
test.s:11:26: Error: Garbages at end of line
test.s:13:13: Error: Illegal place of segment
test.s:14:13: Error: Illegal place of cbid
test.s:15:13: Error: Illegal place of entry
test.s:16:13: Error: Illegal place of sampler
test.s:17:13: Error: Illegal place of cbmask
test.s:19:22: Error: Expected expression
test.s:21:24: Error: Expected expression
test.s:22:22: Error: Expected expression
test.s:23:22: Error: Expected expression
test.s:23:23: Error: Expected expression
test.s:24:24: Error: Expected ',' before argument
test.s:26:25: Error: Expected ',' before argument
test.s:27:26: Error: Expected expression
test.s:28:24: Error: Expected expression
test.s:28:24: Error: Expected ',' before argument
test.s:29:23: Error: Expected ',' before argument
test.s:30:22: Error: Expected expression
test.s:30:22: Error: Expected ',' before argument
test.s:31:21: Error: Expected ',' before argument
test.s:32:19: Error: Expected expression
test.s:32:19: Error: Expected ',' before argument
test.s:33:24: Error: Garbages at end of line
test.s:35:13: Error: Illegal place of configuration pseudo-op
test.s:39:13: Error: Metadata can't be defined if configuration was defined
test.s:40:13: Error: Header can't be defined if configuration was defined
test.s:41:13: Error: CALNote can't be defined if configuration was defined
test.s:42:13: Error: CALNote can't be defined if configuration was defined
test.s:43:13: Error: Used SGPRs number out of range (0-102)
test.s:44:13: Error: Used SGPRs number out of range (0-102)
test.s:45:13: Error: Used VGPRs number out of range (0-256)
test.s:47:13: Error: Used VGPRs number out of range (0-256)
test.s:49:13: Error: UAVId out of range (0-1023)
test.s:50:13: Error: ConstBufferId out of range (0-1023)
test.s:51:13: Error: PrintfId out of range (0-1023)
test.s:52:13: Error: PrivateId out of range (0-1023)
test.s:54:13: Error: HWLocalSize out of range (0-32768)
test.s:55:21: Error: Expected expression
test.s:56:22: Error: Expected expression
test.s:57:22: Error: Expected expression
test.s:58:23: Error: Expected expression
test.s:59:21: Error: Expected expression
test.s:64:17: Error: Expected expression
test.s:70:23: Error: Some garbages at ApiSlot place
test.s:70:27: Error: RegStart+RegSize out of range (0-16)
test.s:72:37: Error: RegStart+RegSize out of range (0-16)
test.s:73:37: Error: RegStart out of range (0-15)
test.s:73:37: Error: RegStart+RegSize out of range (0-16)
test.s:74:34: Error: Expected ',' before argument
test.s:75:36: Error: Expected ',' before argument
test.s:76:38: Error: Expected ',' before argument
test.s:77:39: Error: Expected expression
test.s:79:18: Error: Expected argument name
test.s:79:18: Error: Expected ',' before argument
test.s:80:31: Error: Unknown argument type name
test.s:81:22: Error: Unknown argument type name
test.s:82:30: Error: Unknown or not given pointer space
test.s:83:29: Error: Some garbages at pointer space place
test.s:84:37: Error: Some garbages at access qualifier place
test.s:84:37: Error: Unknown or not given access qualifier type
test.s:85:37: Error: Expression have unresolved symbol 'XXX'
test.s:86:39: Error: This is not 'unused' specifier
test.s:87:31: Error: Expected ',' before argument
test.s:88:32: Error: Expected expression
test.s:89:29: Error: Unknown access qualifier
test.s:90:29: Error: Unknown access qualifier
test.s:91:33: Error: Resource Id out of range (0-7)
test.s:92:38: Error: Expected unused specifier
test.s:92:38: Error: This is not 'unused' specifier
test.s:94:18: Error: Kernel argument 'vx' is already defined
test.s:95:33: Error: Expression have unresolved symbol 'global'
test.s:95:39: Error: Expected ',' before argument
test.s:96:33: Error: Expression have unresolved symbol 'global'
test.s:96:40: Error: Unknown or not given pointer space
test.s:97:38: Error: UAVId out of range (0-1023)
test.s:98:40: Error: Resource Id out of range (0-127)
test.s:99:41: Error: Resource Id out of range (0-7)
test.s:100:41: Error: UAVId out of range (0-159)
test.s:101:37: Error: Some garbages at unused specifier place
test.s:101:37: Error: This is not 'unused' specifier
test.s:102:43: Error: Some garbages at unused specifier place
test.s:102:43: Error: This is not 'unused' specifier
test.s:103:22: Error: Illegal pointer type
test.s:104:22: Error: Illegal pointer type
test.s:105:22: Error: Illegal pointer type
test.s:106:22: Error: Illegal pointer type
test.s:108:13: Error: Illegal place of configuration pseudo-op
)ffDXD", false
    }
};

static void testAssembler(cxuint testId, const AsmTestCase& testCase)
{
    std::istringstream input(testCase.input);
    std::ostringstream errorStream;
    std::ostringstream printStream;
    
    Assembler assembler("test.s", input, ASM_ALL, BinaryFormat::AMD,
            GPUDeviceType::CAPE_VERDE, errorStream, printStream);
    bool good = assembler.assemble();
    
    std::ostringstream dumpOss;
    if (good && assembler.getFormatHandler()!=nullptr)
    {   // get format handler and their output
        if (assembler.getBinaryFormat() == BinaryFormat::AMD)
            printAmdOutput(dumpOss, static_cast<const AsmAmdHandler*>(
                        assembler.getFormatHandler())->getOutput());
        else if (assembler.getBinaryFormat() == BinaryFormat::GALLIUM)
            printGalliumOutput(dumpOss, static_cast<const AsmGalliumHandler*>(
                        assembler.getFormatHandler())->getOutput());
    }
    /* compare results */
    char testName[30];
    snprintf(testName, 30, "Test #%u", testId);
    
    assertValue(testName, "good", int(testCase.good), int(good));
    assertString(testName, "dump", testCase.dump, dumpOss.str());
    assertString(testName, "errorMessages", testCase.errors, errorStream.str());
}

int main(int argc, const char** argv)
{
    int retVal = 0;
    for (size_t i = 0; i < sizeof(asmTestCases1Tbl)/sizeof(AsmTestCase); i++)
        try
        { testAssembler(i, asmTestCases1Tbl[i]); }
        catch(const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
            retVal = 1;
        }
    return retVal;
}

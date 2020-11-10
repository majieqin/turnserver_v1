// Copyright (c) 2014-2017 Agora.io, Inc.
// Reflect Utilities

#pragma once  // NOLINT(build/header_guard)

#include <array>
#include <cstring>
#include <functional>
#include <tuple>
#include <type_traits>

#include "compile_time_utility.h"
#include "const_utility.h"
#include "packet_helper.h"
#include "type_traits.hpp"

namespace agora {
namespace base {
namespace reflect {

#define __MAKE_ARG_LIST_1(op, arg, ...) op(arg)
#define __MAKE_ARG_LIST_2(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_1(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_3(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_2(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_4(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_3(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_5(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_4(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_6(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_5(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_7(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_6(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_8(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_7(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_9(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_8(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_10(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_9(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_11(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_10(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_12(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_11(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_13(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_12(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_14(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_13(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_15(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_14(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_16(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_15(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_17(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_16(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_18(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_17(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_19(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_18(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_20(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_19(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_21(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_20(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_22(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_21(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_23(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_22(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_24(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_23(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_25(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_24(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_26(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_25(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_27(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_26(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_28(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_27(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_29(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_28(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_30(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_29(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_31(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_30(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_32(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_31(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_33(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_32(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_34(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_33(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_35(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_34(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_36(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_35(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_37(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_36(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_38(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_37(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_39(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_38(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_40(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_39(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_41(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_40(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_42(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_41(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_43(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_42(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_44(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_43(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_45(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_44(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_46(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_45(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_47(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_46(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_48(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_47(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_49(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_48(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_50(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_49(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_51(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_50(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_52(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_51(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_53(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_52(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_54(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_53(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_55(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_54(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_56(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_55(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_57(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_56(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_58(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_57(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_59(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_58(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_60(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_59(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_61(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_60(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_62(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_61(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_63(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_62(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_64(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_63(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_65(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_64(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_66(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_65(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_67(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_66(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_68(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_67(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_69(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_68(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_70(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_69(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_71(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_70(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_72(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_71(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_73(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_72(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_74(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_73(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_75(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_74(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_76(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_75(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_77(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_76(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_78(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_77(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_79(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_78(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_80(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_79(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_81(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_80(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_82(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_81(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_83(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_82(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_84(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_83(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_85(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_84(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_86(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_85(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_87(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_86(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_88(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_87(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_89(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_88(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_90(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_89(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_91(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_90(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_92(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_91(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_93(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_92(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_94(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_93(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_95(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_94(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_96(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_95(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_97(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_96(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_98(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_97(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_99(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_98(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_100(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_99(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_101(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_100(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_102(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_101(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_103(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_102(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_104(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_103(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_105(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_104(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_106(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_105(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_107(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_106(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_108(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_107(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_109(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_108(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_110(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_109(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_111(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_110(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_112(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_111(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_113(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_112(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_114(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_113(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_115(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_114(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_116(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_115(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_117(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_116(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_118(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_117(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_119(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_118(op, __VA_ARGS__))
#define __MAKE_ARG_LIST_120(op, arg, ...) \
  op(arg), __MARCO_EXPAND(__MAKE_ARG_LIST_119(op, __VA_ARGS__))

#define __ADD_VIEW(ele) ::agora::base::StringRef(#ele)

#define __SEPERATOR ,
#define __CONST_STR_1(ele, ...) __ADD_VIEW(ele)
#define __CONST_STR_2(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_1(__VA_ARGS__))
#define __CONST_STR_3(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_2(__VA_ARGS__))
#define __CONST_STR_4(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_3(__VA_ARGS__))
#define __CONST_STR_5(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_4(__VA_ARGS__))
#define __CONST_STR_6(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_5(__VA_ARGS__))
#define __CONST_STR_7(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_6(__VA_ARGS__))
#define __CONST_STR_8(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_7(__VA_ARGS__))
#define __CONST_STR_9(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_8(__VA_ARGS__))
#define __CONST_STR_10(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_9(__VA_ARGS__))
#define __CONST_STR_11(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_10(__VA_ARGS__))
#define __CONST_STR_12(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_11(__VA_ARGS__))
#define __CONST_STR_13(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_12(__VA_ARGS__))
#define __CONST_STR_14(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_13(__VA_ARGS__))
#define __CONST_STR_15(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_14(__VA_ARGS__))
#define __CONST_STR_16(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_15(__VA_ARGS__))
#define __CONST_STR_17(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_16(__VA_ARGS__))
#define __CONST_STR_18(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_17(__VA_ARGS__))
#define __CONST_STR_19(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_18(__VA_ARGS__))
#define __CONST_STR_20(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_19(__VA_ARGS__))
#define __CONST_STR_21(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_20(__VA_ARGS__))
#define __CONST_STR_22(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_21(__VA_ARGS__))
#define __CONST_STR_23(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_22(__VA_ARGS__))
#define __CONST_STR_24(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_23(__VA_ARGS__))
#define __CONST_STR_25(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_24(__VA_ARGS__))
#define __CONST_STR_26(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_25(__VA_ARGS__))
#define __CONST_STR_27(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_26(__VA_ARGS__))
#define __CONST_STR_28(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_27(__VA_ARGS__))
#define __CONST_STR_29(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_28(__VA_ARGS__))
#define __CONST_STR_30(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_29(__VA_ARGS__))
#define __CONST_STR_31(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_30(__VA_ARGS__))
#define __CONST_STR_32(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_31(__VA_ARGS__))
#define __CONST_STR_33(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_32(__VA_ARGS__))
#define __CONST_STR_34(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_33(__VA_ARGS__))
#define __CONST_STR_35(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_34(__VA_ARGS__))
#define __CONST_STR_36(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_35(__VA_ARGS__))
#define __CONST_STR_37(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_36(__VA_ARGS__))
#define __CONST_STR_38(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_37(__VA_ARGS__))
#define __CONST_STR_39(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_38(__VA_ARGS__))
#define __CONST_STR_40(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_39(__VA_ARGS__))
#define __CONST_STR_41(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_40(__VA_ARGS__))
#define __CONST_STR_42(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_41(__VA_ARGS__))
#define __CONST_STR_43(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_42(__VA_ARGS__))
#define __CONST_STR_44(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_43(__VA_ARGS__))
#define __CONST_STR_45(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_44(__VA_ARGS__))
#define __CONST_STR_46(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_45(__VA_ARGS__))
#define __CONST_STR_47(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_46(__VA_ARGS__))
#define __CONST_STR_48(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_47(__VA_ARGS__))
#define __CONST_STR_49(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_48(__VA_ARGS__))
#define __CONST_STR_50(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_49(__VA_ARGS__))
#define __CONST_STR_51(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_50(__VA_ARGS__))
#define __CONST_STR_52(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_51(__VA_ARGS__))
#define __CONST_STR_53(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_52(__VA_ARGS__))
#define __CONST_STR_54(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_53(__VA_ARGS__))
#define __CONST_STR_55(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_54(__VA_ARGS__))
#define __CONST_STR_56(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_55(__VA_ARGS__))
#define __CONST_STR_57(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_56(__VA_ARGS__))
#define __CONST_STR_58(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_57(__VA_ARGS__))
#define __CONST_STR_59(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_58(__VA_ARGS__))
#define __CONST_STR_60(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_59(__VA_ARGS__))
#define __CONST_STR_61(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_60(__VA_ARGS__))
#define __CONST_STR_62(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_61(__VA_ARGS__))
#define __CONST_STR_63(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_62(__VA_ARGS__))
#define __CONST_STR_64(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_63(__VA_ARGS__))
#define __CONST_STR_65(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_64(__VA_ARGS__))
#define __CONST_STR_66(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_65(__VA_ARGS__))
#define __CONST_STR_67(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_66(__VA_ARGS__))
#define __CONST_STR_68(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_67(__VA_ARGS__))
#define __CONST_STR_69(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_68(__VA_ARGS__))
#define __CONST_STR_70(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_69(__VA_ARGS__))
#define __CONST_STR_71(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_70(__VA_ARGS__))
#define __CONST_STR_72(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_71(__VA_ARGS__))
#define __CONST_STR_73(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_72(__VA_ARGS__))
#define __CONST_STR_74(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_73(__VA_ARGS__))
#define __CONST_STR_75(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_74(__VA_ARGS__))
#define __CONST_STR_76(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_75(__VA_ARGS__))
#define __CONST_STR_77(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_76(__VA_ARGS__))
#define __CONST_STR_78(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_77(__VA_ARGS__))
#define __CONST_STR_79(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_78(__VA_ARGS__))
#define __CONST_STR_80(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_79(__VA_ARGS__))
#define __CONST_STR_81(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_80(__VA_ARGS__))
#define __CONST_STR_82(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_81(__VA_ARGS__))
#define __CONST_STR_83(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_82(__VA_ARGS__))
#define __CONST_STR_84(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_83(__VA_ARGS__))
#define __CONST_STR_85(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_84(__VA_ARGS__))
#define __CONST_STR_86(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_85(__VA_ARGS__))
#define __CONST_STR_87(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_86(__VA_ARGS__))
#define __CONST_STR_88(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_87(__VA_ARGS__))
#define __CONST_STR_89(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_88(__VA_ARGS__))
#define __CONST_STR_90(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_89(__VA_ARGS__))
#define __CONST_STR_91(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_90(__VA_ARGS__))
#define __CONST_STR_92(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_91(__VA_ARGS__))
#define __CONST_STR_93(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_92(__VA_ARGS__))
#define __CONST_STR_94(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_93(__VA_ARGS__))
#define __CONST_STR_95(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_94(__VA_ARGS__))
#define __CONST_STR_96(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_95(__VA_ARGS__))
#define __CONST_STR_97(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_96(__VA_ARGS__))
#define __CONST_STR_98(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_97(__VA_ARGS__))
#define __CONST_STR_99(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_98(__VA_ARGS__))
#define __CONST_STR_100(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_99(__VA_ARGS__))
#define __CONST_STR_101(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_100(__VA_ARGS__))
#define __CONST_STR_102(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_101(__VA_ARGS__))
#define __CONST_STR_103(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_102(__VA_ARGS__))
#define __CONST_STR_104(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_103(__VA_ARGS__))
#define __CONST_STR_105(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_104(__VA_ARGS__))
#define __CONST_STR_106(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_105(__VA_ARGS__))
#define __CONST_STR_107(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_106(__VA_ARGS__))
#define __CONST_STR_108(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_107(__VA_ARGS__))
#define __CONST_STR_109(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_108(__VA_ARGS__))
#define __CONST_STR_110(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_109(__VA_ARGS__))
#define __CONST_STR_111(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_110(__VA_ARGS__))
#define __CONST_STR_112(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_111(__VA_ARGS__))
#define __CONST_STR_113(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_112(__VA_ARGS__))
#define __CONST_STR_114(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_113(__VA_ARGS__))
#define __CONST_STR_115(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_114(__VA_ARGS__))
#define __CONST_STR_116(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_115(__VA_ARGS__))
#define __CONST_STR_117(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_116(__VA_ARGS__))
#define __CONST_STR_118(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_117(__VA_ARGS__))
#define __CONST_STR_119(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_118(__VA_ARGS__))
#define __CONST_STR_120(ele, ...) \
  __ADD_VIEW(ele) __SEPERATOR __MARCO_EXPAND(__CONST_STR_119(__VA_ARGS__))

#define __RSEQ_N()                                                             \
  119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, 105,   \
      104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, \
      87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70,  \
      69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52,  \
      51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34,  \
      33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,  \
      15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define __ARG_N(                                                               \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16,     \
    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, \
    _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, \
    _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, \
    _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, \
    _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, \
    _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104,      \
    _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116,    \
    _117, _118, _119, N, ...)                                                  \
  N

#define __GET_ARG_COUNT_INNER(...) __MARCO_EXPAND(__ARG_N(__VA_ARGS__))
#define __GET_ARG_COUNT(...) __GET_ARG_COUNT_INNER(__VA_ARGS__, __RSEQ_N())

#define __MARCO_EXPAND(...) __VA_ARGS__
#define __APPLY_VARIADIC_MACRO(macro, ...) __MARCO_EXPAND(macro(__VA_ARGS__))

#define __ADD_REFERENCE(t) std::reference_wrapper<decltype(t)>(t)
#define __ADD_REFERENCE_CONST(t) \
  std::reference_wrapper<typename std::add_const<decltype(t)>::type>(t)

#define __FIELD(t) t
#define __MAKE_NAMES(...) #__VA_ARGS__,

// note use MACRO_CONCAT like A##_##B direct may cause marco expand error
#define __MACRO_CONCAT(A, B) __MACRO_CONCAT1(A, B)
#define __MACRO_CONCAT1(A, B) A##_##B

#define __MAKE_ARG_LIST(N, op, arg, ...) \
  __MACRO_CONCAT(__MAKE_ARG_LIST, N)(op, arg, __VA_ARGS__)

#define __MAKE_STR_LIST(...) \
  __MACRO_CONCAT(__CONST_STR, __GET_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define __MAKE_META_DATA_IMPL(STRUCT_NAME, ...)                             \
  struct __reflect_members_##STRUCT_NAME {                                  \
    constexpr decltype(std::make_tuple(__VA_ARGS__)) static apply_impl() {  \
      return std::make_tuple(__VA_ARGS__);                                  \
    }                                                                       \
    using type = void;                                                      \
    using size_type =                                                       \
        std::integral_constant<uint32_t, __GET_ARG_COUNT(__VA_ARGS__)>;     \
    constexpr static ::agora::base::StringRef Name() {                      \
      return ::agora::base::StringRef(#STRUCT_NAME);                        \
    }                                                                       \
    constexpr static uint32_t Value() { return size_type::value; }          \
    constexpr static std::array<::agora::base::StringRef, size_type::value> \
    Arr() {                                                                 \
      return __arr_##STRUCT_NAME::Value();                                  \
    }                                                                       \
  };                                                                        \
  constexpr __reflect_members_##STRUCT_NAME __reflect_members(              \
      STRUCT_NAME const&) {                                                 \
    return __reflect_members_##STRUCT_NAME{};                               \
  }

#define __MAKE_META_DATA(STRUCT_NAME, N, ...)                             \
  struct __arr_##STRUCT_NAME {                                            \
    constexpr static std::array<::agora::base::StringRef, N> Value() {    \
      return std::array<::agora::base::StringRef, N>{                     \
          {__MARCO_EXPAND(__MACRO_CONCAT(__CONST_STR, N)(__VA_ARGS__))}}; \
    }                                                                     \
  };                                                                      \
  __MAKE_META_DATA_IMPL(                                                  \
      STRUCT_NAME, __MAKE_ARG_LIST(N, &STRUCT_NAME::__FIELD, __VA_ARGS__))

#define __REFLECTION(STRUCT_NAME, ...) \
  __MAKE_META_DATA(STRUCT_NAME, __GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define __REMOVE_HEAD_2(ele1, ele2, ...) __VA_ARGS__
#define __REMOVE_HEAD_1(ele1, ...) __VA_ARGS__
#define __FIRST_ELEMENT(ele, ...) ele
#define __SELECT_MEMBER_NAMES_2(tp, nm, ...) nm
#define __SELECT_MEMBER_NAMES_4(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_2(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_6(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_4(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_8(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_6(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_10(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_8(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_12(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_10(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_14(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_12(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_16(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_14(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_18(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_16(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_20(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_18(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_22(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_20(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_24(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_22(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_26(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_24(__VA_ARGS__))
#define __SELECT_MEMBER_NAMES_28(tp, nm, ...) \
  nm, __MARCO_EXPAND(__SELECT_MEMBER_NAMES_26(__VA_ARGS__))

#define __ARG_N_2_PACKET_N_2 0
#define __ARG_N_2_PACKET_N_4 1
#define __ARG_N_2_PACKET_N_6 2
#define __ARG_N_2_PACKET_N_8 3
#define __ARG_N_2_PACKET_N_10 4
#define __ARG_N_2_PACKET_N_12 5
#define __ARG_N_2_PACKET_N_14 6
#define __ARG_N_2_PACKET_N_16 7
#define __ARG_N_2_PACKET_N_18 8
#define __ARG_N_2_PACKET_N_20 9
#define __ARG_N_2_PACKET_N_22 10
#define __ARG_N_2_PACKET_N_24 11
#define __ARG_N_2_PACKET_N_26 12
#define __ARG_N_2_PACKET_N_28 13
#define __ARG_N_2_PACKET_N_30 14

#define __ARG_N_2_PACKABLE_N_3 1
#define __ARG_N_2_PACKABLE_N_5 2
#define __ARG_N_2_PACKABLE_N_7 3
#define __ARG_N_2_PACKABLE_N_9 4
#define __ARG_N_2_PACKABLE_N_11 5
#define __ARG_N_2_PACKABLE_N_13 6
#define __ARG_N_2_PACKABLE_N_15 7
#define __ARG_N_2_PACKABLE_N_17 8
#define __ARG_N_2_PACKABLE_N_19 9
#define __ARG_N_2_PACKABLE_N_21 10
#define __ARG_N_2_PACKABLE_N_23 11
#define __ARG_N_2_PACKABLE_N_25 12
#define __ARG_N_2_PACKABLE_N_27 13
#define __ARG_N_2_PACKABLE_N_29 14

#define __SELECT_MEMBER_NAMES_IMPL(...)                               \
  __MACRO_CONCAT(__SELECT_MEMBER_NAMES, __GET_ARG_COUNT(__VA_ARGS__)) \
  (__VA_ARGS__)

#define __SELECT_MEMBER_NAMES(...) \
  __SELECT_MEMBER_NAMES_IMPL(__REMOVE_HEAD_2(__VA_ARGS__))

#define __SELECT_PACKABLE_MEMBER_NAMES(...) \
  __SELECT_MEMBER_NAMES_IMPL(__REMOVE_HEAD_1(__VA_ARGS__))

#define R_DECLARE_PACKET(...)                                           \
  __MACRO_CONCAT(                                                       \
      NEW_DECLARE_PACKET,                                               \
      __MACRO_CONCAT(__ARG_N_2_PACKET_N, __GET_ARG_COUNT(__VA_ARGS__))) \
  (__VA_ARGS__) __REFLECTION(__FIRST_ELEMENT(__VA_ARGS__),              \
                             __SELECT_MEMBER_NAMES(__VA_ARGS__))

#define R_DECLARE_PACKABLE(...)                                           \
  __MACRO_CONCAT(                                                         \
      NEW_DECLARE_PACKABLE,                                               \
      __MACRO_CONCAT(__ARG_N_2_PACKABLE_N, __GET_ARG_COUNT(__VA_ARGS__))) \
  (__VA_ARGS__) __REFLECTION(__FIRST_ELEMENT(__VA_ARGS__),                \
                             __SELECT_PACKABLE_MEMBER_NAMES(__VA_ARGS__))

template<typename T>
using ReflectMembers = decltype(__reflect_members(std::declval<T>()));

template<typename T, typename = void>
struct IsReflection : std::false_type {};

template<typename T>
struct IsReflection<T, void_t<typename ReflectMembers<T>::type>>
    : std::true_type {};

template<typename T>
constexpr const ::agora::base::StringRef GetName() {
  return decltype(__reflect_members(std::declval<T>()))::Name();
}

template<typename T>
constexpr const ::agora::base::StringRef GetName(uint32_t i) {
  return static_cast<decltype(
      decltype(__reflect_members(std::declval<T>()))::Arr()) const&>(
      decltype(
          __reflect_members(std::declval<T>()))::Arr())[static_cast<size_t>(i)];
}

template<typename T>
constexpr uint32_t GetMembersCount() {
  return decltype(__reflect_members(std::declval<T>()))::Value();
}

template<typename T>
struct __GET_HELP_M {
  using M = decltype(__reflect_members(std::declval<T>()));
};

template<uint32_t I, typename T>
struct __GET_HELP_U {
  using U = decltype(std::move(std::declval<T>()).*
                     (std::get<I>(__GET_HELP_M<T>::M::apply_impl())));
};

template<uint32_t I, typename T, bool is_array>
struct __GET_IMPL;

template<uint32_t I, typename T>
struct __GET_IMPL<I, T, true> {
  constexpr static auto s(T&& t)
      -> decltype(std::forward<T>(t).*
                  (std::get<I>(__GET_HELP_M<T>::M::apply_impl()))) {
    return std::forward<T>(t).*(std::get<I>(__GET_HELP_M<T>::M::apply_impl()));
  }
  constexpr const static std::array<char, __GET_HELP_U<I, T>::U> arr{};
  constexpr static std::array<char, __GET_HELP_U<I, T>::U> Get(T&& t) {
    return memcpy(arr.data(), s(std::forward<T>(t)), arr.size()), arr;
  }
};

template<uint32_t I, typename T>
struct __GET_IMPL<I, T, false> {
  constexpr static auto Get(T&& t)
      -> decltype(std::forward<T>(t).*
                  (std::get<I>(__GET_HELP_M<T>::M::apply_impl()))) {
    return std::forward<T>(t).*(std::get<I>(__GET_HELP_M<T>::M::apply_impl()));
  }
};

template<uint32_t I, typename T>
constexpr auto Get(T&& t) -> decltype(
    __GET_IMPL<I, T, std::is_array<T>::value>::Get(std::forward<T>(t))) {
  return __GET_IMPL<I, T, std::is_array<T>::value>::Get(std::forward<T>(t));
}

template<typename T, uint32_t... Is>
constexpr auto __GetImpl(T const& t, Sequence<Is...>)
    -> decltype(std::make_tuple(Get<Is>(t)...)) {
  return std::make_tuple(Get<Is>(t)...);
}

template<typename T, uint32_t... Is>
constexpr auto __GetImpl(T const& t, Sequence<Is...>)
    -> decltype(std::make_tuple(std::ref(Get<Is>(t))...)) {
  return std::make_tuple(std::ref(Get<Is>(t))...);
}

template<typename T>
constexpr auto Get(T const& t) -> decltype(
    __GetImpl(t, GenSequence<decltype(__reflect_members(t))::Value()>{})) {
  return __GetImpl(t, GenSequence<decltype(__reflect_members(t))::Value()>{});
}

template<typename T>
constexpr auto GetRef(T& t) -> decltype(
    __GetImpl(t, GenSequence<decltype(__reflect_members(t))::Value()>{})) {
  return __GetImpl(t, GenSequence<decltype(__reflect_members(t))::Value()>{});
}

template<typename T>
constexpr int GetIdIsSame(base::StringRef, Sequence<>) {
  return -1;
}

template<typename T, uint32_t First, uint32_t... Rest>
constexpr int GetIdIsSame(::agora::base::StringRef name,
                          Sequence<First, Rest...>) {
  return name == GetName<T>(First) ? First
                                   : GetIdIsSame<T>(name, Sequence<Rest...>{});
}

template<typename T>
constexpr int GetId(base::StringRef name) {
  return GetIdIsSame<T>(name, GenSequence<GetMembersCount<T>()>{});
}

namespace details {
template<typename T, uint32_t N, uint32_t... I>
constexpr std::array<T, N + 1> AppendArrayImpl(std::array<T, N> arr, T val,
                                               base::Sequence<I...>) {
  return {arr[I]..., val};
}

template<typename T, uint32_t N>
constexpr std::array<T, N + 1> AppendArray(std::array<T, N> arr, T val) {
  return AppendArrayImpl(arr, val, GenSequence<N>());
}
}  // namespace details

template<typename T, uint32_t Id>
struct __RStructFieldMarker {};

#define GEN_TAG(STRUCT_NAME, FIELD_NAME)                       \
  ::agora::base::reflect::__RStructFieldMarker<                \
      STRUCT_NAME, ::agora::base::reflect::GetId<STRUCT_NAME>( \
                       ::agora::base::StringRef(#FIELD_NAME))>

#define R_ATTRIBUTE(STRUCT_NAME, FIELD_NAME, ATTA_TAG)  \
  using __TAG##STRUCT_NAME##FIELD_NAME##ATTA_TAG =      \
      GEN_TAG(STRUCT_NAME, FIELD_NAME);                 \
  ::agora::base::StringRef constexpr __attr_storage(    \
      __TAG##STRUCT_NAME##FIELD_NAME##ATTA_TAG,         \
      ::agora::base::ConstantIndex<COUNTER_READ(        \
          __TAG##STRUCT_NAME##FIELD_NAME##ATTA_TAG)>) { \
    return __ADD_VIEW(ATTA_TAG);                        \
  };                                                    \
  COUNTER_INC(__TAG##STRUCT_NAME##FIELD_NAME##ATTA_TAG)

template<typename T, uint32_t Id>
constexpr bool HasAttributeImpl(__RStructFieldMarker<T, Id>, StringRef,
                                Sequence<>) {
  return false;
}
template<typename T, uint32_t Id, uint32_t First, uint32_t... Rest>
constexpr bool HasAttributeImpl(__RStructFieldMarker<T, Id> m, StringRef name,
                                Sequence<First, Rest...>) {
  return __attr_storage(m, ConstantIndex<First>{}) == name
             ? true
             : HasAttributeImpl(m, name, Sequence<Rest...>{});
}

template<typename T, uint32_t Id>
constexpr bool HasAttribute(__RStructFieldMarker<T, Id> m, StringRef name) {
  return HasAttributeImpl<T>(m, name, GenSequence<COUNTER_READ(decltype(m))>{});
}

constexpr bool HasAttribute(int, StringRef) { return false; }

#define HAS_ATTRIBUTE(STRUCT_NAME, FIELD_NAME, ATTA_TAG)                   \
  ::agora::base::reflect::HasAttribute(GEN_TAG(STRUCT_NAME, FIELD_NAME){}, \
                                       __ADD_VIEW(ATTA_TAG))

}  // namespace reflect
}  // namespace base
}  // namespace agora


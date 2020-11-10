// Copyright (c) 2014-2017 Agora.io, Inc.
// basic macros for packets xmitted on networks
//

#pragma once  // NOLINT(build/header_guard)

#include "packer.h"
#include "unpacker.h"

#define NEW_DECLARE_STRUCT_END };

#define NEW_DECLARE_SIMPLE_STRUCT_1_START(Name, Type1, member1) \
  struct Name { \
    Type1 member1; \
    Name() : member1() {} \
    explicit Name(Type1 value) : member1(value) {} \
    Name(const Name &r) : member1(r.member1) {} \
    Name& operator=(const Name &r) { \
      if (this == &r) return *this; \
      member1 = r.member1; \
      return *this; \
    }

#define NEW_DECLARE_SIMPLE_STRUCT_1(Name, Type1, member1) \
  NEW_DECLARE_SIMPLE_STRUCT_1_START(Name, Type1, member1) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_SIMPLE_STRUCT_2_START(Name, Type1, member1, \
    Type2, member2) \
  struct Name { \
    Type1 member1; \
    Type2 member2; \
    Name() : member1(), member2() {} \
    Name(Type1 v1, Type2 v2) : member1(v1), member2(v2) {} \
    Name(const Name &r) : member1(r.member1), member2(r.member2) {} \
    Name& operator=(const Name &r) { \
      if (this == &r) return *this; \
      member1 = r.member1; \
      member2 = r.member2; \
      return *this; \
    }

#define NEW_DECLARE_SIMPLE_STRUCT_2(Name, Type1, member1, Type2, member2) \
  NEW_DECLARE_SIMPLE_STRUCT_2_START(Name, Type1, member1, Type2, member2) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_SIMPLE_STRUCT_3_START(Name, Type1, member1, \
    Type2, member2, Type3, member3) \
  struct Name { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Name() : member1(), member2(), member3() {} \
    Name(Type1 v1, Type2 v2, Type3 v3) \
        : member1(v1), member2(v2), member3(v3) {} \
    Name(const Name &r) \
        : member1(r.member1), member2(r.member2), member3(r.member3) {} \
    Name& operator=(const Name &r) { \
      if (this == &r) return *this; \
      member1 = r.member1; \
      member2 = r.member2; \
      member3 = r.member3; \
      return *this; \
    }

#define NEW_DECLARE_SIMPLE_STRUCT_3(Name, Type1, member1, Type2, member2, \
    Type3, member3) \
  NEW_DECLARE_SIMPLE_STRUCT_3_START(Name, Type1, member1, Type2, member2, \
    Type3, member3) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_SIMPLE_STRUCT_4_START(Name, Type1, member1, \
    Type2, member2, Type3, member3, Type4, member4) \
  struct Name { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Name() : member1(), member2(), member3(), member4() {} \
    Name(Type1 v1, Type2 v2, Type3 v3, Type4 v4) \
        : member1(v1), member2(v2), member3(v3), member4(v4) {} \
    Name(const Name &r) \
        : member1(r.member1), \
          member2(r.member2), \
          member3(r.member3), \
          member4(r.member4) { \
    } \
    Name& operator=(const Name &r) { \
      if (this == &r) return *this; \
      member1 = r.member1; \
      member2 = r.member2; \
      member3 = r.member3; \
      member4 = r.member4; \
      return *this; \
    }

#define NEW_DECLARE_SIMPLE_STRUCT_4(Name, Type1, member1, Type2, member2, \
    Type3, member3, Type4, member4) \
  NEW_DECLARE_SIMPLE_STRUCT_4_START(Name, Type1, member1, Type2, member2, \
      Type3, member3, Type4, member4) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_SIMPLE_STRUCT_5_START(Name, Type1, member1, \
    Type2, member2, Type3, member3, Type4, member4, Type5, member5) \
  struct Name { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Name() : member1(), member2(), member3(), member4(), member5() {} \
    Name(Type1 v1, Type2 v2, Type3 v3, Type4 v4, Type5 v5) \
        : member1(v1), member2(v2), member3(v3), member4(v4), member5(v5) {} \
    Name(const Name &r) \
        : member1(r.member1), \
          member2(r.member2), \
          member3(r.member3), \
          member4(r.member4), \
          member5(r.member5) { \
    } \
    Name& operator=(const Name &r) { \
      if (this == &r) return *this; \
      member1 = r.member1; \
      member2 = r.member2; \
      member3 = r.member3; \
      member4 = r.member4; \
      member5 = r.member5; \
      return *this; \
    }

#define NEW_DECLARE_SIMPLE_STRUCT_5(Name, Type1, member1, \
    Type2, member2, Type3, member3, Type4, member4, Type5, member5) \
  NEW_DECLARE_SIMPLE_STRUCT_5_START(Name, Type1, member1, \
      Type2, member2, Type3, member3, Type4, member4, Type5, member5) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_SIMPLE_STRUCT_6_START(Name, Type1, member1, \
    Type2, member2, Type3, member3, Type4, member4, Type5, member5, \
    Type6, member6) \
  struct Name { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Name() \
        : member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6() { \
    } \
    Name(Type1 v1, Type2 v2, Type3 v3, Type4 v4, Type5 v5, Type6 v6) \
        : member1(v1), \
          member2(v2), \
          member3(v3), \
          member4(v4), \
          member5(v5), \
          member6(v6) { \
    } \
    Name(const Name &r) \
        : member1(r.member1), \
          member2(r.member2), \
          member3(r.member3), \
          member4(r.member4), \
          member5(r.member5), \
          member6(r.member6) { \
    } \
    Name& operator=(const Name &r) { \
      if (this == &r) return *this; \
      member1 = r.member1; \
      member2 = r.member2; \
      member3 = r.member3; \
      member4 = r.member4; \
      member5 = r.member5; \
      member6 = r.member6; \
      return *this; \
    }

#define NEW_DECLARE_SIMPLE_STRUCT_6(Name, Type1, member1, \
    Type2, member2, Type3, member3, Type4, member4, Type5, member5, \
    Type6, member6) \
  NEW_DECLARE_SIMPLE_STRUCT_6_START(Name, Type1, member1, \
      Type2, member2, Type3, member3, Type4, member4, Type5, member5, \
      Type6, member6) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_SIMPLE_STRUCT_7_START(Name, Type1, member1, \
    Type2, member2, Type3, member3, Type4, member4, Type5, member5, \
    Type6, member6, Type7, member7) \
  struct Name { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Type7 member7; \
    Name() \
        : member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7() { \
    } \
    Name(Type1 v1, Type2 v2, Type3 v3, Type4 v4, Type5 v5, Type6 v6, Type7 v7) \
        : member1(v1), \
          member2(v2), \
          member3(v3), \
          member4(v4), \
          member5(v5), \
          member6(v6), \
          member7(v7) { \
    } \
    Name(const Name &r) \
        : member1(r.member1), \
          member2(r.member2), \
          member3(r.member3), \
          member4(r.member4), \
          member5(r.member5), \
          member6(r.member6), \
          member7(r.member7) { \
    } \
    Name& operator=(const Name &r) { \
      if (this == &r) return *this; \
      member1 = r.member1; \
      member2 = r.member2; \
      member3 = r.member3; \
      member4 = r.member4; \
      member5 = r.member5; \
      member6 = r.member6; \
      member7 = r.member7; \
      return *this; \
    }

#define NEW_DECLARE_SIMPLE_STRUCT_7(Name, Type1, member1, \
    Type2, member2, Type3, member3, Type4, member4, Type5, member5, \
    Type6, member6, Type7, member7) \
  NEW_DECLARE_SIMPLE_STRUCT_7_START(Name, Type1, member1, \
      Type2, member2, Type3, member3, Type4, member4, Type5, member5, \
      Type6, member6, Type7, member7) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_SIMPLE_STRUCT_8_START( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6, Type7, member7, Type8, member8) \
  struct Name { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Type7 member7; \
    Type8 member8; \
    Name() \
        : member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8() { \
    } \
    Name(Type1 v1, Type2 v2, Type3 v3, Type4 v4, Type5 v5, Type6 v6, Type7 v7, \
         Type8 v8) \
        : member1(v1), \
          member2(v2), \
          member3(v3), \
          member4(v4), \
          member5(v5), \
          member6(v6), \
          member7(v7), \
          member8(v8) { \
    } \
    Name(const Name &r) \
        : member1(r.member1), \
          member2(r.member2), \
          member3(r.member3), \
          member4(r.member4), \
          member5(r.member5), \
          member6(r.member6), \
          member7(r.member7), \
          member8(r.member8) { \
    } \
    Name& operator=(const Name &r) { \
      if (this == &r) return *this; \
      member1 = r.member1; \
      member2 = r.member2; \
      member3 = r.member3; \
      member4 = r.member4; \
      member5 = r.member5; \
      member6 = r.member6; \
      member7 = r.member7; \
      member8 = r.member8; \
      return *this; \
    }

#define NEW_DECLARE_SIMPLE_STRUCT_8( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6, Type7, member7, Type8, member8) \
  NEW_DECLARE_SIMPLE_STRUCT_8_START( \
      Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
      Type5, member5, Type6, member6, Type7, member7, Type8, member8) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_SIMPLE_STRUCT_9_START( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6, Type7, member7, Type8, member8, \
    Type9, member9) \
  struct Name { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Type7 member7; \
    Type8 member8; \
    Type9 member9; \
    Name() \
        : member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9() { \
    } \
    Name(Type1 v1, Type2 v2, Type3 v3, Type4 v4, Type5 v5, Type6 v6, Type7 v7, \
         Type8 v8, Type9 v9) \
        : member1(v1), \
          member2(v2), \
          member3(v3), \
          member4(v4), \
          member5(v5), \
          member6(v6), \
          member7(v7), \
          member8(v8), \
          member9(v9) { \
    } \
    Name(const Name &r) \
        : member1(r.member1), \
          member2(r.member2), \
          member3(r.member3), \
          member4(r.member4), \
          member5(r.member5), \
          member6(r.member6), \
          member7(r.member7), \
          member8(r.member8), \
          member9(r.member9) { \
    } \
    Name& operator=(const Name &r) { \
      if (this == &r) return *this; \
      member1 = r.member1; \
      member2 = r.member2; \
      member3 = r.member3; \
      member4 = r.member4; \
      member5 = r.member5; \
      member6 = r.member6; \
      member7 = r.member7; \
      member8 = r.member8; \
      member9 = r.member9; \
      return *this; \
    }

#define NEW_DECLARE_SIMPLE_STRUCT_9( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6, Type7, member7, Type8, member8, \
    Type9, member9) \
  NEW_DECLARE_SIMPLE_STRUCT_9_START( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6, Type7, member7, Type8, member8, \
    Type9, member9) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_STRUCT_1_START(Name, Type1, member1) \
  NEW_DECLARE_SIMPLE_STRUCT_1_START(Name, Type1, member1) \
  friend bool operator<(const Name &l, const Name &r) { \
    return l.member1 < r.member1; \
  } \
  friend bool operator==(const Name &l, const Name &r) { \
    return l.member1 == r.member1; \
  } \
  friend bool operator!=(const Name &l, const Name &r) { \
    return !(l == r); \
  }

#define NEW_DECLARE_STRUCT_1(Name, Type1, member1) \
  NEW_DECLARE_STRUCT_1_START(Name, Type1, member1) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_STRUCT_2_START(Name, Type1, member1, Type2, member2) \
  NEW_DECLARE_SIMPLE_STRUCT_2_START(Name, Type1, member1, Type2, member2) \
  friend bool operator<(const Name &l, const Name &r) { \
    return l.member1 < r.member1 || ((l.member1 == r.member1) && \
        (l.member2 < r.member2)); \
  } \
  friend bool operator==(const Name &l, const Name &r) { \
    return l.member1 == r.member1 && l.member2 == r.member2; \
  } \
  friend bool operator!=(const Name &l, const Name &r) { \
    return !(l == r); \
  }

#define NEW_DECLARE_STRUCT_2(Name, Type1, member1, Type2, member2) \
  NEW_DECLARE_STRUCT_2_START(Name, Type1, member1, Type2, member2) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_STRUCT_3_START(Name, Type1, member1, Type2, member2, \
                                   Type3, member3) \
  NEW_DECLARE_SIMPLE_STRUCT_3_START(Name, Type1, member1, Type2, member2, \
                                    Type3, member3) \
  friend bool operator<(const Name &l, const Name &r) { \
    return (l.member1 < r.member1) || ((l.member1 == r.member1) && \
        (l.member2 < r.member2)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 < r.member3)); \
  } \
  friend bool operator==(const Name &l, const Name &r) { \
    return l.member1 == r.member1 && l.member2 == r.member2 && \
        l.member3 == r.member3; \
  } \
  friend bool operator!=(const Name &l, const Name &r) { \
    return !(l == r); \
  }

#define NEW_DECLARE_STRUCT_3(Name, Type1, member1, Type2, member2, \
                             Type3, member3) \
  NEW_DECLARE_STRUCT_3_START(Name, Type1, member1, Type2, member2, \
                             Type3, member3) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_STRUCT_4_START(Name, Type1, member1, Type2, member2, \
                                   Type3, member3, Type4, member4) \
  NEW_DECLARE_SIMPLE_STRUCT_4_START(Name, Type1, member1, Type2, member2, \
                                    Type3, member3, Type4, member4) \
  friend bool operator<(const Name &l, const Name &r) { \
    return (l.member1 < r.member1) || ((l.member1 == r.member1) && \
        (l.member2 < r.member2)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 < r.member3)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 < r.member4)); \
  } \
  friend bool operator==(const Name &l, const Name &r) { \
    return (l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4); \
  } \
  friend bool operator!=(const Name &l, const Name &r) { \
    return !(l == r); \
  }

#define NEW_DECLARE_STRUCT_4(Name, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4) \
  NEW_DECLARE_STRUCT_4_START(Name, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_STRUCT_5_START( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5) \
  NEW_DECLARE_SIMPLE_STRUCT_5_START( \
      Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
      Type5, member5) \
  friend bool operator<(const Name &l, const Name &r) { \
    return (l.member1 < r.member1) || ((l.member1 == r.member1) && \
        (l.member2 < r.member2)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 < r.member3)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 < r.member4)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 < r.member5)); \
  } \
  friend bool operator==(const Name &l, const Name &r) { \
    return (l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 == r.member5); \
  } \
  friend bool operator!=(const Name &l, const Name &r) { \
    return !(l == r); \
  }

#define NEW_DECLARE_STRUCT_5( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5) \
  NEW_DECLARE_STRUCT_5_START( \
      Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
      Type5, member5) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_STRUCT_6_START( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6) \
  NEW_DECLARE_SIMPLE_STRUCT_6_START( \
      Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
      Type5, member5, Type6, member6) \
  friend bool operator<(const Name &l, const Name &r) { \
    return (l.member1 < r.member1) || ((l.member1 == r.member1) && \
        (l.member2 < r.member2)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 < r.member3)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 < r.member4)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 < r.member5)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 == r.member3) && \
        (l.member4 == r.member4) && (l.member5 == r.member5) && \
        (l.member6 < r.member6)); \
  } \
  friend bool operator==(const Name &l, const Name &r) { \
    return (l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 == r.member5) && (l.member6 == r.member6); \
  } \
  friend bool operator!=(const Name &l, const Name &r) { \
    return !(l == r); \
  }

#define NEW_DECLARE_STRUCT_6( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6) \
  NEW_DECLARE_STRUCT_6_START( \
      Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
      Type5, member5, Type6, member6) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_STRUCT_7_START( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6, Type7, member7) \
  NEW_DECLARE_SIMPLE_STRUCT_7_START( \
      Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
      Type5, member5, Type6, member6, Type7, member7) \
  friend bool operator<(const Name &l, const Name &r) { \
    return (l.member1 < r.member1) || ((l.member1 == r.member1) && \
        (l.member2 < r.member2)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 < r.member3)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 < r.member4)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 < r.member5)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 == r.member3) && \
        (l.member4 == r.member4) && (l.member5 == r.member5) && \
        (l.member6 < r.member6)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 == r.member3) && \
        (l.member4 == r.member4) && (l.member5 == r.member5) && \
        (l.member6 == r.member6) && (l.member7 < r.member7)); \
  } \
  friend bool operator==(const Name &l, const Name &r) { \
    return (l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 == r.member5) && (l.member6 == r.member6) && \
        (l.member7 == r.member7); \
  } \
  friend bool operator!=(const Name &l, const Name &r) { \
    return !(l == r); \
  }

#define NEW_DECLARE_STRUCT_7( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6, Type7, member7) \
  NEW_DECLARE_STRUCT_7_START( \
      Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
      Type5, member5, Type6, member6, Type7, member7) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_STRUCT_8_START( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6, Type7, member7, Type8, member8) \
  NEW_DECLARE_SIMPLE_STRUCT_8_START( \
      Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
      Type5, member5, Type6, member6, Type7, member7, Type8, member8) \
  friend bool operator<(const Name &l, const Name &r) { \
    return (l.member1 < r.member1) || ((l.member1 == r.member1) && \
        (l.member2 < r.member2)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 < r.member3)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 < r.member4)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 < r.member5)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 == r.member3) && \
        (l.member4 == r.member4) && (l.member5 == r.member5) && \
        (l.member6 < r.member6)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 == r.member3) && \
        (l.member4 == r.member4) && (l.member5 == r.member5) && \
        (l.member6 == r.member6) && (l.member7 < r.member7)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 == r.member5) && (l.member6 == r.member6) && \
        (l.member7 == r.member7) && (l.member8 < r.member8)); \
  } \
  friend bool operator==(const Name &l, const Name &r) { \
    return (l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 == r.member5) && (l.member6 == r.member6) && \
        (l.member7 == r.member7) && (l.member8 == r.member8); \
  } \
  friend bool operator!=(const Name &l, const Name &r) { \
    return !(l == r); \
  }

#define NEW_DECLARE_STRUCT_8( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6, Type7, member7, Type8, member8) \
  NEW_DECLARE_STRUCT_8_START( \
      Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
      Type5, member5, Type6, member6, Type7, member7, Type8, member8) \
  NEW_DECLARE_STRUCT_END

#define NEW_DECLARE_STRUCT_9_START( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6, Type7, member7, Type8, member8, \
    Type9, member9) \
  NEW_DECLARE_SIMPLE_STRUCT_9_START( \
      Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
      Type5, member5, Type6, member6, Type7, member7, Type8, member8, \
      Type9, member9) \
  friend bool operator<(const Name &l, const Name &r) { \
    return (l.member1 < r.member1) || ((l.member1 == r.member1) && \
        (l.member2 < r.member2)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 < r.member3)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 < r.member4)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 < r.member5)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 == r.member3) && \
        (l.member4 == r.member4) && (l.member5 == r.member5) && \
        (l.member6 < r.member6)) || ((l.member1 == r.member1) && \
        (l.member2 == r.member2) && (l.member3 == r.member3) && \
        (l.member4 == r.member4) && (l.member5 == r.member5) && \
        (l.member6 == r.member6) && (l.member7 < r.member7)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 == r.member5) && (l.member6 == r.member6) && \
        (l.member7 == r.member7) && (l.member8 < r.member8)) || \
        ((l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 == r.member5) && (l.member6 == r.member6) && \
        (l.member7 == r.member7) && (l.member8 == r.member8) && \
        (l.member9 < r.member9)); \
  } \
  friend bool operator==(const Name &l, const Name &r) { \
    return (l.member1 == r.member1) && (l.member2 == r.member2) && \
        (l.member3 == r.member3) && (l.member4 == r.member4) && \
        (l.member5 == r.member5) && (l.member6 == r.member6) && \
        (l.member7 == r.member7) && (l.member8 == r.member8) && \
        (l.member9 == r.member9); \
  } \
  friend bool operator!=(const Name &l, const Name &r) { \
    return !(l == r); \
  }

#define NEW_DECLARE_STRUCT_9( \
    Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6, Type7, member7, Type8, member8, \
    Type9, member9) \
  NEW_DECLARE_STRUCT_9_START( \
      Name, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
      Type5, member5, Type6, member6, Type7, member7, Type8, member8, \
      Type9, member9) \
  NEW_DECLARE_STRUCT_END


// NOTE(liuyong): Following macros help to build up a serializable class
//
#define NEW_DECLARE_PACKABLE_1_START(Name, Type1, member1) \
  NEW_DECLARE_STRUCT_1_START(Name, Type1, member1) \
  friend agora::base::Packer& operator<<(agora::base::Packer &p, \
                                         const Name &x) {  \
    p << x.member1; \
    return p; \
  } \
  friend agora::base::Unpacker& operator>>(agora::base::Unpacker &p, \
                                           Name &x) { \
    p >> x.member1; \
    return p; \
  }

#define NEW_DECLARE_PACKABLE_1(Name, Type1, member1) \
  NEW_DECLARE_PACKABLE_1_START(Name, Type1, member1) \
  NEW_DECLARE_STRUCT_END


#define NEW_DECLARE_PACKABLE_2_START(Name, Type1, member1, Type2, member2) \
  NEW_DECLARE_STRUCT_2_START(Name, Type1, member1, Type2, member2) \
  friend agora::base::Packer& operator<<(agora::base::Packer &p, \
                                         const Name &x) {  \
    p << x.member1 << x.member2; \
    return p; \
  } \
  friend agora::base::Unpacker& operator>>(agora::base::Unpacker &p, \
                                           Name &x) { \
    p >> x.member1 >> x.member2; \
    return p; \
  }

#define NEW_DECLARE_PACKABLE_2(Name, Type1, member1, Type2, member2) \
  NEW_DECLARE_PACKABLE_2_START(Name, Type1, member1, Type2, member2) \
  NEW_DECLARE_STRUCT_END


#define NEW_DECLARE_PACKABLE_3_START(Name, Type1, member1, Type2, member2, \
                                     Type3, member3) \
  NEW_DECLARE_STRUCT_3_START(Name, Type1, member1, Type2, member2, \
                             Type3, member3) \
  friend agora::base::Packer& operator<<(agora::base::Packer &p, \
                                         const Name &x) {  \
    p << x.member1 << x.member2 << x.member3; \
    return p; \
  } \
  friend agora::base::Unpacker& operator>>(agora::base::Unpacker &p, \
                                           Name &x) { \
    p >> x.member1 >> x.member2 >> x.member3; \
    return p; \
  }

#define NEW_DECLARE_PACKABLE_3(Name, Type1, member1, Type2, member2, \
                               Type3, member3) \
  NEW_DECLARE_PACKABLE_3_START(Name, Type1, member1, Type2, member2, \
                               Type3, member3) \
  NEW_DECLARE_STRUCT_END


#define NEW_DECLARE_PACKABLE_4_START(Name, Type1, member1, Type2, member2, \
                                     Type3, member3, Type4, member4) \
  NEW_DECLARE_STRUCT_4_START(Name, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4) \
  friend agora::base::Packer& operator<<(agora::base::Packer &p, \
                                         const Name &x) {  \
    p << x.member1 << x.member2 << x.member3 << x.member4; \
    return p; \
  } \
  friend agora::base::Unpacker& operator>>(agora::base::Unpacker &p, \
                                           Name &x) { \
    p >> x.member1 >> x.member2 >> x.member3 >> x.member4; \
    return p; \
  }

#define NEW_DECLARE_PACKABLE_4(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4) \
  NEW_DECLARE_PACKABLE_4_START(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4) \
  NEW_DECLARE_STRUCT_END


#define NEW_DECLARE_PACKABLE_5_START(Name, Type1, member1, Type2, member2, \
                                     Type3, member3, Type4, member4, \
                                     Type5, member5) \
  NEW_DECLARE_STRUCT_5_START(Name, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4, \
                             Type5, member5) \
  friend agora::base::Packer& operator<<(agora::base::Packer &p, \
                                         const Name &x) {  \
    p << x.member1 << x.member2 << x.member3 << x.member4 << x.member5; \
    return p; \
  } \
  friend agora::base::Unpacker& operator>>(agora::base::Unpacker &p, \
                                           Name &x) { \
    p >> x.member1 >> x.member2 >> x.member3 >> x.member4 >> x.member5; \
    return p; \
  }

#define NEW_DECLARE_PACKABLE_5(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4, \
                               Type5, member5) \
  NEW_DECLARE_PACKABLE_5_START(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4, \
                               Type5, member5) \
  NEW_DECLARE_STRUCT_END


#define NEW_DECLARE_PACKABLE_6_START(Name, Type1, member1, Type2, member2, \
                                     Type3, member3, Type4, member4, \
                                     Type5, member5, Type6, member6) \
  NEW_DECLARE_STRUCT_6_START(Name, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4, \
                             Type5, member5, Type6, member6) \
  friend agora::base::Packer& operator<<(agora::base::Packer &p, \
                                         const Name &x) {  \
    p << x.member1 << x.member2 << x.member3 << x.member4 << x.member5 \
      << x.member6; \
    return p; \
  } \
  friend agora::base::Unpacker& operator>>(agora::base::Unpacker &p, \
                                           Name &x) { \
    p >> x.member1 >> x.member2 >> x.member3 >> x.member4 >> x.member5 \
      >> x.member6; \
    return p; \
  }

#define NEW_DECLARE_PACKABLE_6(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4, \
                               Type5, member5, Type6, member6) \
  NEW_DECLARE_PACKABLE_6_START(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4, \
                               Type5, member5, Type6, member6) \
  NEW_DECLARE_STRUCT_END


#define NEW_DECLARE_PACKABLE_7_START(Name, Type1, member1, Type2, member2, \
                                     Type3, member3, Type4, member4, \
                                     Type5, member5, Type6, member6, \
                                     Type7, member7) \
  NEW_DECLARE_STRUCT_7_START(Name, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4, \
                             Type5, member5, Type6, member6, \
                             Type7, member7) \
  friend agora::base::Packer& operator<<(agora::base::Packer &p, \
                                         const Name &x) {  \
    p << x.member1 << x.member2 << x.member3 << x.member4 << x.member5 \
      << x.member6 << x.member7; \
    return p; \
  } \
  friend agora::base::Unpacker& operator>>(agora::base::Unpacker &p, \
                                           Name &x) { \
    p >> x.member1 >> x.member2 >> x.member3 >> x.member4 >> x.member5 \
      >> x.member6 >> x.member7; \
    return p; \
  }

#define NEW_DECLARE_PACKABLE_7(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4, \
                               Type5, member5, Type6, member6, \
                               Type7, member7) \
  NEW_DECLARE_PACKABLE_7_START(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4, \
                               Type5, member5, Type6, member6, \
                               Type7, member7) \
  NEW_DECLARE_STRUCT_END


#define NEW_DECLARE_PACKABLE_8_START(Name, Type1, member1, Type2, member2, \
                                     Type3, member3, Type4, member4, \
                                     Type5, member5, Type6, member6, \
                                     Type7, member7, Type8, member8) \
  NEW_DECLARE_STRUCT_8_START(Name, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4, \
                             Type5, member5, Type6, member6, \
                             Type7, member7, Type8, member8) \
  friend agora::base::Packer& operator<<(agora::base::Packer &p, \
                                         const Name &x) {  \
    p << x.member1 << x.member2 << x.member3 << x.member4 << x.member5 \
      << x.member6 << x.member7 << x.member8; \
    return p; \
  } \
  friend agora::base::Unpacker& operator>>(agora::base::Unpacker &p, \
                                           Name &x) { \
    p >> x.member1 >> x.member2 >> x.member3 >> x.member4 >> x.member5 \
      >> x.member6 >> x.member7 >> x.member8; \
    return p; \
  }

#define NEW_DECLARE_PACKABLE_8(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4, \
                               Type5, member5, Type6, member6, \
                               Type7, member7, Type8, member8) \
  NEW_DECLARE_PACKABLE_8_START(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4, \
                               Type5, member5, Type6, member6, \
                               Type7, member7, Type8, member8) \
  NEW_DECLARE_STRUCT_END


#define NEW_DECLARE_PACKABLE_9_START(Name, Type1, member1, Type2, member2, \
                                     Type3, member3, Type4, member4, \
                                     Type5, member5, Type6, member6, \
                                     Type7, member7, Type8, member8, \
                                     Type9, member9) \
  NEW_DECLARE_STRUCT_9_START(Name, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4, \
                             Type5, member5, Type6, member6, \
                             Type7, member7, Type8, member8, \
                             Type9, member9) \
  friend agora::base::Packer& operator<<(agora::base::Packer &p, \
                                         const Name &x) {  \
    p << x.member1 << x.member2 << x.member3 << x.member4 << x.member5 \
      << x.member6 << x.member7 << x.member8 << x.member9; \
    return p; \
  } \
  friend agora::base::Unpacker& operator>>(agora::base::Unpacker &p, \
                                           Name &x) { \
    p >> x.member1 >> x.member2 >> x.member3 >> x.member4 >> x.member5 \
      >> x.member6 >> x.member7 >> x.member8 >> x.member9; \
    return p; \
  }

#define NEW_DECLARE_PACKABLE_9(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4, \
                               Type5, member5, Type6, member6, \
                               Type7, member7, Type8, member8, \
                               Type9, member9) \
  NEW_DECLARE_PACKABLE_9_START(Name, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4, \
                               Type5, member5, Type6, member6, \
                               Type7, member7, Type8, member8, \
                               Type9, member9) \
  NEW_DECLARE_STRUCT_END

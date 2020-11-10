// Copyright (c) 2014-2017 Agora.io, Inc.
// Base class for packing & unpacking protocols xmitted through networks
//

#pragma once  // NOLINT(build/header_guard)

#include "packer.h"
#include "packet.h"

#define NEW_DECLARE_PACKET_END  };

#define NEW_DECLARE_PACKET_0(Name, uri) \
  struct Name : agora::base::Packet { \
    Name() : agora::base::Packet(uri) {} \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri) { \
    } \
  };

#define NEW_DECLARE_PACKET_1(Name, uri, Type1, member1) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Name() : agora::base::Packet(uri), member1() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), member1() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1; \
    } \
  };

#define NEW_DECLARE_PACKET_2(Name, uri, Type1, member1, Type2, member2) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Name() : agora::base::Packet(uri), member1(), member2() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), member1(), member2() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2; \
    } \
  };

#define NEW_DECLARE_PACKET_3(Name, uri, Type1, member1, Type2, member2, \
                             Type3, member3) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Name() : agora::base::Packet(uri), member1(), member2(), member3() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3; \
    } \
  };

#define NEW_DECLARE_PACKET_4(Name, uri, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Name() \
        : agora::base::Packet(uri), \
          member1(), \
          member2(), \
          member3(), \
          member4() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3(), \
          member4() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3 >> member4; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3 << member4; \
    } \
  };

#define NEW_DECLARE_PACKET_5(Name, uri, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4, Type5, member5) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Name() \
        : agora::base::Packet(uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3 >> member4 >> member5; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3 << member4 << member5; \
    } \
  };

#define NEW_DECLARE_PACKET_6_START(Name, \
    uri, Type1, member1, Type2, member2, Type3, member3, Type4, member4, \
    Type5, member5, Type6, member6) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Name() \
        : agora::base::Packet(uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3 >> member4 >> member5 \
          >> member6; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3 << member4 << member5 \
          << member6; \
    }

#define NEW_DECLARE_PACKET_6(Name, uri, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4, Type5, member5, \
                             Type6, member6) \
    NEW_DECLARE_PACKET_6_START(Name, uri, Type1, member1, Type2, member2, \
                               Type3, member3, Type4, member4, \
                               Type5, member5, Type6, member6) \
    NEW_DECLARE_PACKET_END

#define NEW_DECLARE_PACKET_7(Name, uri, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4, Type5, member5, \
                             Type6, member6, Type7, member7) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Type7 member7; \
    Name() \
        : agora::base::Packet(uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3 >> member4 >> member5 \
          >> member6 >> member7; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3 << member4 << member5 \
          << member6 << member7; \
    } \
  };

#define NEW_DECLARE_PACKET_8(Name, uri, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4, Type5, member5, \
                             Type6, member6, Type7, member7, Type8, member8) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Type7 member7; \
    Type8 member8; \
    Name() \
        : agora::base::Packet(uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3 >> member4 >> member5 \
          >> member6 >> member7 >> member8; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3 << member4 << member5 \
          << member6 << member7 << member8; \
    } \
  };

#define NEW_DECLARE_PACKET_9(Name, uri, Type1, member1, Type2, member2, \
                             Type3, member3, Type4, member4, Type5, member5, \
                             Type6, member6, Type7, member7, Type8, member8, \
                             Type9, member9) \
  struct Name : agora::base::Packet { \
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
        : agora::base::Packet(uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3 >> member4 >> member5 \
          >> member6 >> member7 >> member8 >> member9; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3 << member4 << member5 \
          << member6 << member7 << member8 << member9; \
    } \
  };

#define NEW_DECLARE_PACKET_10(Name, uri, Type1, member1, Type2, member2, \
                              Type3, member3, Type4, member4, Type5, member5, \
                              Type6, member6, Type7, member7, Type8, member8, \
                              Type9, member9, Type10, member10) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Type7 member7; \
    Type8 member8; \
    Type9 member9; \
    Type10 member10; \
    Name() \
        : agora::base::Packet(uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9(), \
          member10() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9(), \
          member10() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3 >> member4 >> member5 \
          >> member6 >> member7 >> member8 >> member9 >> member10; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3 << member4 << member5 \
          << member6 << member7 << member8 << member9 << member10; \
    } \
  };

#define NEW_DECLARE_PACKET_11(Name, uri, Type1, member1, Type2, member2, \
                              Type3, member3, Type4, member4, Type5, member5, \
                              Type6, member6, Type7, member7, Type8, member8, \
                              Type9, member9, Type10, member10, \
                              Type11, member11) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Type7 member7; \
    Type8 member8; \
    Type9 member9; \
    Type10 member10; \
    Type11 member11; \
    Name() \
        : agora::base::Packet(uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9(), \
          member10(), \
          member11() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9(), \
          member10(), \
          member11() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3 >> member4 >> member5 \
          >> member6 >> member7 >> member8 >> member9 >> member10 >> member11; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3 << member4 << member5 \
          << member6 << member7 << member8 << member9 << member10 << member11; \
    } \
  };

#define NEW_DECLARE_PACKET_12(Name, uri, Type1, member1, Type2, member2, \
                              Type3, member3, Type4, member4, Type5, member5, \
                              Type6, member6, Type7, member7, Type8, member8, \
                              Type9, member9, Type10, member10, \
                              Type11, member11, Type12, member12) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Type7 member7; \
    Type8 member8; \
    Type9 member9; \
    Type10 member10; \
    Type11 member11; \
    Type12 member12; \
    Name() \
        : agora::base::Packet(uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9(), \
          member10(), \
          member11(), \
          member12() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9(), \
          member10(), \
          member11(), \
          member12() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3 >> member4 >> member5 \
         >> member6 >> member7 >> member8 >> member9 >> member10 >> member11 \
         >> member12; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3 << member4 << member5 \
         << member6 << member7 << member8 << member9 << member10 << member11 \
         << member12; \
    } \
  };

#define NEW_DECLARE_PACKET_13(Name, uri, Type1, member1, Type2, member2, \
                              Type3, member3, Type4, member4, Type5, member5, \
                              Type6, member6, Type7, member7, Type8, member8, \
                              Type9, member9, Type10, member10, \
                              Type11, member11, Type12, member12, \
                              Type13, member13) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Type7 member7; \
    Type8 member8; \
    Type9 member9; \
    Type10 member10; \
    Type11 member11; \
    Type12 member12; \
    Type13 member13; \
    Name() \
        : agora::base::Packet(uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9(), \
          member10(), \
          member11(), \
          member12(), \
          member13() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9(), \
          member10(), \
          member11(), \
          member12(), \
          member13() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3 >> member4 >> member5 \
         >> member6 >> member7 >> member8 >> member9 >> member10 >> member11 \
         >> member12 >> member13; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3 << member4 << member5 \
         << member6 << member7 << member8 << member9 << member10 << member11 \
         << member12 << member13; \
    } \
  };

#define NEW_DECLARE_PACKET_14(Name, uri, Type1, member1, Type2, member2, \
                              Type3, member3, Type4, member4, Type5, member5, \
                              Type6, member6, Type7, member7, Type8, member8, \
                              Type9, member9, Type10, member10, \
                              Type11, member11, Type12, member12, \
                              Type13, member13, Type14, member14) \
  struct Name : agora::base::Packet { \
    Type1 member1; \
    Type2 member2; \
    Type3 member3; \
    Type4 member4; \
    Type5 member5; \
    Type6 member6; \
    Type7 member7; \
    Type8 member8; \
    Type9 member9; \
    Type10 member10; \
    Type11 member11; \
    Type12 member12; \
    Type13 member13; \
    Type14 member14; \
    Name() \
        : agora::base::Packet(uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9(), \
          member10(), \
          member11(), \
          member12(), \
          member13(), \
          member14() { \
    } \
    explicit Name(uint16_t server_type) \
        : agora::base::Packet(server_type, uri), \
          member1(), \
          member2(), \
          member3(), \
          member4(), \
          member5(), \
          member6(), \
          member7(), \
          member8(), \
          member9(), \
          member10(), \
          member11(), \
          member12(), \
          member13(), \
          member14() { \
    } \
    virtual void Unmarshall(agora::base::Unpacker *p) { \
      agora::base::Packet::Unmarshall(p); \
      *p >> member1 >> member2 >> member3 >> member4 >> member5 \
         >> member6 >> member7 >> member8 >> member9 >> member10 >> member11 \
         >> member12 >> member13 >> member14; \
    } \
    virtual void Marshall(agora::base::Packer *p) const { \
      agora::base::Packet::Marshall(p); \
      *p << member1 << member2 << member3 << member4 << member5 \
         << member6 << member7 << member8 << member9 << member10 << member11 \
         << member12 << member13 << member14; \
    } \
  };


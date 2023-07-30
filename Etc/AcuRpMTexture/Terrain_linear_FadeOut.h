#if 0
    vs_1_1
    dcl_position v0
    dcl_normal v1
    dcl_color v2
    dcl_texcoord v3
    dcl_texcoord1 v4
    dcl_texcoord2 v5
    dcl_texcoord3 v6
    dcl_texcoord4 v7
    dcl_texcoord5 v8
    def c14, 1, 0, 0.5, 0
    m4x4 oPos, v0, c0
    dp4 r0.x, v0, c15
    mad oFog, -r0.x, c7.y, c7.x
    mov oT0.xy, v3
    mov oT1.xy, v4
    mov oT2.xy, v5
    mov oT3.xy, v6
    mov oT4.xy, v7
    mov oT5.xy, v8
    dp3 r1.x, c4, v1
    max r1.x, c14.y, -r1.x
    mul r3, c5, r1.x
    mad oD0, r3, v2, c6
    mul r0, v0.y, c18
    mad r0, c17, v0.x, r0
    mad r0, c19, v0.z, r0
    mad r1, c20, v0.w, r0
    dp4 r1.w, r1, r1
    rsq r2.w, r1.w
    mul r1.w, r2.w, r1.w
    mad r1.w, r1.w, c16.x, -c16.y
    add oD0.w, -r1.w, c14.x

// approximately 25 instruction slots used
#endif

const DWORD g_vs11_terrain_LinearFO[] =
{
    0xfffe0101, 0x0000001f, 0x80000000, 0x900f0000, 0x0000001f, 0x80000003, 
    0x900f0001, 0x0000001f, 0x8000000a, 0x900f0002, 0x0000001f, 0x80000005, 
    0x900f0003, 0x0000001f, 0x80010005, 0x900f0004, 0x0000001f, 0x80020005, 
    0x900f0005, 0x0000001f, 0x80030005, 0x900f0006, 0x0000001f, 0x80040005, 
    0x900f0007, 0x0000001f, 0x80050005, 0x900f0008, 0x00000051, 0xa00f000e, 
    0x3f800000, 0x00000000, 0x3f000000, 0x00000000, 0x00000014, 0xc00f0000, 
    0x90e40000, 0xa0e40000, 0x00000009, 0x80010000, 0x90e40000, 0xa0e4000f, 
    0x00000004, 0xc00f0001, 0x81000000, 0xa0550007, 0xa0000007, 0x00000001, 
    0xe0030000, 0x90e40003, 0x00000001, 0xe0030001, 0x90e40004, 0x00000001, 
    0xe0030002, 0x90e40005, 0x00000001, 0xe0030003, 0x90e40006, 0x00000001, 
    0xe0030004, 0x90e40007, 0x00000001, 0xe0030005, 0x90e40008, 0x00000008, 
    0x80010001, 0xa0e40004, 0x90e40001, 0x0000000b, 0x80010001, 0xa055000e, 
    0x81000001, 0x00000005, 0x800f0003, 0xa0e40005, 0x80000001, 0x00000004, 
    0xd00f0000, 0x80e40003, 0x90e40002, 0xa0e40006, 0x00000005, 0x800f0000, 
    0x90550000, 0xa0e40012, 0x00000004, 0x800f0000, 0xa0e40011, 0x90000000, 
    0x80e40000, 0x00000004, 0x800f0000, 0xa0e40013, 0x90aa0000, 0x80e40000, 
    0x00000004, 0x800f0001, 0xa0e40014, 0x90ff0000, 0x80e40000, 0x00000009, 
    0x80080001, 0x80e40001, 0x80e40001, 0x00000007, 0x80080002, 0x80ff0001, 
    0x00000005, 0x80080001, 0x80ff0002, 0x80ff0001, 0x00000004, 0x80080001, 
    0x80ff0001, 0xa0000010, 0xa1550010, 0x00000002, 0xd0080000, 0x81ff0001, 
    0xa000000e, 0x0000ffff
};

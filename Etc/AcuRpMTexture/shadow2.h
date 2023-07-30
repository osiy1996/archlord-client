#if 0
    vs_1_1
    dcl_position v0
    dcl_normal v1
    dcl_color v2
    dcl_texcoord v3
    dcl_texcoord1 v4
    dcl_texcoord2 v5
    dcl_texcoord3 v6
    def c14, 1, 0, 1, 0
    m4x4 r0, v0, c0
    mov oPos, r0
    dp4 r0.x, v0, c15
    mad r0.z, -r0.x, c7.y, c7.x
    dp3 r1.x, -c4, v1
    sge r1.x, r1.x, c14.y
    mul r7.w, r0.z, r1.x
    mov oFog, c14.x
    m4x4 r0, v0, c8
    dp4 r1.x, r0, c12
    dp4 r1.y, r0, c13
    mov oT0.xy, r1.xyyy
    mov oD0.xyz, r7.w
    mov oD0.w, c14.x

// approximately 20 instruction slots used
#endif

const DWORD g_vs11_shadow[] =
{
    0xfffe0101, 0x0000001f, 0x80000000, 0x900f0000, 0x0000001f, 0x80000003, 
    0x900f0001, 0x0000001f, 0x8000000a, 0x900f0002, 0x0000001f, 0x80000005, 
    0x900f0003, 0x0000001f, 0x80010005, 0x900f0004, 0x0000001f, 0x80020005, 
    0x900f0005, 0x0000001f, 0x80030005, 0x900f0006, 0x00000051, 0xa00f000e, 
    0x3f800000, 0x00000000, 0x3f800000, 0x00000000, 0x00000014, 0x800f0000, 
    0x90e40000, 0xa0e40000, 0x00000001, 0xc00f0000, 0x80e40000, 0x00000009, 
    0x80010000, 0x90e40000, 0xa0e4000f, 0x00000004, 0x80040000, 0x81000000, 
    0xa0550007, 0xa0000007, 0x00000008, 0x80010001, 0xa1e40004, 0x90e40001, 
    0x0000000d, 0x80010001, 0x80000001, 0xa055000e, 0x00000005, 0x80080007, 
    0x80aa0000, 0x80000001, 0x00000001, 0xc00f0001, 0xa000000e, 0x00000014, 
    0x800f0000, 0x90e40000, 0xa0e40008, 0x00000009, 0x80010001, 0x80e40000, 
    0xa0e4000c, 0x00000009, 0x80020001, 0x80e40000, 0xa0e4000d, 0x00000001, 
    0xe0030000, 0x80540001, 0x00000001, 0xd0070000, 0x80ff0007, 0x00000001, 
    0xd0080000, 0xa000000e, 0x0000ffff
};

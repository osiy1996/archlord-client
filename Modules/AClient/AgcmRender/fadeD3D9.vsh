vs_1_1

dcl_position0 v0
dcl_normal0 v1
;dcl_color0	v2
dcl_texcoord0	v3

//c0 - c3 : composed world-view-projection matrix
//c4: fade color
//c5: direct light dir
//c6: direct light color
//c7: ambient light color
//c8: fog constant
//c9: x(1.0f),y(0.0f)
//c10-c13: world inverse-transpose matrix
//c14: view matrix 3 row(for transform z to camera z)

def	 c9,1.0,0.0,0.1,0.0

dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 r0.w, v0, c3
mov oPos.w, r0.w

;fog out(exp..bug --;)
//mul r0.w,r0.w,c8.x
//expp r0.w,r0.w
//rcp oFog,r0.w

;Fog Out
dp4	r0.x,v0,c14
sub	r0.y,c8.x,r0.x			// (fog end - camera space Z)
mul	r0.z,r0.y,c8.y			// (fog end - camera space Z) / (fog end - fog start)

max	r0.w,c9.y,r0.z			// 0 �̻����� ����
min	r0.w,c9.x,r0.w			// 1 ���Ϸ� ����
mov	oFog,r0.w				// fog ���

//normal transform
m4x4 r0,v1,c10

//normal vector normalize
dp3  r0.w,r0,r0
rsq	 r0.w,r0.w
mul  r0,r0,r0.w

;add r1,c7,v2
mov r1,c7
dp3 r2.x,c5,r0
max r2.x,c9.y,-r2.x				// ��Ⱑ 0 �̸��̸� 0���� ����
mad r3,c6,r2.x,r1
mov r3.w,c4.w
min	oD0,r3,c9.x					// 1���� ũ�� �ʰ� ����

;add r0,c7,v2
;dp3 r1.x,c5,v1
;max r1.x,c9.y,-r1.x				// ��Ⱑ 0 �̸��̸� 0���� ����
;mad r2,c6,r1.x,r0
;mov r2.w,c4.w
;min	oD0,r2,c9.x					// 1���� ũ�� �ʰ� ����

mov oT0.xy,v3.xy










// Linear Fog ��� BackUp ( FxCard���� ĳ���� ������.. )
//c8: x(fog end),y(1/fog end - fog start),z(1.0f),w(0.0f)
//c10: world-view transposed matrix 3 row(for camera space z calculate)z
;m4x4	oPos,v0,c0

//linear fog calculate
;dp3		r0.x,v0,c10
;sub		r0.y,c8.x,r0.x			// (fog end - camera space Z)
;mul		r0.z,r0.y,c8.y			// (fog end - camera space Z) / (fog end - fog start)

;max		r0.w,c9.y,r0.z			// 0 �̻����� ����
;min		r0.w,c9.x,r0.w			// 1 ���Ϸ� ����
;mov		oFog,r0.w				// fog ���